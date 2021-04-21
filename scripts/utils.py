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
import subprocess
import sys

from constants import CHROMIUM_VERSION_FILE, PATCH_LIST_FILE, PATCHES_DIR, CHROMIUM_SRC_DIR, get_env


def get_chromium_version():
    with open(CHROMIUM_VERSION_FILE, 'r') as f:
        version = f.read().replace('\n', '').replace(' ', '')
    return version


def get_patch_filename(filename):
    return filename.replace('/', '@') + '.patch'


def get_original_filename(filename):
    return filename.replace('@', '/')[:-6]


def check_patch_consistency(treat_as_fatal=False):
    with open(PATCH_LIST_FILE, 'r') as f:
        patch_list = f.read().split('\n')

    patch_list = set(patch_list)
    patch_files = os.listdir(PATCHES_DIR)

    for p in patch_files:
        if not p.endswith('.patch'):
            print('Found non-patch file %s, please consider move or delete it.' % p)
            continue

        patch_filename = get_original_filename(p)
        if patch_filename not in patch_list:
            print('Found a patch file which is not listed in PATCHES:', p)
            if treat_as_fatal:
                return False
            continue
        else:
            patch_list.remove(patch_filename)

    if len(patch_list) > 0:
        print('Missing patch(es) for following file(s):')
        for missing in patch_list:
            print(missing)
        return False

    print('Patch consistency checked.')
    return True


def apply_patches(stop_when_failed=False):
    with open(PATCH_LIST_FILE, 'r') as f:
        patch_list = f.read().split('\n')

    print('Applying patches...')
    for p in patch_list:
        patch_filename = os.path.join(PATCHES_DIR, get_patch_filename(p))

        ret = run_command(['patch', '-p1',
                           '-i', patch_filename,
                           '-d', CHROMIUM_SRC_DIR,
                           '--no-backup-if-mismatch',
                           '--forward'])

        if not ret:
            print('Failed to patch file ', p)
            if stop_when_failed:
                return False

    print('Patches applied.')
    return True


def run_command(command, extra_env=None, cwd=None):
    print('$', ' '.join(command))

    env = get_env()
    if isinstance(extra_env, dict):
        for k, v in extra_env.items():
            env[k] = v

    use_shell = True if sys.platform == 'win32' else False

    proc = subprocess.Popen(command,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT,
                            shell=use_shell,
                            cwd=cwd,
                            env=env)
    while True:
        output = proc.stdout.readline()
        if len(output) == 0 and proc.poll() is not None:
            break

        print(output.decode(sys.stdout.encoding))

    return proc.returncode == 0
