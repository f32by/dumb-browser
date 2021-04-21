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

#ifndef DUMB_NET_DUMB_URL_PURIFY_DELEGATE_HELPER_H_
#define DUMB_NET_DUMB_URL_PURIFY_DELEGATE_HELPER_H_

#include <memory>

#include "dumb/browser/net/url_context.h"
#include "third_party/re2/src/re2/re2.h"

namespace net {
class URLRequest;
}

namespace dumb {

int OnBeforeURLRequest_URLPurifyWork(
    const ResponseCallback& next_callback,
    std::shared_ptr<DumbRequestInfo> ctx);

int OnBeforeStartTransaction_URLPurifyWork(
    net::HttpRequestHeaders* headers,
    const ResponseCallback& next_callback,
    std::shared_ptr<DumbRequestInfo> ctx);

}  // namespace dumb

#endif  // DUMB_NET_DUMB_URL_PURIFY_DELEGATE_HELPER_H_
