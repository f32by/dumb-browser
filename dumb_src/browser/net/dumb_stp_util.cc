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

#include "dumb/browser/net/dumb_stp_util.h"

#include "base/no_destructor.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"

namespace dumb {

base::flat_set<base::StringPiece>* TrackableSecurityHeaders() {
  static base::NoDestructor<base::flat_set<base::StringPiece>>
      kTrackableSecurityHeaders(base::flat_set<base::StringPiece>{
          "Strict-Transport-Security", "Expect-CT", "Public-Key-Pins",
          "Public-Key-Pins-Report-Only"});
  return kTrackableSecurityHeaders.get();
}

void RemoveTrackableSecurityHeadersForThirdParty(
    const GURL& request_url, const url::Origin& top_frame_origin,
    const net::HttpResponseHeaders* original_response_headers,
    scoped_refptr<net::HttpResponseHeaders>* override_response_headers) {
  if (!original_response_headers && !override_response_headers->get()) {
    return;
  }

  if (net::registry_controlled_domains::SameDomainOrHost(
          request_url, top_frame_origin,
          net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES)) {
    return;
  }

  if (!override_response_headers->get()) {
    *override_response_headers =
        new net::HttpResponseHeaders(original_response_headers->raw_headers());
  }
  for (auto header : *TrackableSecurityHeaders()) {
    (*override_response_headers)->RemoveHeader(header.as_string());
  }
}

}  // namespace dumb
