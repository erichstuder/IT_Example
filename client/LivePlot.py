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

from IT_Client.helpers.TelegramParser import TelegramParser
from IT_Client.helpers.TelegramPlotter import TelegramPlotter

#os.system('mode 70,15')
#os.system("title LivePlot")

print("Starting up, may take a few seconds ...")

telegramParser = TelegramParser('mySession.session')
plotter = TelegramPlotter()
plotterFull = TelegramPlotter()

while True:
	telegram = telegramParser.getLastValue('Ki')
	if telegram is not None:
		print(telegram['value'])
		print('---')

	desiredValue = telegramParser.getLastValues('desiredValue', 10e6)
	actualValue = telegramParser.getLastValues('actualValue', 10e6)
	plotter.plot(desiredValue)
	plotter.plot(actualValue)
	plotter.update()
