/* Copyright (c) 2021 The Dumb Browser Authors. All rights reserved.
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

#ifndef DUMB_COMPONENTS_PRIVACY_GUARD_URL_PURIFY_URL_PURIFY_DEFAULT_RULES_H_
#define DUMB_COMPONENTS_PRIVACY_GUARD_URL_PURIFY_URL_PURIFY_DEFAULT_RULES_H_

#include "dumb/components/privacy_guard/url_purify/url_purify_rule.h"

const URLPurifyRule& GetDefaultGlobalRules();

const std::vector<URLPurifyRule>& GetDefaultPerSiteRules();

#endif  // DUMB_COMPONENTS_PRIVACY_GUARD_URL_PURIFY_URL_PURIFY_DEFAULT_RULES_H_
