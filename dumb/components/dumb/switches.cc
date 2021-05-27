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

#include "dumb/components/dumb/switches.h"

namespace switches {

// Enable fingerprinting deception for getClientRects and getBoundingClientRect.
const char kFingerprintingClientRectsNoise[] =
    "fingerprinting-client-rects-noise";

// Enable fingerprinting deception for measureText
const char kFingerprintingCanvasMeasureTextNoise[] =
    "fingerprinting-canvas-measuretext-noise";

// Enable fingerprinting deception for Canvas image data
const char kFingerprintingCanvasImageDataNoise[] =
    "fingerprinting-canvas-image-data-noise";

}
