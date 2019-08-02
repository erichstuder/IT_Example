"""
IT - Internal Tracer
Copyright (C) 2019 Erich Studer

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""

import time
import sys


while True:
    cmd = input()
    if cmd == 'exit':
        print('goodbye ...')
        sys.stdout.flush()
        time.sleep(2)
        break
    elif cmd == 'status':
        print('status not yet implemented')
    elif cmd == 'start':
        print('start not yet implemented')
    elif cmd == 'stop':
        print('stop not yet implemented')
    else:
        print(cmd)
