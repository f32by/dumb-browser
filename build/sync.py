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
import sys
from distutils.dir_util import copy_tree

from constants import PROJECT_DIR, DUMB_SRC_DIR, CHROMIUM_SRC_DIR
from utils import run_command, get_chromium_version, check_patch_consistency, apply_patches


def main(args):
    chromium_version = get_chromium_version()

    # run gclient
    ret = run_command(['gclient', 'sync',
                       '--reset',
                       '--revision', 'src@refs/tags/%s' % chromium_version,
                       '--force',
                       '--with_tags',
                       '--with_branch_heads',
                       '--upstream'],
                      cwd=PROJECT_DIR)

    if ret != 0:
        print('Failed to run gclient.')
        return 1

    # apply patches
    if not check_patch_consistency(treat_as_fatal=True):
        print('Please solve these problems before apply any patch.')
        return 1

    apply_patches()

    # copy dumb-specific sources to src
    print('Copying Dumb Browser source files to Chromium source root...')
    copy_tree(DUMB_SRC_DIR, os.path.join(CHROMIUM_SRC_DIR, 'dumb'))

    print('Copying resources...')
    copy_tree(os.path.join(PROJECT_DIR, 'resources', 'chrome'), CHROMIUM_SRC_DIR)

    print("Project synchronization finished. Now you can run 'build/build.py' to build Dumb Browser.")

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
