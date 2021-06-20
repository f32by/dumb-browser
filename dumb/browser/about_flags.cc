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

#include "dumb/browser/about_flags.h"

#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "components/flags_ui/feature_entry_macros.h"
#include "dumb/browser/flag_descriptions.h"
#include "dumb/browser/ui/ui_features.h"
#include "dumb/common/dumb_switches.h"
#include "dumb/components/dumb/switches.h"

using flags_ui::FeatureEntry;
using flags_ui::kDeprecated;
using flags_ui::kOsAndroid;
using flags_ui::kOsCrOS;
using flags_ui::kOsCrOSOwnerOnly;
using flags_ui::kOsLinux;
using flags_ui::kOsMac;
using flags_ui::kOsWin;

namespace {

const unsigned kOsAll = kOsMac | kOsWin | kOsLinux | kOsCrOS | kOsAndroid;
const unsigned kOsDesktop = kOsMac | kOsWin | kOsLinux | kOsCrOS;

// The "Enabled" state for this feature is "0" and representing setting A.
const FeatureEntry::FeatureParam kTabHoverCardsSettingB[] = {
    {features::kTabHoverCardsFeatureParameterName, "1"}};
const FeatureEntry::FeatureParam kTabHoverCardsSettingC[] = {
    {features::kTabHoverCardsFeatureParameterName, "2"}};

const FeatureEntry::FeatureVariation kTabHoverCardsFeatureVariations[] = {
    {"B", kTabHoverCardsSettingB, base::size(kTabHoverCardsSettingB), nullptr},
    {"C", kTabHoverCardsSettingC, base::size(kTabHoverCardsSettingC), nullptr}};

}

namespace about_flags {

const FeatureEntry kDumbFeatureEntries[] = {
  // UI flags.
  {"tab-hover-cards", flag_descriptions::kTabHoverCardsName,
     flag_descriptions::kTabHoverCardsDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kTabHoverCards,
                                    kTabHoverCardsFeatureVariations,
                                    "TabHoverCards")},
  {flag_descriptions::kExtensionsToolbarMenuId,
   flag_descriptions::kExtensionsToolbarMenuName,
   flag_descriptions::kExtensionsToolbarMenuDescription, kOsDesktop,
   FEATURE_VALUE_TYPE(features::kExtensionsToolbarMenu)},
  {flag_descriptions::kEnableTabSearchFlagId,
   flag_descriptions::kEnableTabSearchName,
   flag_descriptions::kEnableTabSearchDescription, kOsDesktop,
   FEATURE_VALUE_TYPE(features::kTabSearch)},
  {flag_descriptions::kTabAudioMutingFlagId,
   flag_descriptions::kTabAudioMutingName,
   flag_descriptions::kTabAudioMutingDescription, kOsDesktop,
   SINGLE_VALUE_TYPE(switches::kTabAudioMuting)},
  // Privacy flags.
  {flag_descriptions::kFingerprintingClientRectsNoiseId,
   flag_descriptions::kFingerprintingClientRectsNoiseName,
   flag_descriptions::kFingerprintingClientRectsNoiseDescription, kOsAll,
   SINGLE_VALUE_TYPE(switches::kFingerprintingClientRectsNoise)},
  {flag_descriptions::kFingerprintingCanvasMeasureTextNoiseId,
   flag_descriptions::kFingerprintingCanvasMeasureTextNoiseName,
   flag_descriptions::kFingerprintingCanvasMeasureTextNoiseDescription, kOsAll,
   SINGLE_VALUE_TYPE(switches::kFingerprintingCanvasMeasureTextNoise)},
  {flag_descriptions::kFingerprintingCanvasImageDataNoiseId,
   flag_descriptions::kFingerprintingCanvasImageDataNoiseName,
   flag_descriptions::kFingerprintingCanvasImageDataNoiseDescription, kOsAll,
   SINGLE_VALUE_TYPE(switches::kFingerprintingCanvasImageDataNoise)},
};

const base::span<const flags_ui::FeatureEntry> GetDumbFeatureEntries() {
  return kDumbFeatureEntries;
}

}
