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
    if (element.type() == network::mojom::DataElementDataView::Tag::kBytes) {
      const auto& bytes = element.As<network::DataElementBytes>().bytes();
      upload_data.append(bytes.begin(), bytes.end());
    }
  }

  return upload_data;
}

}  // namespace

DumbRequestInfo::DumbRequestInfo() = default;

DumbRequestInfo::DumbRequestInfo(const GURL& url) : request_url(url) {}

DumbRequestInfo::~DumbRequestInfo() = default;

// static
std::shared_ptr<dumb::DumbRequestInfo> DumbRequestInfo::MakeCTX(
                               const network::ResourceRequest& request,
                               int render_process_id,
                               int frame_tree_node_id,
                               uint64_t request_identifier,
                               content::BrowserContext* browser_context,
                               std::shared_ptr<dumb::DumbRequestInfo> old_ctx) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  auto ctx = std::make_shared<dumb::DumbRequestInfo>();
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

// TODO(iefremov): remove tab_url. Change tab_origin from GURL to Origin.
  // ctx->tab_url = request.top_frame_origin;
  if (request.trusted_params) {
    // TODO(iefremov): Turns out it provides us a not expected value for
    // cross-site top-level navigations. Fortunately for now it is not a problem
    // for shields functionality. We should reconsider this machinery, also
    // given that this is always empty for subresources.
    ctx->network_isolation_key =
        request.trusted_params->isolation_info.network_isolation_key();
    ctx->tab_origin = request.trusted_params->isolation_info.top_frame_origin()
                          .value_or(url::Origin())
                          .GetURL();
  }

  if (old_ctx) {
    ctx->internal_redirect = old_ctx->internal_redirect;
    ctx->redirect_source = old_ctx->redirect_source;
  }

  // Profile* profile = Profile::FromBrowserContext(browser_context);
  ctx->upload_data = GetUploadData(request);

  ctx->browser_context = browser_context;

  // TODO(fmarier): remove this once the hacky code in
  // brave_proxying_url_loader_factory.cc is refactored. See
  // BraveProxyingURLLoaderFactory::InProgressRequest::UpdateRequestInfo().
  if (old_ctx) {
    ctx->internal_redirect = old_ctx->internal_redirect;
    ctx->redirect_source = old_ctx->redirect_source;
  }

  return ctx;
}

}  // namespace dumb