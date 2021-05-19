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

#include "dumb/components/privacy_guard/url_purify/url_purify_rule.h"

URLPurifyRule::URLPurifyRule(std::string name,
    std::string url_pattern,
    std::vector<std::string> query_patterns,
    std::vector<std::string> url_exceptions)
    : name(std::move(name)),
      url_pattern(std::move(url_pattern)),
      query_patterns(std::move(query_patterns)),
      url_exceptions(std::move(url_exceptions)) {}

URLPurifyRule::URLPurifyRule(std::string name,
    std::string url_pattern,
    std::vector<std::string> query_patterns,
    base::Optional<std::vector<std::string>> url_exceptions)
    : name(std::move(name)),
      url_pattern(std::move(url_pattern)),
      query_patterns(std::move(query_patterns)),
      url_exceptions(std::move(url_exceptions)) {}

URLPurifyRule::URLPurifyRule(const URLPurifyRule&) = default;

URLPurifyRule::URLPurifyRule(URLPurifyRule&&) = default;

URLPurifyRule::~URLPurifyRule() = default;
