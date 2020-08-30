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

#include "dumb_browser/browser/net/dumb_request_handler.h"

#include <algorithm>
#include <utility>

#include "base/task/post_task.h"
// #include "brave/common/pref_names.h"
// #include "brave/components/brave_referrals/buildflags/buildflags.h"
// #include "brave/components/brave_rewards/browser/buildflags/buildflags.h"
// #include "brave/components/brave_webtorrent/browser/buildflags/buildflags.h"
#include "chrome/browser/browser_process.h"
#include "components/prefs/pref_change_registrar.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/url_constants.h"
#include "dumb_browser/browser/net/dumb_stp_util.h"
#include "dumb_browser/browser/net/url_purify/dumb_url_purify_delegate_helper.h"
#include "extensions/common/constants.h"

// #if BUILDFLAG(ENABLE_BRAVE_REFERRALS)
// #include "brave/browser/net/brave_referrals_network_delegate_helper.h"
// #endif

// #if BUILDFLAG(BRAVE_REWARDS_ENABLED)
// #include "brave/components/brave_rewards/browser/net/network_delegate_helper.h"
// #endif

// #if BUILDFLAG(ENABLE_BRAVE_WEBTORRENT)
// #include "brave/components/brave_webtorrent/browser/net/brave_torrent_redirect_network_delegate_helper.h"
// #endif

// #if BUILDFLAG(ENABLE_BRAVE_TRANSLATE_GO)
// #include "brave/browser/net/brave_translate_redirect_network_delegate_helper.h"
// #endif

static bool IsInternalScheme(std::shared_ptr<dumb::DumbRequestInfo> ctx) {
  DCHECK(ctx);
  return ctx->request_url.SchemeIs(extensions::kExtensionScheme) ||
         ctx->request_url.SchemeIs(content::kChromeUIScheme);
}

DumbRequestHandler::DumbRequestHandler() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  SetupCallbacks();
  // Initialize the preference change registrar.
  InitPrefChangeRegistrar();
}

DumbRequestHandler::~DumbRequestHandler() = default;

void DumbRequestHandler::SetupCallbacks() {
  dumb::OnBeforeURLRequestCallback callback =
      base::Bind(dumb::OnBeforeURLRequest_URLPurifyWork);
  before_url_request_callbacks_.push_back(callback);

  // callback = base::Bind(brave::OnBeforeURLRequest_AdBlockTPPreWork);
  // before_url_request_callbacks_.push_back(callback);

  // callback = base::Bind(brave::OnBeforeURLRequest_HttpsePreFileWork);
  // before_url_request_callbacks_.push_back(callback);

  // callback = base::Bind(brave::OnBeforeURLRequest_CommonStaticRedirectWork);
  // before_url_request_callbacks_.push_back(callback);

// #if BUILDFLAG(BRAVE_REWARDS_ENABLED)
//   callback = base::Bind(brave_rewards::OnBeforeURLRequest);
//   before_url_request_callbacks_.push_back(callback);
// #endif

// #if BUILDFLAG(ENABLE_BRAVE_TRANSLATE_GO)
//   callback =
//       base::BindRepeating(brave::OnBeforeURLRequest_TranslateRedirectWork);
//   before_url_request_callbacks_.push_back(callback);
// #endif

  dumb::OnBeforeStartTransactionCallback start_transaction_callback =
      base::Bind(dumb::OnBeforeStartTransaction_URLPurifyWork);
  before_start_transaction_callbacks_.push_back(start_transaction_callback);

// #if BUILDFLAG(ENABLE_BRAVE_REFERRALS)
//   start_transaction_callback =
//       base::Bind(brave::OnBeforeStartTransaction_ReferralsWork);
//   before_start_transaction_callbacks_.push_back(start_transaction_callback);
// #endif

// #if BUILDFLAG(ENABLE_BRAVE_WEBTORRENT)
//   brave::OnHeadersReceivedCallback headers_received_callback =
//       base::Bind(webtorrent::OnHeadersReceived_TorrentRedirectWork);
//   headers_received_callbacks_.push_back(headers_received_callback);
// #endif
}

void DumbRequestHandler::InitPrefChangeRegistrar() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
// #if BUILDFLAG(ENABLE_BRAVE_REFERRALS)
//   PrefService* prefs = g_browser_process->local_state();
//   pref_change_registrar_.reset(new PrefChangeRegistrar());
//   pref_change_registrar_->Init(prefs);
//   pref_change_registrar_->Add(
//       kReferralHeaders,
//       base::Bind(&BraveRequestHandler::OnReferralHeadersChanged,
//                  base::Unretained(this)));
//   // Retrieve current referral headers, if any.
//   OnReferralHeadersChanged();
// #endif
}

void DumbRequestHandler::OnReferralHeadersChanged() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  // if (const base::ListValue* referral_headers =
  //         g_browser_process->local_state()->GetList(kReferralHeaders)) {
  //   referral_headers_list_.reset(referral_headers->DeepCopy());
  // }
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
  if (before_start_transaction_callbacks_.empty() || IsInternalScheme(ctx)) {
    return net::OK;
  }

  ctx->event_type = dumb::kOnBeforeStartTransaction;
  ctx->headers = headers;
  ctx->referral_headers_list = referral_headers_list_.get();
  callbacks_[ctx->request_identifier] = std::move(callback);
  RunNextCallback(ctx);

  return net::ERR_IO_PENDING;
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

// TODO(iefremov): Merge all callback containers into one and run only one loop
// instead of many (issues/5574).
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
  } else if (ctx->event_type == dumb::kOnBeforeStartTransaction) {
    while (before_start_transaction_callbacks_.size() !=
           ctx->next_url_request_index) {
      dumb::OnBeforeStartTransactionCallback callback =
          before_start_transaction_callbacks_[ctx->next_url_request_index++];
      dumb::ResponseCallback next_callback = base::Bind(
          &DumbRequestHandler::RunNextCallback,
          weak_factory_.GetWeakPtr(),
          ctx);
      rv = callback.Run(ctx->headers, next_callback, ctx);
      if (rv == net::ERR_IO_PENDING) {
        return;
      }
      if (rv != net::OK) {
        break;
      }
    }
  } else if (ctx->event_type == dumb::kOnHeadersReceived) {
    while (headers_received_callbacks_.size() != ctx->next_url_request_index) {
      dumb::OnHeadersReceivedCallback callback =
          headers_received_callbacks_[ctx->next_url_request_index++];
      dumb::ResponseCallback next_callback = base::Bind(
          &DumbRequestHandler::RunNextCallback,
          weak_factory_.GetWeakPtr(),
          ctx);
      rv = callback.Run(ctx->original_response_headers,
                        ctx->override_response_headers,
                        ctx->allowed_unsafe_redirect_url, next_callback, ctx);
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
    if (ctx->blocked_by == dumb::kAdBlocked) {
      if (ctx->cancel_request_explicitly) {
        RunCallbackForRequestIdentifier(ctx->request_identifier,
                                        net::ERR_ABORTED);
        return;
      }
    }
  }

  RunCallbackForRequestIdentifier(ctx->request_identifier, rv);
}
