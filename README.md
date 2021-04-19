# Dumb Browser
A custom Chromium build inspired by ungoogled-chromium and Brave.

Current version: `90.0.4430.72`.

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
2. [Privacy] Automatically remove track query parameters from URL (*referrer, utm_\*, spm, and more*)
3. [History] Unlimited browsing history.
4. [UI] Force popups to be opened in new tabs.

# Installation

Tested on macOS 11.3 Beta 8 (20E5231a). Windows is not tested and build may fail.

### Prerequisites

Windows: [Building Dumb Browser for Windows](docs/windows.md)

macOS: [Building Dumb Browser for macOS](docs/macos.md)

### Build Instruction

```shell
git clone https://github.com/f32/dumb-browser
cd dumb-browser
# install dependencies
./scripts/prepare.py
# fetch chromium src and apply dumb browser's patches
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

https://www.chromium.org/developers/how-tos/api-keys
