# Building Dumb Browser for Windows

## Install Visual Studio 2019

Community Edition should work if its license is appropriate for you. You must install the “Desktop development with C++” component and the “MFC/ATL support” sub-components. This can be done from the command line by passing these arguments to the VS installer:

```shell
$ PATH_TO_INSTALLER.EXE ^
--add Microsoft.VisualStudio.Workload.NativeDesktop ^
--add Microsoft.VisualStudio.Component.VC.ATLMFC ^
--includeRecommended
```

https://visualstudio.microsoft.com

## Install Windows 10 SDK 10.0.19041+

https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/

## Install Python 3.5+

Download from https://www.python.org/downloads/windows/

It's highly recommended to apply PATH_MAX fix when installing Python. If you forgot to do so, double click `util/remove_max_path_restriction.reg` and click `yes` to apply.

## Useful notes
1. [Chromium's official build instruction](https://chromium.googlesource.com/chromium/src/+/master/docs/windows_build_instructions.md)