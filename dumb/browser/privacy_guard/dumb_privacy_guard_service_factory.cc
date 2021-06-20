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

#include "dumb/browser/privacy_guard/dumb_privacy_guard_service_factory.h"

#include "base/no_destructor.h"
#include "chrome/browser/profiles/incognito_helpers.h"
#include "chrome/browser/profiles/profile.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "dumb/components/privacy_guard/browser/dumb_privacy_guard_service.h"

// static
DumbPrivacyGuardService* DumbPrivacyGuardServiceFactory::GetForBrowserContext(
    content::BrowserContext* context) {
  return static_cast<DumbPrivacyGuardService*>(
      GetInstance()->GetServiceForBrowserContext(context,
                                                 /*create=*/true));
}

// static
DumbPrivacyGuardServiceFactory* DumbPrivacyGuardServiceFactory::GetInstance() {
  static base::NoDestructor<DumbPrivacyGuardServiceFactory> instance;
  return instance.get();
}

DumbPrivacyGuardServiceFactory::DumbPrivacyGuardServiceFactory()
    : BrowserContextKeyedServiceFactory(
          "DumbPrivacyGuardService",
          BrowserContextDependencyManager::GetInstance()) {}

DumbPrivacyGuardServiceFactory::~DumbPrivacyGuardServiceFactory() = default;

KeyedService* DumbPrivacyGuardServiceFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) const {
  return new DumbPrivacyGuardService(
      Profile::FromBrowserContext(context)->GetPrefs());
}

content::BrowserContext*
DumbPrivacyGuardServiceFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return chrome::GetBrowserContextRedirectedInIncognito(context);
}

bool
DumbPrivacyGuardServiceFactory::ServiceIsCreatedWithBrowserContext() const {
  return true;
}
