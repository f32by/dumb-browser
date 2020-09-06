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

#include "dumb/browser/net/url_context.h"

#include <memory>
#include <string>

#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/isolation_info.h"
#include "services/network/public/cpp/resource_request.h"

namespace dumb {

namespace {

std::string GetUploadData(const network::ResourceRequest& request) {
  std::string upload_data;
  if (!request.request_body) {
    return {};
  }
  const auto* elements = request.request_body->elements();
  for (const network::DataElement& element : *elements) {
    if (element.type() == network::mojom::DataElementType::kBytes) {
      upload_data.append(element.bytes(), element.length());
    }
  }

  return upload_data;
}

}  // namespace

DumbRequestInfo::DumbRequestInfo() = default;

DumbRequestInfo::DumbRequestInfo(const GURL& url) : request_url(url) {}

DumbRequestInfo::~DumbRequestInfo() = default;

// static
void DumbRequestInfo::FillCTX(const network::ResourceRequest& request,
                               int render_process_id,
                               int frame_tree_node_id,
                               uint64_t request_identifier,
                               content::BrowserContext* browser_context,
                               std::shared_ptr<dumb::DumbRequestInfo> ctx) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  ctx->request_identifier = request_identifier;
  ctx->request_url = request.url;
  // TODO(iefremov): Replace GURL with Origin
  ctx->initiator_url =
      request.request_initiator.value_or(url::Origin()).GetURL();

  ctx->referrer = request.referrer;
  ctx->referrer_policy = request.referrer_policy;

  ctx->resource_type =
      static_cast<blink::mojom::ResourceType>(request.resource_type);

  ctx->is_webtorrent_disabled =
// #if BUILDFLAG(ENABLE_BRAVE_WEBTORRENT)
//       !webtorrent::IsWebtorrentEnabled(browser_context);
// #else
      true;
// #endif

  ctx->render_frame_id = request.render_frame_id;
  ctx->render_process_id = render_process_id;
  ctx->frame_tree_node_id = frame_tree_node_id;

  // TODO(iefremov): remove tab_url. Change tab_origin from GURL to Origin.
  // ctx->tab_url = request.top_frame_origin;
  if (request.trusted_params) {
    // TODO(iefremov): Turns out it provides us a not expected value for
    // cross-site top-level navigations. Fortunately for now it is not a problem
    // for shields functionality. We should reconsider this machinery, also
    // given that this is always empty for subresources.
    ctx->tab_origin =
        request.trusted_params->isolation_info.network_isolation_key()
            .GetTopFrameOrigin()
            .value_or(url::Origin())
            .GetURL();
  }
  // TODO(iefremov): We still need this for WebSockets, currently
  // |AddChannelRequest| provides only old-fashioned |site_for_cookies|.
  // (See |BraveProxyingWebSocket|).
  // if (ctx->tab_origin.is_empty()) {
    // ctx->tab_origin = brave_shields::BraveShieldsWebContentsObserver::
    //                       GetTabURLFromRenderFrameInfo(ctx->render_process_id,
    //                                                    ctx->render_frame_id,
    //                                                    ctx->frame_tree_node_id)
    //                           .GetOrigin();
  // }

  Profile* profile = Profile::FromBrowserContext(browser_context);
  auto* map = HostContentSettingsMapFactory::GetForProfile(profile);
  // ctx->allow_brave_shields =
  //     brave_shields::GetBraveShieldsEnabled(map, ctx->tab_origin);
  // ctx->allow_ads = brave_shields::GetAdControlType(
  //     map, ctx->tab_origin) == brave_shields::ControlType::ALLOW;
  // ctx->allow_http_upgradable_resource =
  //     !brave_shields::GetHTTPSEverywhereEnabled(map, ctx->tab_origin);
  // ctx->allow_referrers = brave_shields::AllowReferrers(map, ctx->tab_origin);
  ctx->upload_data = GetUploadData(request);
}

}  // namespace dumb
