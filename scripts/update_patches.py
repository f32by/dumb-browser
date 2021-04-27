#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2020 f32
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
import re
from shutil import copyfile

from ordered_set import OrderedSet

from constants import CHROMIUM_SRC_DIR, PATCHES_DIR, PATCH_LIST_FILE, DUMB_OVERWRITE_CHROMIUM_SRC_DIR
from utils import get_patch_filename, get_original_filename

GIT_DIFF_PATTERN = b'diff --git '

EXCLUSION_FILES = [
    'chrome/app/theme/chromium/BRANDING',
]


def main(args):
    print('Loading difference from git...')
    new_patch_list = OrderedSet()

    unified_diff = subprocess.check_output(['git', 'diff'], cwd=CHROMIUM_SRC_DIR)
    diffs = unified_diff.split(GIT_DIFF_PATTERN)
    diffs.remove(b'')
    total = len(diffs)
    regex = re.compile(rb'(?<=a/)(.*)(?= b/)')

    print('Updating patches...')
    i = 0
    for entry in diffs:
        i += 1

        filename = regex.findall(entry)[0].decode('utf-8')
        # skip dumb source
        if filename.startswith('dumb/'):
            continue

        if filename in EXCLUSION_FILES:
            # copy file to dumb_src
            msg = f'Copying {filename}'
            copyfile(os.path.join(CHROMIUM_SRC_DIR, filename),
                     os.path.join(DUMB_OVERWRITE_CHROMIUM_SRC_DIR, filename))
            skip = True
        else:
            msg = f'Generating patch for {filename}'
            skip = False

        print('[%d/%d]' % (i, total), msg)

        if skip:
            continue

        new_patch_list.add(filename)
        patch_filename = get_patch_filename(filename)

        # write to file
        with open(os.path.join(PATCHES_DIR, patch_filename), 'wb') as f:
            f.write(GIT_DIFF_PATTERN)
            f.write(entry)

    # remove old patches
    if os.path.exists(PATCH_LIST_FILE):
        print('Removing old patches...')

        removed_count = 0
        old_patches = os.listdir(PATCHES_DIR)
        for op in old_patches:
            if not op.endswith('.patch'):
                continue

            orig_filename = get_original_filename(op)
            if orig_filename not in new_patch_list:
                os.remove(os.path.join(PATCHES_DIR, op))
                removed_count += 1
        print('Removed %d old patch(es).' % removed_count)

    print('Writing new patch list...')
    with open(PATCH_LIST_FILE, 'w') as f:
        f.writelines('\n'.join(new_patch_list))

    print('Patches updated.')

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
