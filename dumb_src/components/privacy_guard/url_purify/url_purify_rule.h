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

#ifndef DUMB_COMPONENTS_PRIVACY_GUARD_URL_PURIFY_URL_PURIFY_RULE_H_
#define DUMB_COMPONENTS_PRIVACY_GUARD_URL_PURIFY_URL_PURIFY_RULE_H_

#include <string>
#include <vector>

#include "base/macros.h"
#include "base/optional.h"

struct URLPurifyRule {
  URLPurifyRule(std::string name,
      std::string url_pattern,
      std::vector<std::string> query_patterns,
      base::Optional<std::vector<std::string>> url_exceptions);

  URLPurifyRule(std::string name,
      std::string url_pattern,
      std::vector<std::string> query_patterns,
      std::vector<std::string> url_exceptions);

  URLPurifyRule(const URLPurifyRule&);
  URLPurifyRule(URLPurifyRule&&);

  ~URLPurifyRule();

  std::string name;
  std::string url_pattern;
  std::vector<std::string> query_patterns;
  base::Optional<std::vector<std::string>> url_exceptions;
};

#endif // DUMB_COMPONENTS_PRIVACY_GUARD_URL_PURIFY_URL_PURIFY_RULE_H_
