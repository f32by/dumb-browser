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

#ifndef DUMB_SERVICES_NETWORK_URL_PURIFY_DELEGATE_HELPER_H_
#define DUMB_SERVICES_NETWORK_URL_PURIFY_DELEGATE_HELPER_H_

#include <memory>

#include "base/optional.h"
#include "url/gurl.h"

namespace net {
class URLRequest;
}

namespace dumb {

struct URLPurifyResult {
  URLPurifyResult(base::Optional<GURL> new_url, int count);
  ~URLPurifyResult();

  URLPurifyResult(const URLPurifyResult&);

  base::Optional<GURL> new_url;
  int count;
};

URLPurifyResult::URLPurifyResult(base::Optional<GURL> new_url, int count):
    new_url(new_url), count(count) {}

URLPurifyResult::URLPurifyResult(const URLPurifyResult& other) {
  new_url = other.new_url;
  count = other.count;
}

URLPurifyResult::~URLPurifyResult() = default;

URLPurifyResult MaybeTruncateURLParameters(net::URLRequest* const request,
                                           const GURL& effective_url);

}  // namespace dumb

#endif  // DUMB_SERVICES_NETWORK_URL_PURIFY_DELEGATE_HELPER_H_
