#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2020 ByTanuky
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import os
import urllib.request as request
import winreg
import zipfile

from constants import DEPOT_TOOLS_DIR, PROJECT_DIR
from utils import run_command

VS_REGISTRY_KEY = r"SOFTWARE\Microsoft\VisualStudio\14.0"
DEPOT_TOOLS_BUNDLE_URL = 'https://storage.googleapis.com/chrome-infra/depot_tools.zip'


def __show_progress(blocknum, bs, size):
    print('Downloading {}/{} bytes'.format(blocknum * bs, size))


def check_vs_version(required_version=2019):
    print('Checking Visual Studio version...')

    try:
        winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, VS_REGISTRY_KEY, 0, winreg.KEY_READ)
    except Exception:
        print('Visual Studio 2019 not found.')
        return False

    print('Visual Studio 2019 is installed.')
    return True


def check_depot_tools():
    print('Checking depot_tools. Path: ', DEPOT_TOOLS_DIR)
    if not os.path.exists(DEPOT_TOOLS_DIR):
        print('Downloading depot_tools...')
        download_path = os.path.join(PROJECT_DIR, '.download')
        if not os.path.exists(download_path):
            os.mkdir(download_path)

        download_zip_filename = os.path.join(download_path, 'depot_tools.zip')

        request.urlretrieve(DEPOT_TOOLS_BUNDLE_URL, filename=download_zip_filename, reporthook=__show_progress)
        print('Installing depot_tools...')
        # extract zip
        with zipfile.ZipFile(download_zip_filename, 'r') as zip_ref:
            zip_ref.extractall(DEPOT_TOOLS_DIR)

        # remove archive
        os.remove(download_zip_filename)

        # run gclient once
        print('Install Windows-specific dependencies...')
        run_command(['gclient'], extra_env={'DEPOT_TOOLS_WIN_TOOLCHAIN': '0'})

    print('depot_tools is installed.')
    return True


def do_extra_git_config():
    run_command(['git.exe', 'config', '--global', 'core.autocrlf', 'false'])
    run_command(['git.exe', 'config', '--global', 'core.filemode', 'false'])
    run_command(['git.exe', 'config', '--global', 'branch.autosetuprebase', 'always'])
    return True


def check_prerequisites():
    return check_vs_version() and check_depot_tools() and do_extra_git_config()
