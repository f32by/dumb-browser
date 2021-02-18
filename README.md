# Dumb Browser
A custom Chromium build inspired by ungoogled-chromium and Brave.

Current version: `88.0.4324.182`.

**Some patches are experimental and build may fail.**

# Features

> All patches are in `patches` folder.

Following components are removed/disabled:
1. [Safe Browsing](https://safebrowsing.google.com)
2. Histogram logging ( chrome://histograms )
3. Crash Reporter & Crashpad
4. WebRTC Log Uploader
5. Network Time Tracker
6. [RLZ](https://blog.chromium.org/2010/06/in-open-for-rlz.html)
7. [Rappor](https://github.com/google/rappor)
8. (macOS only) Download Quarantine
10. (Windows only) Chrome Cleaner
11. Field trails fetching
12. Promotions in New Tab Page
13. [HTML \<ping\> attribute](https://www.w3schools.com/tags/att_a_ping.asp)
14. Chromium Updater
15. Subresource filter (which is used by Chrome's integrated AdBlocking feature.)
16. [UKM(Url-Keyed Metrics API)](https://chromium.googlesource.com/chromium/src/+/master/services/metrics/ukm_api.md) ( chrome://ukm )

Other changes:
1. [UI] Restore mute button on tab
2. [Privacy] Automatically remove track query parameters from URL (*referrer, utm_\*, spm, and more*)
3. [History] Unlimited browsing history.

# Installation

## 1. [Recommended] Build from Sources

It's impossible to build Dumb Browser via GitHub Actions due to GitHub free account's resource limitations.
Please build on your own or wait for official binaries.

Tested on following OS:
1. macOS 11.2.1
2. Windows 10 1909 18363.1082

### Prerequisites

Android: Currently not supported.

Windows: [Building Dumb Browser for Windows](docs/windows.md)

macOS: [Building Dumb Browser for macOS](docs/macos.md)

### Build Instruction

```shell
git clone https://github.com/bytanuky/dumb-browser
cd dumb-browser
# install dependencies
./scripts/prepare.py
# fetch chromium src and apply dumb browser's patches
# depend on your network connection, this may take 30-60 minutes.
./scripts/sync.py
# get a release build
# you may also use `./build/build.py debug` to get a debug build
# show available options: ./build/build.py -h
./scripts/build.py release
# wait patiently :)
```

Binaries will be found in `src/out/{Release|Debug} .`

## 2. Official Binaries
*Coming soon*

> Note: Provided binaries do not contain any Google API keys thus you can't use Chrome Sync by default.
> To solve this, follow [Chromium's Official How-Tos](https://www.chromium.org/developers/how-tos/api-keys)
> and add your own API keys to environment variables.

# Known Issues

1. [1Password extension (desktop app required)](https://chrome.google.com/webstore/detail/1password-extension-deskt/aomjjhallfgjeglblehebfpbcfeobpgk) is unavailable since it forces an allowlist-based browser signature check. Instead you can use `1Password X` which requires a valid 1Password subscription.

# Credits

[ungoogled-chromium](https://github.com/Eloston/ungoogled-chromium)

[Brave](https://github.com/brave/brave-browser)

[CleanURLs](https://github.com/GlowPuff/CleanURLs)
