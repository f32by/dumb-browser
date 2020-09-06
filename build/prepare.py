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

import sys


def main(args):
    if sys.platform == 'darwin':
        from util_darwin import check_prerequisites
    elif sys.platform == 'win32':
        from util_win32 import check_prerequisites
    else:
        raise Exception('Your platform ', sys.platform, ' is not supported.')

    check_prerequisites()

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
