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

#include "dumb/components/privacy_guard/dumb_privacy_guard_service.h"

#include "base/no_destructor.h"
#include "components/prefs/pref_change_registrar.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "dumb/components/privacy_guard/pref_names.h"

DumbPrivacyGuardService::DumbPrivacyGuardService(PrefService* prefs)
    : prefs_(prefs) {
  pref_change_registrar_.reset(new PrefChangeRegistrar());
  pref_change_registrar_->Init(prefs);
  // TODO: add pref change registrar callbacks.
}

DumbPrivacyGuardService::~DumbPrivacyGuardService() = default;

// static
void DumbPrivacyGuardService::RegisterProfilePrefs(
    PrefRegistrySimple *registry) {
  registry->RegisterBooleanPref(prefs::kURLPurifyEnabled, true);
}

void DumbPrivacyGuardService::OnPrefsChanged(const std::string& pref_name) {

}
