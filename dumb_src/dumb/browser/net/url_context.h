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

#ifndef DUMB_NET_URL_CONTEXT_H_
#define DUMB_NET_URL_CONTEXT_H_

#include <memory>
#include <set>
#include <string>

#include "net/url_request/url_request.h"
#include "third_party/blink/public/mojom/loader/resource_load_info.mojom-shared.h"
#include "url/gurl.h"

class DumbRequestHandler;

namespace content {
class BrowserContext;
}

namespace network {
struct ResourceRequest;
}

namespace dumb {
struct DumbRequestInfo;
using ResponseCallback = base::Callback<void()>;
}  // namespace dumb

namespace dumb {

enum DumbNetworkDelegateEventType {
  kOnBeforeRequest,
  kOnBeforeStartTransaction,
  kOnHeadersReceived,
  kOnCanGetCookies,
  kOnCanSetCookies,
  kUnknownEventType
};

enum BlockedBy { kNotBlocked, kAdBlocked, kOtherBlocked };

struct DumbRequestInfo {
  DumbRequestInfo();

  // For tests, should not be used directly.
  explicit DumbRequestInfo(const GURL& url);

  ~DumbRequestInfo();
  GURL request_url;
  GURL tab_origin;
  GURL tab_url;
  GURL initiator_url;

  GURL referrer;
  net::ReferrerPolicy referrer_policy =
      net::ReferrerPolicy::
          CLEAR_ON_TRANSITION_FROM_SECURE_TO_INSECURE;
  base::Optional<GURL> new_referrer;

  std::string new_url_spec;
  bool allow_brave_shields = true;
  bool allow_ads = false;
  bool allow_http_upgradable_resource = false;
  bool allow_referrers = false;
  bool is_webtorrent_disabled = false;
  int render_process_id = 0;
  int render_frame_id = 0;
  int frame_tree_node_id = 0;
  uint64_t request_identifier = 0;
  size_t next_url_request_index = 0;

  net::HttpRequestHeaders* headers = nullptr;
  // The following two sets are populated by |OnBeforeStartTransactionCallback|.
  // |set_headers| contains headers which values were added or modified.
  std::set<std::string> set_headers;
  std::set<std::string> removed_headers;
  const net::HttpResponseHeaders* original_response_headers = nullptr;
  scoped_refptr<net::HttpResponseHeaders>* override_response_headers = nullptr;

  GURL* allowed_unsafe_redirect_url = nullptr;
  DumbNetworkDelegateEventType event_type = kUnknownEventType;
  const base::ListValue* referral_headers_list = nullptr;
  BlockedBy blocked_by = kNotBlocked;
  bool cancel_request_explicitly = false;
  std::string mock_data_url;

  // Default to invalid type for resource_type, so delegate helpers
  // can properly detect that the info couldn't be obtained.
  // TODO(iefremov): Replace with something like |WebRequestResourceType| to
  // distinguish WebSockets.
  static constexpr blink::mojom::ResourceType kInvalidResourceType =
      static_cast<blink::mojom::ResourceType>(-1);
  blink::mojom::ResourceType resource_type = kInvalidResourceType;

  std::string upload_data;

  static void FillCTX(const network::ResourceRequest& request,
                      int render_process_id,
                      int frame_tree_node_id,
                      uint64_t request_identifier,
                      content::BrowserContext* browser_context,
                      std::shared_ptr<dumb::DumbRequestInfo> ctx);

 private:
  // Please don't add any more friends here if it can be avoided.
  // We should also remove the one below.
  friend class ::DumbRequestHandler;

  GURL* new_url = nullptr;

  DISALLOW_COPY_AND_ASSIGN(DumbRequestInfo);
};

// ResponseListener
using OnBeforeURLRequestCallback =
    base::Callback<int(const ResponseCallback& next_callback,
                       std::shared_ptr<DumbRequestInfo> ctx)>;
using OnBeforeStartTransactionCallback =
    base::Callback<int(net::HttpRequestHeaders* headers,
                       const ResponseCallback& next_callback,
                       std::shared_ptr<DumbRequestInfo> ctx)>;
using OnHeadersReceivedCallback = base::Callback<int(
    const net::HttpResponseHeaders* original_response_headers,
    scoped_refptr<net::HttpResponseHeaders>* override_response_headers,
    GURL* allowed_unsafe_redirect_url,
    const ResponseCallback& next_callback,
    std::shared_ptr<DumbRequestInfo> ctx)>;

}  // namespace dumb

#endif  // DUMB_NET_URL_CONTEXT_H_
