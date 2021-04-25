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

#include "base/component_export.h"
#include "dumb/services/network/url_purify/url_purify_default_rules.h"
#include "url/gurl.h"

namespace net {
class URLRequest;
}

namespace re2{
class RE2;
}

namespace dumb {

struct URLPurifyResult {
  URLPurifyResult(base::Optional<GURL> new_url, int count);
  ~URLPurifyResult();

  URLPurifyResult(const URLPurifyResult&);

  base::Optional<GURL> new_url;
  int count;
};

struct QueryMatcher {
  explicit QueryMatcher(const MatcherRule& rule);

  QueryMatcher(QueryMatcher&&);

  ~QueryMatcher();

  std::unique_ptr<re2::RE2> url_matcher;
  base::Optional<re2::RE2*> url_exception_matcher;
  std::vector<std::unique_ptr<re2::RE2>> query_matchers;
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

private:
  base::Optional<int> TryApplyMatcher(const QueryMatcher& matcher,
                                      bool is_global,
                                      const std::string& full_url,
                                      std::string& new_query) const;

  bool enabled_;
  QueryMatcher global_rules_matcher_;
  std::vector<QueryMatcher> per_site_matchers_;
};

} // namespace dumb

#endif  // DUMB_SERVICES_NETWORK_URL_PURIFY_URL_PURIFY_DELEGATE_H_
