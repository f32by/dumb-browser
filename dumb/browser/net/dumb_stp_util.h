/* Copyright 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DUMB_NET_DUMB_STP_UTIL_H_
#define DUMB_NET_DUMB_STP_UTIL_H_

#include "base/containers/flat_set.h"
#include "base/strings/string_piece.h"
#include "net/http/http_response_headers.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace dumb {

base::flat_set<base::StringPiece>* TrackableSecurityHeaders();

void RemoveTrackableSecurityHeadersForThirdParty(
    const GURL& request_url, const url::Origin& top_frame_origin,
    const net::HttpResponseHeaders* original_response_headers,
    scoped_refptr<net::HttpResponseHeaders>* override_response_headers);

}  // namespace dumb

#endif  // DUMB_NET_DUMB_STP_UTIL_H_
