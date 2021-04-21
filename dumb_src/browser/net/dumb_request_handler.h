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

#ifndef DUMB_NET_DUMB_REQUEST_HANDLER_H_
#define DUMB_NET_DUMB_REQUEST_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "dumb/browser/net/url_context.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/completion_once_callback.h"

class PrefChangeRegistrar;

// Contains different network stack hooks (similar to capabilities of WebRequest
// API).
class DumbRequestHandler {
 public:
  using ResponseCallback = base::Callback<void(const base::DictionaryValue&)>;

  DumbRequestHandler();
  ~DumbRequestHandler();

  bool IsRequestIdentifierValid(uint64_t request_identifier);

  int OnBeforeURLRequest(std::shared_ptr<dumb::DumbRequestInfo> ctx,
                         net::CompletionOnceCallback callback,
                         GURL* new_url);

  int OnBeforeStartTransaction(std::shared_ptr<dumb::DumbRequestInfo> ctx,
                               net::CompletionOnceCallback callback,
                               net::HttpRequestHeaders* headers);
  int OnHeadersReceived(
      std::shared_ptr<dumb::DumbRequestInfo> ctx,
      net::CompletionOnceCallback callback,
      const net::HttpResponseHeaders* original_response_headers,
      scoped_refptr<net::HttpResponseHeaders>* override_response_headers,
      GURL* allowed_unsafe_redirect_url);

  void OnURLRequestDestroyed(std::shared_ptr<dumb::DumbRequestInfo> ctx);
  void RunCallbackForRequestIdentifier(uint64_t request_identifier, int rv);

 private:
  void SetupCallbacks();
  void InitPrefChangeRegistrar();
  void OnReferralHeadersChanged();
  void OnPreferenceChanged(const std::string& pref_name);
  void UpdateAdBlockFromPref(const std::string& pref_name);

  void RunNextCallback(std::shared_ptr<dumb::DumbRequestInfo> ctx);

  std::vector<dumb::OnBeforeURLRequestCallback> before_url_request_callbacks_;
  std::vector<dumb::OnBeforeStartTransactionCallback>
      before_start_transaction_callbacks_;
  std::vector<dumb::OnHeadersReceivedCallback> headers_received_callbacks_;

  // TODO(iefremov): actually, we don't have to keep the list here, since
  // it is global for the whole browser and could live a singletonce in the
  // rewards service. Eliminating this will also help to avoid using
  // PrefChangeRegistrar and corresponding |base::Unretained| usages, that are
  // illegal.
  std::unique_ptr<base::ListValue> referral_headers_list_;
  std::map<uint64_t, net::CompletionOnceCallback> callbacks_;
  std::unique_ptr<PrefChangeRegistrar, content::BrowserThread::DeleteOnUIThread>
      pref_change_registrar_;

  base::WeakPtrFactory<DumbRequestHandler> weak_factory_{this};
  DISALLOW_COPY_AND_ASSIGN(DumbRequestHandler);
};

#endif  // DUMB_NET_DUMB_REQUEST_HANDLER_H_
