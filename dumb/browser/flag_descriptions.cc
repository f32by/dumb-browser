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

#include "dumb/browser/flag_descriptions.h"

namespace flag_descriptions {

const char kTabHoverCardsId[] = "tab-hover-cards";
const char kTabHoverCardsName[] = "Tab Hover Cards";
const char kTabHoverCardsDescription[] =
    "Enables a popup containing tab information to be visible when hovering "
    "over a tab. This will replace tooltips for tabs.";

const char kExtensionsToolbarMenuId[] = "extensions-toolbar-menu";
const char kExtensionsToolbarMenuName[] = "Extensions Toolbar Menu";
const char kExtensionsToolbarMenuDescription[] =
    "Enable a separate toolbar button and menu for extensions";

const char kEnableTabSearchFlagId[] = "enable-tab-search";
const char kEnableTabSearchName[] = "Enable Tab Search";
const char kEnableTabSearchDescription[] =
    "Enable the Tab Search feature in Top Chrome UI, which will show a popup "
    "bubble that allows users to search over their currently open tabs.";

const char kTabAudioMutingFlagId[] = "enable-tab-audio-muting";
const char kTabAudioMutingName[] = "Tab audio muting UI control";
const char kTabAudioMutingDescription[] =
    "When enabled, the audio indicators in the tab strip double as tab audio "
    "mute controls. This also adds commands in the tab context menu for "
    "quickly muting multiple selected tabs.";

const char kFingerprintingClientRectsNoiseId[] =
    "fingerprinting-client-rects-noise";
const char kFingerprintingClientRectsNoiseName[] =
    "Enable get*ClientRects() fingerprinting deception.";
const char kFingerprintingClientRectsNoiseDescription[] =
    "Scale the output values of Range::getClientRects() and"
    "Element::getBoundingClientRect() with a randomly selected factor"
    "in the range -0.0003% to 0.0003%, which are recomputed on every document"
    "initialization. ungoogled-chromium flag, Bromite feature.";

const char kFingerprintingCanvasMeasureTextNoiseId[] =
    "fingerprinting-canvas-measuretext-noise";
const char kFingerprintingCanvasMeasureTextNoiseName[] =
    "Enable Canvas::measureText() fingerprinting deception.";
const char kFingerprintingCanvasMeasureTextNoiseDescription[] =
    "Scale the output values of Canvas::measureText() with a randomly selected"
    "factor in the range -0.0003% to 0.0003%, which are recomputed on every"
    "document initialization. ungoogled-chromium flag, Bromite feature.";

const char kFingerprintingCanvasImageDataNoiseId[] =
    "fingerprinting-canvas-image-data-noise";
const char kFingerprintingCanvasImageDataNoiseName[] =
    "Enable Canvas image data fingerprint deception.";
const char kFingerprintingCanvasImageDataNoiseDescription[] =
    "Slightly modifies at most 10 pixels in Canvas image data extracted"
    "via JS APIs. ungoogled-chromium flag, Bromite feature.";

}
