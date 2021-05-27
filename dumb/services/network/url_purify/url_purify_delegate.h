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

#ifndef DUMB_SERVICES_NETWORK_URL_PURIFY_URL_PURIFY_DELEGATE_H_
#define DUMB_SERVICES_NETWORK_URL_PURIFY_URL_PURIFY_DELEGATE_H_

#include <memory>

#include "base/bind.h"
#include "base/component_export.h"
#include "base/containers/span.h"
#include "url/gurl.h"

namespace net {
class URLRequest;
}

namespace re2 {
class RE2;
}

struct URLPurifyRule;

namespace dumb {

struct URLPurifyResult {
  URLPurifyResult(base::Optional<GURL> new_url, int count);
  ~URLPurifyResult();

  URLPurifyResult(const URLPurifyResult&);

  base::Optional<GURL> new_url;
  int count;
};

class QueryMatcher {
public:
  explicit QueryMatcher(const URLPurifyRule& rule);
  QueryMatcher(QueryMatcher&&);
  ~QueryMatcher();

  const std::string& name() const {
    return name_;
  }

  const re2::RE2* url_matcher() const {
    return url_matcher_.get();
  }

  const base::Optional<re2::RE2*>& url_exception_matcher() const {
    return url_exception_matcher_;
  }

  const std::vector<std::unique_ptr<re2::RE2>>& query_matchers() const {
    return query_matchers_;
  }

private:
  std::string name_;
  std::unique_ptr<re2::RE2> url_matcher_;
  base::Optional<re2::RE2*> url_exception_matcher_;
  std::vector<std::unique_ptr<re2::RE2>> query_matchers_;
};

class COMPONENT_EXPORT(NETWORK_SERVICE) URLPurifyDelegate {
public:
  URLPurifyDelegate();

  ~URLPurifyDelegate();

  URLPurifyResult TruncateURLParameters(net::URLRequest* const request,
                                        const GURL& effective_url) const;

  void SetEnabled(bool enabled) {
    enabled_ = enabled;
  }

  void OnNewRules(base::span<const uint8_t> purify_rules,
                  base::OnceClosure callback);

private:
  int TryApplyMatcher(const QueryMatcher& matcher,
                      bool is_global,
                      const std::string& full_url,
                      std::string& new_query) const;

  bool enabled_;
  QueryMatcher global_rules_matcher_;
  std::vector<QueryMatcher> per_site_matchers_;
};

} // namespace dumb

#endif  // DUMB_SERVICES_NETWORK_URL_PURIFY_URL_PURIFY_DELEGATE_H_
