/* Copyright 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DUMB_NET_URL_CONTEXT_H_
#define DUMB_NET_URL_CONTEXT_H_

#include <memory>
#include <set>
#include <string>

#include "net/base/network_isolation_key.h"
#include "net/http/http_request_headers.h"
#include "net/http/http_response_headers.h"
#include "net/url_request/referrer_policy.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
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
using ResponseCallback = base::RepeatingCallback<void()>;
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
  std::string method;
  GURL request_url;
  GURL tab_origin;
  GURL tab_url;
  GURL initiator_url;

  bool internal_redirect = false;
  GURL redirect_source;

  GURL referrer;
  net::ReferrerPolicy referrer_policy =
      net::ReferrerPolicy::CLEAR_ON_TRANSITION_FROM_SECURE_TO_INSECURE;
  base::Optional<GURL> new_referrer;

  std::string new_url_spec;
  // TODO(iefremov): rename to shields_up.
  bool allow_brave_shields = true;
  bool allow_ads = false;
  bool allow_http_upgradable_resource = false;
  bool allow_referrers = false;
  bool is_webtorrent_disabled = false;
  int render_process_id = 0;
  int frame_tree_node_id = 0;
  uint64_t request_identifier = 0;
  size_t next_url_request_index = 0;

  content::BrowserContext* browser_context = nullptr;
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
  std::string mock_data_url;
  GURL ipfs_gateway_url;
  bool ipfs_auto_fallback = false;

  bool ShouldMockRequest() const { return !mock_data_url.empty(); }

  net::NetworkIsolationKey network_isolation_key = net::NetworkIsolationKey();

  // Default to invalid type for resource_type, so delegate helpers
  // can properly detect that the info couldn't be obtained.
  // TODO(iefremov): Replace with something like |WebRequestResourceType| to
  // distinguish WebSockets.
  static constexpr blink::mojom::ResourceType kInvalidResourceType =
      static_cast<blink::mojom::ResourceType>(-1);
  blink::mojom::ResourceType resource_type = kInvalidResourceType;

  std::string upload_data;

  static std::shared_ptr<dumb::DumbRequestInfo> MakeCTX(
      const network::ResourceRequest& request,
      int render_process_id,
      int frame_tree_node_id,
      uint64_t request_identifier,
      content::BrowserContext* browser_context,
      std::shared_ptr<dumb::DumbRequestInfo> old_ctx);

 private:
  // Please don't add any more friends here if it can be avoided.
  // We should also remove the one below.
  friend class ::DumbRequestHandler;

  GURL* new_url = nullptr;

  DISALLOW_COPY_AND_ASSIGN(DumbRequestInfo);
};

// ResponseListener
using OnBeforeURLRequestCallback =
    base::RepeatingCallback<int(const ResponseCallback& next_callback,
                                std::shared_ptr<DumbRequestInfo> ctx)>;
using OnBeforeStartTransactionCallback =
    base::RepeatingCallback<int(net::HttpRequestHeaders* headers,
                                const ResponseCallback& next_callback,
                                std::shared_ptr<DumbRequestInfo> ctx)>;
using OnHeadersReceivedCallback = base::RepeatingCallback<int(
    const net::HttpResponseHeaders* original_response_headers,
    scoped_refptr<net::HttpResponseHeaders>* override_response_headers,
    GURL* allowed_unsafe_redirect_url,
    const ResponseCallback& next_callback,
    std::shared_ptr<DumbRequestInfo> ctx)>;

}  // namespace dumb

#endif  // DUMB_NET_URL_CONTEXT_H_
