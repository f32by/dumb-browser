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

#ifndef DUMB_COMPONENTS_PRIVACY_PRIVACY_GUARD_BROWSER_DUMB_PRIVACY_GUARD_SERVICE_H_
#define DUMB_COMPONENTS_PRIVACY_PRIVACY_GUARD_BROWSER_DUMB_PRIVACY_GUARD_SERVICE_H_

#include "components/keyed_service/core/keyed_service.h"
#include "content/public/browser/browser_thread.h"
#include "dumb/base/compiler_specific.h"

class PrefChangeRegistrar;
class PrefRegistrySimple;
class PrefService;

class DumbPrivacyGuardService : public KeyedService {
public:
  explicit DumbPrivacyGuardService(PrefService* prefs);
  ~DumbPrivacyGuardService() override;

  // static
  static void RegisterProfilePrefs(PrefRegistrySimple* registry);

private:
  void OnPrefsChanged(const std::string& pref_name);

  DUMB_ALLOW_UNUSED PrefService* prefs_;
  std::unique_ptr<PrefChangeRegistrar, content::BrowserThread::DeleteOnUIThread>
      pref_change_registrar_;
};

#endif // DUMB_COMPONENTS_PRIVACY_PRIVACY_GUARD_BROWSER_DUMB_PRIVACY_GUARD_SERVICE_H_
