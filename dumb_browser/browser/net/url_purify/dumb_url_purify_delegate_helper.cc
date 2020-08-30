/* Copyright (c) 2020 The Dumb Browser Authors. All rights reserved.
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

#include "dumb_browser/browser/net/url_purify/dumb_url_purify_delegate_helper.h"

#include "dumb_browser/browser/net/url_purify/url_purify_rules.h"

namespace dumb {

namespace {

static void ApplyPotentialQueryStringFilter(const GURL& request_url,
                                            std::string* new_url_spec) {
  DCHECK(new_url_spec);
  std::string new_query = request_url.query();

  int replacement_count = ApplyQueryFilters(request_url, new_query);

  if (replacement_count > 0) {
    url::Replacements<char> replacements;
    if (new_query.empty()) {
      replacements.ClearQuery();
    } else {
      replacements.SetQuery(new_query.c_str(),
                            url::Component(0, new_query.size()));
    }
    *new_url_spec = request_url.ReplaceComponents(replacements).spec();
  }
}

}  // namespace

int OnBeforeURLRequest_URLPurifyWork(const ResponseCallback& next_callback,
                                     std::shared_ptr<DumbRequestInfo> ctx) {
  if (ctx->request_url.has_query()) {
    ApplyPotentialQueryStringFilter(ctx->request_url, &ctx->new_url_spec);
  }
  return net::OK;
}

int OnBeforeStartTransaction_URLPurifyWork(
    net::HttpRequestHeaders* headers,
    const ResponseCallback& next_callback,
    std::shared_ptr<DumbRequestInfo> ctx) {
  // if (IsUAWhitelisted(ctx->request_url)) {
  //   std::string user_agent;
  //   if (headers->GetHeader(kUserAgentHeader, &user_agent)) {
  //     // We do not want to modify the same UA multiple times - for instance,
  //     // during redirects.
  //     if (std::string::npos == user_agent.find("Brave")) {
  //       base::ReplaceFirstSubstringAfterOffset(&user_agent, 0, "Chrome",
  //                                              "Brave Chrome");
  //       headers->SetHeader(kUserAgentHeader, user_agent);
  //       ctx->set_headers.insert(kUserAgentHeader);
  //     }
  //   }
  // }
  return net::OK;
}

}  // namespace dumb
