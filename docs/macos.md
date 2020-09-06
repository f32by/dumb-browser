# Building Dumb Browser for macOS

## Install Xcode 11+

Install from AppStore or download from [developer.apple.com/download/more](https://developer.apple.com/download/more/)

**DO NOT DOWNLOAD FROM ANY UNOFFICICAL SOURCES**

## Install Python 3.5+

[homebrew](https://brew.sh) is recommended.

```shell
brew install python
```

Alternatively, you can download a pkg installer from [Python official website](https://www.python.org/downloads/mac-osx/).

## Useful notes
1. [Chromium's official build instruction](https://chromium.googlesource.com/chromium/src/+/master/docs/mac_build_instructions.md)
2. *09/06/2020* If you build Dumb Browser on macOS 11 Beta, you have to create your own python virtualenv since Chromium's vpython is broken on macOS 11. Install `psutil` and `httplib2` in your virtualenv and make sure activate virtualenv before building.
