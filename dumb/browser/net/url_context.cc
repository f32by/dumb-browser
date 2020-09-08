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
  ctx->initiator_url =
      request.request_initiator.value_or(url::Origin()).GetURL();

  ctx->referrer = request.referrer;
  ctx->referrer_policy = request.referrer_policy;

  ctx->resource_type =
      static_cast<blink::mojom::ResourceType>(request.resource_type);

  ctx->is_webtorrent_disabled = true;

  ctx->render_frame_id = request.render_frame_id;
  ctx->render_process_id = render_process_id;
  ctx->frame_tree_node_id = frame_tree_node_id;

  if (request.trusted_params) {
    ctx->tab_origin =
        request.trusted_params->isolation_info.network_isolation_key()
            .GetTopFrameOrigin()
            .value_or(url::Origin())
            .GetURL();
  }

  // Profile* profile = Profile::FromBrowserContext(browser_context);
  ctx->upload_data = GetUploadData(request);
}

}  // namespace dumb
