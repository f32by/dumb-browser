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

import argparse
import os
import sys

from constants import CHROMIUM_SRC_DIR
from utils import run_command


def main(args):
    parser = argparse.ArgumentParser(description='Dumb Browser build script')

    parser.add_argument('build_type',
                        help='Build type. Default is release.',
                        choices=['debug', 'release'],
                        default='release')

    args = parser.parse_args()

    # create build dir
    build_dir = 'Debug' if args.build_type == 'debug' else 'Release'
    path_for_gn = os.path.join('out', build_dir)

    # build installer
    return run_command(['autoninja', '-C', path_for_gn, 'mini_installer'], cwd=CHROMIUM_SRC_DIR)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
