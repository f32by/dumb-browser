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

// Note: this file was stolen from Brave.

#ifndef DUMB_NET_RESOURCE_CONTEXT_DATA_H_
#define DUMB_NET_RESOURCE_CONTEXT_DATA_H_

#include <cstdint>
#include <memory>
#include <set>
#include <string>

#include "base/containers/unique_ptr_adapters.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/supports_user_data.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/content_browser_client.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "services/network/public/mojom/websocket.mojom.h"

class DumbProxyingURLLoaderFactory;
class DumbProxyingWebSocket;
class DumbRequestHandler;

namespace content {
class BrowserContext;
}

// Used for both URLLoaders and WebSocket proxies.
class RequestIDGenerator
    : public base::RefCountedThreadSafe<RequestIDGenerator> {
 public:
  RequestIDGenerator() = default;
  int64_t Generate() {
    DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
    return ++id_;
  }

 private:
  friend class base::RefCountedThreadSafe<RequestIDGenerator>;
  ~RequestIDGenerator() {}

  // Although this initialization can be done in a thread other than the IO
  // thread, we expect at least one memory barrier before actually calling
  // Generate in the IO thread, so we don't protect the variable with a lock.
  int64_t id_ = 0;
  DISALLOW_COPY_AND_ASSIGN(RequestIDGenerator);
};

// Owns proxying factories for URLLoaders and websocket proxies. There is
// one |ResourceContextData| per profile.
class ResourceContextData : public base::SupportsUserData::Data {
 public:
  ~ResourceContextData() override;

  static void StartProxying(
      content::BrowserContext* browser_context,
      int render_process_id,
      int frame_tree_node_id,
      network::mojom::URLLoaderFactoryRequest request,
      network::mojom::URLLoaderFactoryPtrInfo target_factory);

  static DumbProxyingWebSocket* StartProxyingWebSocket(
      content::ContentBrowserClient::WebSocketFactory factory,
      const GURL& url,
      const net::SiteForCookies& site_for_cookies,
      const base::Optional<std::string>& user_agent,
      mojo::PendingRemote<network::mojom::WebSocketHandshakeClient>
          handshake_client,
      content::BrowserContext* browser_context,
      int render_process_id,
      int frame_id,
      int frame_tree_node_id,
      const url::Origin& origin);

  void RemoveProxy(DumbProxyingURLLoaderFactory* proxy);
  void RemoveProxyWebSocket(DumbProxyingWebSocket* proxy);

 private:
  ResourceContextData();

  std::unique_ptr<DumbRequestHandler> request_handler_;
  scoped_refptr<RequestIDGenerator> request_id_generator_;

  std::set<std::unique_ptr<DumbProxyingURLLoaderFactory>,
           base::UniquePtrComparator>
      proxies_;

  std::set<std::unique_ptr<DumbProxyingWebSocket>,
           base::UniquePtrComparator>
      websocket_proxies_;

  base::WeakPtrFactory<ResourceContextData> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(ResourceContextData);
};

#endif  // DUMB_NET_RESOURCE_CONTEXT_DATA_H_
