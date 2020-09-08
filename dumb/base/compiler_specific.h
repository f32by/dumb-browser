/* Copyright (c) 2020 The Dumb Browser Authors. All rights reserved.
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

#ifndef DUMB_BASE_COMPILER_SPECIFIC_H_
#define DUMB_BASE_COMPILER_SPECIFIC_H_

// Annotate a typedef or function indicating it's ok if it's not used.
// Use like:
//   typedef Foo Bar DUMB_ALLOW_UNUSED_TYPE;
#if defined(COMPILER_GCC) || defined(__clang__)
#define DUMB_ALLOW_UNUSED_TYPE __attribute__((unused))
#else
#define DUMB_ALLOW_UNUSED_TYPE
#endif

// Suppress variadic unused variables.
template <typename ...Args>
void DUMB_UNUSED(DUMB_ALLOW_UNUSED_TYPE Args&& ...args) {
  static_cast<void>(sizeof...(args));
}

#endif // DUMB_BASE_COMPILER_SPECIFIC_H_
