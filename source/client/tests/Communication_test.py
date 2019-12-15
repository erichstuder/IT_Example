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

# Important to make this test run:
# - install MinGW with default settings and packages mingw32-bas-bin and msys-base-bin
# - add the following to Path variable
#     - C:\MinGW
#     - C:\MinGW\bin
#     - C:\MinGW\msys\1.0\bin
# - this test has to be executed with pytest in the folder where it is located


import mock
import os
from ctypes import *

"""
it = cdll.LoadLibrary("C:\\Users\\erich\\Documents\\myRepos\\IT\\source\\deviceWithServer\\itLibrary\\x64\\Release\\itLibrary.dll")
# it = WinDLL("C:\\Users\\erich\\Documents\\myRepos\\IT\\source\\deviceWithServer\\itLibrary\\x64\\Release\\itLibrary.dll")
a = it.itTick
# b = it.dummy
print(a)
# print(b)
a()
it.itTick
"""

def setup_module():
    print('now calling make')
    # os.system('make')
    # it = CDLL(os.path.dirname(__file__) + r'\it.so')
    # it = CDLL(r'C:\Users\erich\Documents\myRepos\IT\source\deviceWithServer\itLibrary\x64\Release\itLibrary.dll')
    it = cdll.LoadLibrary(r'C:\Users\erich\Documents\myRepos\IT\source\deviceWithServer\itLibrary\x64\Release\itLibrary.dll')
    # it = cdll.LoadLibrary("C:\\Users\\erich\\Documents\\myRepos\\IT\\source\\deviceWithServer\\itLibrary\\x64\\Release\\itLibrary.dll")

    # cdll.LoadLibrary(r'C:\Users\erich\Documents\myRepos\IT\source\deviceWithServer\itLibrary\Release\itLibrary.dll')
    # cdll.LoadLibrary(r'C:\Users\erich\Documents\myRepos\IT\source\client\tests' + r'\it.so')
    # print(it)
    # it.itInit(None, 0, None, None, None)
    print('test teeeesst')
    # a = c_ubyte(it.dummy()).value
    # print(a)

    it.itTick()


def test_dummy():
    assert 1 == 2
