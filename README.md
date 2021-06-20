# Dumb Browser
A custom Chromium build inspired by ungoogled-chromium and Brave.

Current version: `91.0.4472.114`.

# Features

Following components are removed/disabled:
- [Safe Browsing](https://safebrowsing.google.com)
- Histogram logging (chrome://histograms)
- WebRTC Log Uploader
- Network Time Tracker
- [RLZ](https://blog.chromium.org/2010/06/in-open-for-rlz.html)
- Chrome Enterprise Reporter
- (macOS only) Download Quarantine
- (Windows only) Chrome Cleaner
- Field trails fetching
- Promotions in New Tab Page
- [HTML \<ping\> attribute](https://www.w3schools.com/tags/att_a_ping.asp)
- Chromium Updater
- Subresource filter (used by Chrome's integrated ad-block feature.)
- [UKM(Url-Keyed Metrics API)](https://chromium.googlesource.com/chromium/src/+/master/services/metrics/ukm_api.md) ( chrome://ukm )
- [FLoC](https://www.chromium.org/Home/chromium-privacy/privacy-sandbox/floc)
- [HaTS](https://chromium.googlesource.com/chromium/src/+/master/chrome/browser/ui/hats/)
- CSP report
- Crash reports uploader

Other changes:
- [Privacy] Automatically remove track query parameters from URL (*referrer, utm_\*, spm, etc.)
- [Privacy] Canvas fingerprinting deception (from ungoogled-chromium)
- [UI] Audio mute button on tab
- [UI] Force popups to be opened in new tabs
- [UI] Darker web control elements (Edge style)
- [UI] No need to type `thisisunsafe` to continue when browsing some websites with certificate issues. You can just use the old-style `continue` button.
- [UI] Disable `Tab Search button` in chrome://flags
- [History] Unlimited browsing history
- [Misc] Automatically remove download item after completion.

# Installation

Tested on following OS:

1. macOS Big Sur 11.4 (20F71) x86_64
2. Ubuntu 20.04.2 LTS x86_64
3. Windows 10 20H2 (19042.985) x86_64

*Other OS/architecture has not been tested yet and build may fail.*

### Prerequisites

Windows: [Building Dumb Browser for Windows](docs/windows.md)

macOS: [Building Dumb Browser for macOS](docs/macos.md)

Linux: No build script available. You can follow [official build instructions](https://chromium.googlesource.com/chromium/src/+/master/docs/linux/build_instructions.md) until the `Run the hooks` section
and do following things to make Dumb Browser compile:
```shell
# fetch Chromium sources
./scripts/sync.py
# After code sync, follow Chromium's build instructions until the `Run the hooks` section
# apply Dumb Browser's patches
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
