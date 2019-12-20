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

import os
from datetime import datetime


def load(preferencesFileName):
    if not os.path.exists(preferencesFileName):
        __createDefaultPreferences(preferencesFileName)
    return __loadPreferences(preferencesFileName)


def __createDefaultPreferences(fileName):
    with open(fileName, 'w+') as file:
        dateString = datetime.today().strftime('%Y_%m_%d_%H_%M_%S')
        file.write('activeSession=' + dateString + '.session')


def __loadPreferences(fileName):
    preferences = {}
    with open(fileName, 'r') as file:
        line = file.readline()
        if line != '':
            splitLine = line.split('=')
            preferences[splitLine[0]] = splitLine[1]
    return preferences
