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

#include "dumb/components/privacy_guard/core/url_purify_rule_parser.h"

#include "base/json/json_reader.h"

URLPurifyRuleSet::URLPurifyRuleSet(
    URLPurifyRule global_rule,
    base::Optional<std::vector<URLPurifyRule>> per_site_rules)
    : global_rule(global_rule),
      per_site_rules(per_site_rules) {}

URLPurifyRuleSet::~URLPurifyRuleSet() = default;

URLPurifyRuleSet::URLPurifyRuleSet(const URLPurifyRuleSet&) = default;

base::Optional<URLPurifyRuleSet> ParseRules(std::string rule_string) {

  return base::nullopt;
}
