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

#ifndef DUMB_BROWSER_BROWSER_ABOUT_FLAGS_H_
#define DUMB_BROWSER_BROWSER_ABOUT_FLAGS_H_

#include <stddef.h>
#include <stdint.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "components/flags_ui/feature_entry.h"
#include "components/flags_ui/flags_state.h"

namespace about_flags {

const base::span<const flags_ui::FeatureEntry> GetDumbFeatureEntries();

}

#endif // DUMB_BROWSER_BROWSER_ABOUT_FLAGS_H_
