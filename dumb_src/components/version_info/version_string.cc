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

#include "dumb/components/version_info/version_string.h"

#include "components/strings/grit/components_strings.h"
#include "components/version_info/version_info.h"
#include "ui/base/l10n/l10n_util.h"

namespace version_info {

std::string GetDumbVersionString() {
  std::string version = l10n_util::GetStringUTF8(IDS_DUMB_VERSION);
  version += " (Chromium ";
  version += version_info::GetVersionNumber();
  version += ")";

  return version;
}

} // namespace version_info
