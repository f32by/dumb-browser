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

#ifndef DUMB_BROWSER_DUMB_BROWSER_MAIN_EXTRA_PARTS_H_
#define DUMB_BROWSER_DUMB_BROWSER_MAIN_EXTRA_PARTS_H_

#include "base/macros.h"
#include "chrome/browser/chrome_browser_main_extra_parts.h"

class ChromeBrowserMainParts;

namespace dumb {
void AddProfilesExtraParts(ChromeBrowserMainParts* main_parts);
}

class DumbBrowserMainExtraPartsProfiles: public ChromeBrowserMainExtraParts {
public:
  DumbBrowserMainExtraPartsProfiles();
  ~DumbBrowserMainExtraPartsProfiles() override;

  // Overridden from ChromeBrowserMainExtraParts:
  void PreProfileInit() override;

  DISALLOW_COPY_AND_ASSIGN(DumbBrowserMainExtraPartsProfiles);
};

#endif // DUMB_BROWSER_DUMB_BROWSER_MAIN_EXTRA_PARTS_H_
