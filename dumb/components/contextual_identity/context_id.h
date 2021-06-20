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

#ifndef DUMB_BROWSER_COMPONENTS_CONTEXTUAL_IDENTITY_CONTEXT_ID_H_
#define DUMB_BROWSER_COMPONENTS_CONTEXTUAL_IDENTITY_CONTEXT_ID_H_

#include <cstdint>
#include <string>

#include "build/build_config.h"

namespace contextual_identity {

using ContextId = std::string;

extern const char kDefaultContextId[];

} // namespace contextual_identity

#endif // DUMB_BROWSER_COMPONENTS_CONTEXTUAL_IDENTITY_CONTEXT_ID_H_
