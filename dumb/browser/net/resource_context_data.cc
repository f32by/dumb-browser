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

#include "dumb/browser/net/resource_context_data.h"

#include <memory>
#include <string>
#include <utility>

#include "dumb/browser/net/dumb_proxying_url_loader_factory.h"
#include "dumb/browser/net/dumb_proxying_web_socket.h"
#include "dumb/browser/net/dumb_request_handler.h"
#include "content/public/browser/browser_context.h"
#include "net/cookies/site_for_cookies.h"

// User data key for ResourceContextData.
const void* const kResourceContextUserDataKey = &kResourceContextUserDataKey;

ResourceContextData::ResourceContextData()
    : request_id_generator_(base::MakeRefCounted<RequestIDGenerator>()),
      weak_factory_(this) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
}

ResourceContextData::~ResourceContextData() = default;

// static
void ResourceContextData::StartProxying(
    content::BrowserContext* browser_context,
    int render_process_id,
    int frame_tree_node_id,
    mojo::PendingReceiver<network::mojom::URLLoaderFactory> receiver,
    mojo::PendingRemote<network::mojom::URLLoaderFactory> target_factory) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  auto* self = static_cast<ResourceContextData*>(
      browser_context->GetUserData(kResourceContextUserDataKey));
  if (!self) {
    self = new ResourceContextData();
    browser_context->SetUserData(kResourceContextUserDataKey,
                                  base::WrapUnique(self));
  }

  if (!self->request_handler_) {
    self->request_handler_ = std::make_unique<DumbRequestHandler>();
  }

  auto proxy = std::make_unique<DumbProxyingURLLoaderFactory>(
      self->request_handler_.get(), browser_context, render_process_id,
      frame_tree_node_id, std::move(receiver), std::move(target_factory),
      self->request_id_generator_,
      base::BindOnce(&ResourceContextData::RemoveProxy,
                     self->weak_factory_.GetWeakPtr()));

  self->proxies_.emplace(std::move(proxy));
}

void ResourceContextData::RemoveProxy(DumbProxyingURLLoaderFactory* proxy) {
  auto it = proxies_.find(proxy);
  DCHECK(it != proxies_.end());
  proxies_.erase(it);
}

// static
DumbProxyingWebSocket* ResourceContextData::StartProxyingWebSocket(
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
    const url::Origin& origin) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  auto* self = static_cast<ResourceContextData*>(
      browser_context->GetUserData(kResourceContextUserDataKey));
  if (!self) {
    self = new ResourceContextData();
    browser_context->SetUserData(kResourceContextUserDataKey,
                                  base::WrapUnique(self));
  }

  if (!self->request_handler_) {
    self->request_handler_ = std::make_unique<DumbRequestHandler>();
  }

  network::ResourceRequest request;
  request.url = url;
  // TODO(iefremov): site_for_cookies is not enough, we should find a way
  // to initialize NetworkIsolationKey.
  request.site_for_cookies = site_for_cookies;
  if (user_agent) {
    request.headers.SetHeader(net::HttpRequestHeaders::kUserAgent, *user_agent);
  }
  request.request_initiator = origin;
  request.render_frame_id = frame_id;

  auto proxy = std::make_unique<DumbProxyingWebSocket>(
      std::move(factory), request, std::move(handshake_client),
      render_process_id, frame_tree_node_id, browser_context,
      self->request_id_generator_, self->request_handler_.get(),
      base::BindOnce(&ResourceContextData::RemoveProxyWebSocket,
                     self->weak_factory_.GetWeakPtr()));

  auto* raw_proxy = proxy.get();
  self->websocket_proxies_.emplace(std::move(proxy));
  return raw_proxy;
}

void ResourceContextData::RemoveProxyWebSocket(DumbProxyingWebSocket* proxy) {
  auto it = websocket_proxies_.find(proxy);
  DCHECK(it != websocket_proxies_.end());
  websocket_proxies_.erase(it);
}
