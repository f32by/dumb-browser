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
import subprocess

from constants import DEPOT_TOOLS_DIR
from utils import run_command


def check_xcode_version(required_version=11):
    print('Checking Xcode version, minimum required version is %d.' % required_version)
    ret = subprocess.check_output(['system_profiler', 'SPDeveloperToolsDataType']).decode('utf-8')
    pos = ret.find('Xcode: ')

    if pos == -1:
        print('Error: Xcode is not installed.')
        return False

    next_space = ret.find(' ', pos + 7)

    major_version = int(ret[pos + 7:next_space].split('.')[0])

    print('Your Xcode major version is %d.' % major_version)

    if major_version < required_version:
        print('Please install newer Xcode from AppStore.')
        return False

    return True


def check_depot_tools():
    print('Checking depot_tools...')
    if not os.path.exists(DEPOT_TOOLS_DIR):
        print('Downloading depot_tools...')
        ret = run_command(['git', 'clone', 'https://chromium.googlesource.com/chromium/tools/depot_tools.git', DEPOT_TOOLS_DIR])

        if ret != 0:
            print('Error: Failed to download depot_tools')
            return False

    print('depot_tools is installed.')
    return True


def do_extra_git_config():
    run_command(['git', 'config', '--global', 'core.precomposeUnicode', 'true'])
    return True


def check_prerequisites():
    return check_xcode_version() and check_depot_tools() and do_extra_git_config()
