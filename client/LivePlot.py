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

from IT_Client.helpers.TelegramHandler import TelegramHandler
from IT_Client.helpers.TelegramPlotter import TelegramPlotter
from os import path
import time

def waitUntilFileExists(filePath):
	if not path.isfile(filePath):
		print('waiting for file to exist...')
	while not path.isfile(filePath):
		time.sleep(0.5)
	else:
		print('file exists now.')


print("Starting up, may take a few seconds ...")

sessionFile = 'mySession.session'
waitUntilFileExists(sessionFile)

telegramHandler = TelegramHandler(sessionFile)
plotter = TelegramPlotter('myPlot')

Ki_old = 'unknown'
while True:
	telegram = telegramHandler.getLastValue('Ki')
	if telegram is not None:
		Ki = telegram['value']
		if Ki != Ki_old:
			print('Ki: ' + str(Ki))
			Ki_old = Ki

	desiredValue = telegramHandler.getLastValues('desiredValue', 5e6)
	actualValue = telegramHandler.getLastValues('actualValue', 5e6)
	plotter.plot(desiredValue)
	plotter.plot(actualValue)
	plotter.update()



