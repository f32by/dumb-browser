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

#ifndef DUMB_BRWOSER_PRIVACY_GUARD_BROWSER_DUMB_PRIVACY_GUARD_SERVICE_FACTORY_H_
#define DUMB_BRWOSER_PRIVACY_GUARD_BROWSER_DUMB_PRIVACY_GUARD_SERVICE_FACTORY_H_

#include "base/no_destructor.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"

class DumbPrivacyGuardService;

class DumbPrivacyGuardServiceFactory
    : public BrowserContextKeyedServiceFactory {

public:
  static DumbPrivacyGuardService* GetForBrowserContext(
      content::BrowserContext* context);

  static DumbPrivacyGuardServiceFactory* GetInstance();

 private:
  friend class base::NoDestructor<DumbPrivacyGuardServiceFactory>;

  DumbPrivacyGuardServiceFactory();
  ~DumbPrivacyGuardServiceFactory() override;

  // BrowserContextKeyedServiceFactory:
  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* profile) const override;

  // We use the same service in both normal and incognito modes.
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;

  bool ServiceIsCreatedWithBrowserContext() const override;

  DISALLOW_COPY_AND_ASSIGN(DumbPrivacyGuardServiceFactory);
};

#endif // DUMB_BRWOSER_PRIVACY_GUARD_BROWSER_DUMB_PRIVACY_GUARD_SERVICE_FACTORY_H_
