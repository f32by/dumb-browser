/* Copyright (c) 2021 The Dumb Browser Authors. All rights reserved.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "dumb/services/network/url_purify/url_purify_delegate.h"

// #include <iostream>

#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/strings/string_util.h"
#include "base/synchronization/lock.h"
#include "dumb/services/network/url_purify/url_purify_default_rules.h"
#include "net/url_request/url_request.h"
#include "third_party/re2/src/re2/re2.h"

#define RE2UNANCHORED re2::RE2::Anchor::UNANCHORED

namespace dumb {

URLPurifyResult::URLPurifyResult(base::Optional<GURL> new_url, int count)
    : new_url(new_url), count(count) {}

URLPurifyResult::URLPurifyResult(const URLPurifyResult& other) = default;

URLPurifyResult::~URLPurifyResult() = default;

QueryMatcher::QueryMatcher(const MatcherRule& rule) {
  const auto& url_pattern = rule.url_pattern;
  const auto& url_exceptions = rule.url_exceptions;
  const auto& query_patterns = rule.query_patterns;

  // LOG(INFO) << "Building matchers...";
  re2::RE2::Options options;
  options.set_case_sensitive(false);
  // build url_matcher
  auto* url_matcher = new re2::RE2(url_pattern, options);
  DCHECK(url_matcher->ok());
  // build exceptions matcher
  base::Optional<re2::RE2*> url_exception_matcher;
  if(url_exceptions) {
    const std::string exception_pattern(
      base::JoinString(url_exceptions.value(), "|"));
    url_exception_matcher = new re2::RE2(exception_pattern, options);
    DCHECK(url_exception_matcher.value()->ok());
  }
  // build query matcher
  std::vector<std::unique_ptr<re2::RE2>> query_matchers;
  const std::string query_pattern(base::JoinString(query_patterns, "|"));

  // appended matcher
  auto* query_appended_matcher = new re2::RE2(
    "^(" + query_pattern + ")=[^&]+$", options);
  DCHECK(query_appended_matcher->ok());
  query_matchers.emplace_back(query_appended_matcher);

  // first matcher
  auto* query_first_matcher = new re2::RE2(
    "^(" + query_pattern + ")=[^&]+&", options);
  DCHECK(query_first_matcher->ok());
  query_matchers.emplace_back(query_first_matcher);

  // only matcher
  auto* query_only_matcher = new re2::RE2(
    "&(" + query_pattern + ")=[^&]+", options);
  DCHECK(query_only_matcher->ok());
  query_matchers.emplace_back(query_only_matcher);
}

QueryMatcher::QueryMatcher(QueryMatcher&&) = default;

QueryMatcher::~QueryMatcher() = default;

URLPurifyDelegate::URLPurifyDelegate()
    : global_rules_matcher_(GetDefaultGlobalRules()) {
  for (const auto& r : GetDefaultPerSiteRules()) {
    per_site_matchers_.emplace_back(QueryMatcher(r));
  }
}

URLPurifyDelegate::~URLPurifyDelegate() = default;

URLPurifyResult
URLPurifyDelegate::TruncateURLParameters(net::URLRequest* const request,
                                        const GURL& effective_url) const {
  if (request->url().has_query()) {
    std::string new_query = request->url().query();
    const std::string& full_url = request->url().spec();
    int count = 0;

    // Apply global rules.
    count +=
        TryApplyMatcher(global_rules_matcher_,
            true, full_url, new_query).value();

    // Apply per-site rules.
    for(const auto& matcher: per_site_matchers_) {
      auto result = TryApplyMatcher(matcher, false, full_url, new_query);
      if(result.has_value()) {
        if(result.value() == -1) {
          // Not match
          break;
        }
        count += result.value();
        break;
      }
    }

    if (count > 0) {
      url::Replacements<char> replacements;
      if (new_query.empty()) {
        replacements.ClearQuery();
      } else {
        replacements.SetQuery(new_query.c_str(),
                              url::Component(0, new_query.size()));
      }
      auto new_url = request->url().ReplaceComponents(replacements);
      return {new_url, count};
    }
  }

  return {base::nullopt, 0};
}

base::Optional<int> URLPurifyDelegate::TryApplyMatcher(
    const QueryMatcher& matcher,
    bool is_global,
    const std::string& full_url,
    std::string& new_query) const {
  LOG(INFO) << "Original URL: " << full_url;

  int count = 0;
  const auto url_len = full_url.length() - 1;

  // Skip if match any exception.
  if(!is_global && matcher.url_exception_matcher.has_value() &&
      matcher.url_exception_matcher.value()->Match(
          full_url, 0, url_len, RE2UNANCHORED, nullptr, 0)) {
    LOG(INFO) << "Met an exception, skipping.";
    return -1;
  }

  // filter
  for(const auto& query_matcher: matcher.query_matchers) {
    count += re2::RE2::GlobalReplace(&new_query, *query_matcher.get(), "");
  }

  LOG(INFO) << "Removed " << count
      << " parameters. New spec: " << new_query;

  return count;
}

} // namespace dumb
