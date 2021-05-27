#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2020 f32by
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

CWD = os.path.dirname(os.path.realpath(__file__))
PROJECT_DIR = os.path.realpath(os.path.join(CWD, '..'))

CHROMIUM_VERSION_FILE = os.path.join(PROJECT_DIR, 'config', 'VERSION')
CHROMIUM_SRC_DIR = os.path.join(PROJECT_DIR, 'src')
DUMB_OVERWRITE_CHROMIUM_SRC_DIR = os.path.join(PROJECT_DIR, 'chromium_src')
DUMB_SRC_DIR = os.path.join(PROJECT_DIR, 'dumb')
DUMB_DST_DIR = os.path.join(CHROMIUM_SRC_DIR, 'dumb')
DEPOT_TOOLS_DIR = os.path.join(PROJECT_DIR, 'depot_tools')
PATCH_LIST_FILE = os.path.join(PROJECT_DIR, 'config', 'PATCHES')
PATCHES_DIR = os.path.join(PROJECT_DIR, 'patches')


def get_env():
    current_env = os.environ.copy()
    current_env['PATH'] = DEPOT_TOOLS_DIR + os.pathsep + current_env['PATH']
    # disable gclient metrics
    current_env['DEPOT_TOOLS_METRICS'] = '0'
    return current_env
