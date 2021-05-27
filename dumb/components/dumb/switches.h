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

#ifndef DUMB_COMPONENTS_DUMB_SWITCHES_H_
#define DUMB_COMPONENTS_DUMB_SWITCHES_H_

#include "base/component_export.h"

namespace switches {

// From ungoogle-chromium

COMPONENT_EXPORT(UNGOOGLED) extern const char
    kFingerprintingClientRectsNoise[];
COMPONENT_EXPORT(UNGOOGLED) extern const char
    kFingerprintingCanvasMeasureTextNoise[];
COMPONENT_EXPORT(UNGOOGLED) extern const char
    kFingerprintingCanvasImageDataNoise[];

}

#endif // DUMB_COMPONENTS_DUMB_SWITCHES_H_
