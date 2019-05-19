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

import argparse
from uart import connect

# def connect(args):
#     print("not implemented yet")


parser = argparse.ArgumentParser()
parser.add_argument('--version', action='version', version='0.0.0')
subparsers = parser.add_subparsers()

connectParser = subparsers.add_parser('connect')
# connectParser.add_argument('name', help='name of the person to greet')
# connectParser.add_argument('--greeting', default='Hello', help='word to use for the greeting')
# connectParser.add_argument('--caps', action='store_true', help='uppercase the output')
connectParser.set_defaults(func=connect)

if __name__ == '__main__':
    args = parser.parse_args()
    args.func(args)
