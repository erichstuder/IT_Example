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
import app.Preferences as Preferences
from unittest.mock import Mock
from unittest.mock import patch
import os
from datetime import datetime


@patch('app.Preferences.datetime', Mock(today=lambda: datetime(2019, 12, 20, 7, 7, 38, 484710)))
def test_preferenceFileNotExisting():
    FileName = 'testPreferencesJustCreated.test'
    if os.path.exists(FileName):
        os.remove(FileName)
    preferences = Preferences.load(FileName)
    assert preferences['activeSession'] == '2019_12_20_07_07_38.session'
    assert os.path.exists(FileName) is True
    with open(FileName, 'r') as file:
        assert file.read() == 'activeSession=2019_12_20_07_07_38.session'
    os.remove(FileName)



