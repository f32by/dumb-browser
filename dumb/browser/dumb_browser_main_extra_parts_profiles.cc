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

#include "dumb/browser/dumb_browser_main_extra_parts_profiles.h"

#include "chrome/browser/chrome_browser_main.h"
#include "dumb/browser/browser_context_keyed_service_factories.h"

namespace dumb {

void AddProfilesExtraParts(ChromeBrowserMainParts* main_parts) {
  main_parts->AddParts(std::make_unique<DumbBrowserMainExtraPartsProfiles>());
}

}

DumbBrowserMainExtraPartsProfiles::DumbBrowserMainExtraPartsProfiles()
    = default;

DumbBrowserMainExtraPartsProfiles::~DumbBrowserMainExtraPartsProfiles()
    = default;

void DumbBrowserMainExtraPartsProfiles::PreProfileInit() {
  dumb::EnsureBrowserContextKeyedServiceFactoriesBuilt();
}
