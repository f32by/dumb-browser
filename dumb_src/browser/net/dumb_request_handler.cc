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

#include "dumb/browser/net/dumb_request_handler.h"

#include <algorithm>
#include <utility>

#include "base/task/post_task.h"
#include "chrome/browser/browser_process.h"
#include "components/prefs/pref_change_registrar.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/url_constants.h"
#include "dumb/browser/net/dumb_stp_util.h"
#include "extensions/common/constants.h"

static bool IsInternalScheme(std::shared_ptr<dumb::DumbRequestInfo> ctx) {
  DCHECK(ctx);
  return ctx->request_url.SchemeIs(extensions::kExtensionScheme) ||
         ctx->request_url.SchemeIs(content::kChromeUIScheme);
}

DumbRequestHandler::DumbRequestHandler() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  SetupCallbacks();
}

DumbRequestHandler::~DumbRequestHandler() = default;

void DumbRequestHandler::SetupCallbacks() {
  // dumb::OnBeforeURLRequestCallback callback =
  //     base::Bind(dumb::OnBeforeURLRequest_URLPurifyWork);
  // before_url_request_callbacks_.push_back(callback);
}

void DumbRequestHandler::InitPrefChangeRegistrar() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
}

void DumbRequestHandler::OnReferralHeadersChanged() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
}

bool DumbRequestHandler::IsRequestIdentifierValid(
    uint64_t request_identifier) {
  return base::Contains(callbacks_, request_identifier);
}

int DumbRequestHandler::OnBeforeURLRequest(
    std::shared_ptr<dumb::DumbRequestInfo> ctx,
    net::CompletionOnceCallback callback,
    GURL* new_url) {
  if (before_url_request_callbacks_.empty() || IsInternalScheme(ctx)) {
    return net::OK;
  }

  ctx->new_url = new_url;
  ctx->event_type = dumb::kOnBeforeRequest;
  callbacks_[ctx->request_identifier] = std::move(callback);
  RunNextCallback(ctx);

  return net::ERR_IO_PENDING;
}

int DumbRequestHandler::OnBeforeStartTransaction(
    std::shared_ptr<dumb::DumbRequestInfo> ctx,
    net::CompletionOnceCallback callback,
    net::HttpRequestHeaders* headers) {
  return net::OK;
}

int DumbRequestHandler::OnHeadersReceived(
    std::shared_ptr<dumb::DumbRequestInfo> ctx,
    net::CompletionOnceCallback callback,
    const net::HttpResponseHeaders* original_response_headers,
    scoped_refptr<net::HttpResponseHeaders>* override_response_headers,
    GURL* allowed_unsafe_redirect_url) {
  if (!ctx->tab_origin.is_empty()) {
    dumb::RemoveTrackableSecurityHeadersForThirdParty(
        ctx->request_url, url::Origin::Create(ctx->tab_origin),
        original_response_headers, override_response_headers);
  }

  if (headers_received_callbacks_.empty() &&
      !ctx->request_url.SchemeIs(content::kChromeUIScheme)) {
    // Extension scheme not excluded since brave_webtorrent needs it.
    return net::OK;
  }

  callbacks_[ctx->request_identifier] = std::move(callback);
  ctx->event_type = dumb::kOnHeadersReceived;
  ctx->original_response_headers = original_response_headers;
  ctx->override_response_headers = override_response_headers;
  ctx->allowed_unsafe_redirect_url = allowed_unsafe_redirect_url;

  RunNextCallback(ctx);
  return net::ERR_IO_PENDING;
}

void DumbRequestHandler::OnURLRequestDestroyed(
    std::shared_ptr<dumb::DumbRequestInfo> ctx) {
  if (base::Contains(callbacks_, ctx->request_identifier)) {
    callbacks_.erase(ctx->request_identifier);
  }
}

void DumbRequestHandler::RunCallbackForRequestIdentifier(
    uint64_t request_identifier,
    int rv) {
  std::map<uint64_t, net::CompletionOnceCallback>::iterator it =
      callbacks_.find(request_identifier);
  // We intentionally do the async call to maintain the proper flow
  // of URLLoader callbacks.
  base::PostTask(FROM_HERE, {content::BrowserThread::UI},
                 base::BindOnce(std::move(it->second), rv));
}

void DumbRequestHandler::RunNextCallback(
    std::shared_ptr<dumb::DumbRequestInfo> ctx) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  if (!base::Contains(callbacks_, ctx->request_identifier)) {
    return;
  }

  // Continue processing callbacks until we hit one that returns PENDING
  int rv = net::OK;

  if (ctx->event_type == dumb::kOnBeforeRequest) {
    while (before_url_request_callbacks_.size() !=
           ctx->next_url_request_index) {
      dumb::OnBeforeURLRequestCallback callback =
          before_url_request_callbacks_[ctx->next_url_request_index++];
      dumb::ResponseCallback next_callback = base::Bind(
          &DumbRequestHandler::RunNextCallback,
          weak_factory_.GetWeakPtr(),
          ctx);
      rv = callback.Run(next_callback, ctx);
      if (rv == net::ERR_IO_PENDING) {
        return;
      }
      if (rv != net::OK) {
        break;
      }
    }
  }

  if (rv != net::OK) {
    RunCallbackForRequestIdentifier(ctx->request_identifier, rv);
    return;
  }

  if (ctx->event_type == dumb::kOnBeforeRequest) {
    if (!ctx->new_url_spec.empty() &&
        (ctx->new_url_spec != ctx->request_url.spec()) &&
        IsRequestIdentifierValid(ctx->request_identifier)) {
      *ctx->new_url = GURL(ctx->new_url_spec);
    }
  }

  RunCallbackForRequestIdentifier(ctx->request_identifier, rv);
}
