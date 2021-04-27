# Dumb Browser
A custom Chromium build inspired by ungoogled-chromium and Brave.

Current version: `90.0.4430.93`.

# Features

Following components are removed/disabled:
1. [Safe Browsing](https://safebrowsing.google.com)
2. Histogram logging (chrome://histograms)
3. Crash Reporter & Crashpad
4. WebRTC Log Uploader
5. Network Time Tracker
6. [RLZ](https://blog.chromium.org/2010/06/in-open-for-rlz.html)
7. Chrome Enterprise
8. (macOS only) Download Quarantine
10. (Windows only) Chrome Cleaner
11. Field trails fetching
12. Promotions in New Tab Page
13. [HTML \<ping\> attribute](https://www.w3schools.com/tags/att_a_ping.asp)
14. Chromium Updater
15. Subresource filter (used by Chrome's integrated ad-block feature.)
16. [UKM(Url-Keyed Metrics API)](https://chromium.googlesource.com/chromium/src/+/master/services/metrics/ukm_api.md) ( chrome://ukm )
17. [FLoC](https://www.chromium.org/Home/chromium-privacy/privacy-sandbox/floc)
19. [HaTS](https://chromium.googlesource.com/chromium/src/+/master/chrome/browser/ui/hats/)
20. CSP report

Other changes:
1. [UI] Audio mute button on tab
2. [Privacy] Automatically remove track query parameters from URL (*referrer, utm_\*, spm, etc.)
3. [History] Unlimited browsing history.
4. [UI] Force popups to be opened in new tabs.

# Installation

Tested on following OS:

1. macOS Big Sur 11.3 RC (20E232) x86_64
2. macOS Big Sur 11.4 Beta (20F5046g) x86_64
2. Ubuntu 20.04.2 LTS x86_64

*Other OS/architecture is tested and build may fail.*

### Prerequisites

Windows: [Building Dumb Browser for Windows](docs/windows.md)

macOS: [Building Dumb Browser for macOS](docs/macos.md)

Linux: No build script available. You can follow [official build instructions](https://chromium.googlesource.com/chromium/src/+/master/docs/linux/build_instructions.md)
and apply patches before calling `autoninja`:

```shell
# fetch Chromium sources
./scripts/sync.py
# install dependencies
cd src
./build/install-build-deps.sh
# apply Dumb Browser's patches
cd ..
./scripts/apply_patches.py
# build
./scripts/build.py release
```

### Build Instruction

```shell
git clone https://github.com/f32by/dumb-browser
cd dumb-browser
# install dependencies
./scripts/prepare.py
# fetch Chromium sources and apply Dumb Browser sources
# depend on your network connection, this may take 30-120 minutes.
./scripts/sync.py
# get a release build
# you may also use `./build/build.py debug` to get a debug build
# show available options: ./build/build.py -h
./scripts/build.py release
# wait patiently :)
```

Final binaries will be found in `src/out/{Release|Debug} .`

# Credits

[ungoogled-chromium](https://github.com/Eloston/ungoogled-chromium)

[Brave](https://github.com/brave/brave-browser)

[ClearURLs](https://github.com/ClearURLs/Addon)

# Use Google Services

[Offical API Key Document](https://www.chromium.org/developers/how-tos/api-keys)

Note: Chrome Sync in Chromium has been
limited by Google(see [this](https://blog.chromium.org/2021/01/limiting-private-api-availability-in.html)). You can still apply for the [Chrome Sync API](https://console.cloud.google.com/apis/library/chromesync.googleapis.com) but without the private *Firebase Per Topic
Messaging API*,  any changes synced from other clients will not be received on the
computer you're currently using, unless you force an update manually in
[chrome://sync-internals/](chrome://sync-internals/).
