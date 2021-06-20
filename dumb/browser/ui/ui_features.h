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

#include "base/feature_list.h"
#include "base/metrics/field_trial_params.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "chrome/common/buildflags.h"
#include "extensions/buildflags/buildflags.h"

#ifndef DUMB_BROWSER_BROWSER_UI_UI_FEATURES_H_
#define DUMB_BROWSER_BROWSER_UI_UI_FEATURES_H_

namespace features {

extern const base::Feature kTabHoverCards;
extern const char kTabHoverCardsFeatureParameterName[];

extern const base::Feature kExtensionsToolbarMenu;

extern const base::Feature kTabSearch;

extern const base::Feature kTabAudioMuting;

}

#endif // DUMB_BROWSER_BROWSER_UI_UI_FEATURES_H_
