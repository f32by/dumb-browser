#!/usr/bin/env python3
# -*- coding] = utf-8 -*-

# Copyright (C) 2020 f32by
# This program is free software] = you can redistribute it and/or modify
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
from collections import OrderedDict

from constants import CHROMIUM_SRC_DIR
from utils import run_command

gn_args = OrderedDict()
# release build
gn_args['is_debug'] = 'false'
gn_args['is_official_build'] = '!is_debug'
# no debug symbols
gn_args['blink_symbol_level'] = '0'
gn_args['symbol_level'] = '0'
# disable click to call
gn_args['enable_click_to_call'] = 'false'
gn_args['enable_dsyms'] = 'false'
gn_args['enable_hangout_services_extension'] = 'false'
gn_args['enable_iterator_debugging'] = 'false'
# disable mDNS discovery
gn_args['enable_mdns'] = 'false'
gn_args['enable_media_remoting'] = 'false'
# no NaCl
gn_args['enable_nacl'] = 'false'
gn_args['enable_nacl_nonsfi'] = 'false'
# no readling list
gn_args['enable_reading_list'] = 'false'
gn_args['enable_remoting'] = 'false'
# disable reporting
gn_args['enable_reporting'] = 'false'
# disable service discovery
gn_args['enable_service_discovery'] = 'false'
# enable Widevine DRM
gn_args['enable_widevine'] = 'true'
gn_args['exclude_unwind_tables'] = 'true'
gn_args['fieldtrial_testing_like_official_build'] = 'true'
# use proprietary codecs
gn_args['ffmpeg_branding'] = '"Chrome"'
gn_args['proprietary_codecs'] = 'true'
# disable Safe Browsing
gn_args['safe_browsing_mode'] = '0'
gn_args['use_official_google_api_keys'] = 'false'
# use Chromium branding
gn_args['is_chrome_branded'] = 'false'
gn_args['use_unofficial_version_number'] = 'true'
# enable V8 pointer compression
gn_args['v8_enable_pointer_compression'] = 'true'


def main(args):
    parser = argparse.ArgumentParser(description='Dumb Browser build script')

    parser.add_argument('build_type',
                        help='Build type. Default is release.',
                        choices=['debug', 'release'],
                        default='release')

    parser.add_argument('--full-symbol',
                        help='Build with full debug symbols. This will increase build time.',
                        action='store_true')
    parser.add_argument('--component-build',
                        help='Set is_component_build = true. This can speed up increment build.',
                        action='store_true')
    parser.add_argument('--use-ccache',
                        help='Use ccache to speed up increment build.',
                        action='store_true')
    parser.add_argument('--print-only',
                        help='Show GN args but do not build.',
                        action='store_true')
    parser.add_argument('--skip-gn',
                        help='Skip `gn gen` and call autoninja directly',
                        action='store_true')

    args = parser.parse_args()

    build_dir = 'Debug' if args.build_type == 'debug' else 'Release'
    out_dir = os.path.join(CHROMIUM_SRC_DIR, 'out', build_dir)
    path_for_gn = os.path.join('out', build_dir)

    if args.skip_gn:
        return run_command(['autoninja', '-C', path_for_gn, 'chrome'], cwd=CHROMIUM_SRC_DIR)

    # generate GN args
    if args.build_type == 'debug':
        gn_args['is_debug'] = 'true'
        gn_args['symbol_level'] = '1'
    elif args.build_type == 'release':
        gn_args['is_debug'] = 'false'

    if args.full_symbol:
        gn_args['symbol_level'] = '2'

    if args.component_build:
        gn_args['is_component_build'] = 'true'

    if args.use_ccache:
        gn_args['cc_wrapper'] = '"ccache"'

    if args.print_only:
        for k, v in gn_args.items():
            print(k, '=', v)
        return 0

    # create build dir
    if not os.path.exists(out_dir):
        print('Creating output dir...')
        os.mkdir(out_dir)

    # write args.gn file
    if not os.path.exists(os.path.join(out_dir, 'args.gn')):
        print('Creating args.gn...')
        with open(os.path.join(out_dir, 'args.gn'), 'w') as f:
            for k, v in gn_args.items():
                f.write(f'{k} = {v}\n')

    # run gn command
    run_command(['gn', 'gen', path_for_gn], cwd=CHROMIUM_SRC_DIR)

    # build
    return run_command(['autoninja', '-C', path_for_gn, 'chrome'], cwd=CHROMIUM_SRC_DIR)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
