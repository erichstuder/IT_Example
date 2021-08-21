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

import matplotlib.pyplot as plt
import numpy as np
import time
import os
import array

from IT_Client.helpers.TelegramParser import TelegramParser

#os.system('mode 70,15')
#os.system("title LivePlot")
plt.ion()
plt.style.use('dark_background')

print("Starting up, may take a few seconds ...")

while True:
	telegramParser = TelegramParser('mySession.session')
	telegram = telegramParser.getLastValueByName('Ki')
	print(telegram['value'])
	time.sleep(2)



import sys
sys.exit()




os.system('mode 70,15')
os.system("title LivePlot")
plt.ion()
plt.style.use('dark_background')

print("Starting up, may take a few seconds ...")

desiredValue_borderTimestamp = 0
actualValue_borderTimestamp = 0

desiredValue_telegrams = []
actualValue_telegrams = []

desiredValue_lastTelegramTimestamp_old = 0
actualValue_lastTelegramTimestamp_old = 0

desiredValue_data = []
desiredValue_time = []

actualValue_data = []
actualValue_time = []

timeWindow = 2e6

while True:
	
	#millis1 = int(round(time.time() * 1000))
	
	with open('mySession.session', 'rb') as sessionFile:
		data = sessionFile.read()
		
	Ki_lastTelegram = TelegramParser.parseLastValidTelegram(data, "Ki");
	Kp_lastTelegram = TelegramParser.parseLastValidTelegram(data, "Kp");
	
	os.system('cls')
	if Ki_lastTelegram == None:
		print('Kp:')
	else:
		Kp = Kp_lastTelegram["value"]
		print('Kp: ' + str(round(Kp, 3)).ljust(10) + '( ' + str(Kp) + ' )')
	if Kp_lastTelegram == None:
		print('Ki:')
	else:
		Ki = Ki_lastTelegram["value"]
		print('Ki: ' + str(round(Ki, 3)).ljust(10) + '( ' + str(Ki) + ' )')

	#print('')
	#print('Messages:')
	#for telegram in telegrams:
	#	if telegram['valid'] and telegram['telegramType'] == 'string':
	#		print(telegram['value'])
	
	desiredValue_lastTelegram = TelegramParser.parseLastValidTelegram(data, "desiredValue");
	actualValue_lastTelegram = TelegramParser.parseLastValidTelegram(data, "actualValue");
	
	desiredValue_lastTelegramTimestamp = desiredValue_lastTelegram["timestamp"]
	desiredValue_borderTimestamp = max(desiredValue_lastTelegramTimestamp_old, desiredValue_lastTelegramTimestamp - timeWindow)
	desiredValue_lastTelegramTimestamp_old = desiredValue_lastTelegramTimestamp
	
	actualValue_lastTelegramTimestamp = actualValue_lastTelegram["timestamp"]
	actualValue_borderTimestamp = max(actualValue_lastTelegramTimestamp_old, actualValue_lastTelegramTimestamp - timeWindow)
	actualValue_lastTelegramTimestamp_old = actualValue_lastTelegramTimestamp
	
	desiredValue_telegrams = TelegramParser.getTelegramsAfterTimestamp(data, "desiredValue", desiredValue_borderTimestamp)
	actualValue_telegrams = TelegramParser.getTelegramsAfterTimestamp(data, "actualValue", actualValue_borderTimestamp)
	
	for telegram in desiredValue_telegrams:
		if 'value' in telegram and 'timestamp' in telegram:
			desiredValue_data += [telegram['value']]
			desiredValue_time += [telegram['timestamp']]
	
	for telegram in actualValue_telegrams:
		if 'value' in telegram and 'timestamp' in telegram:
			actualValue_data += [telegram['value']]
			actualValue_time += [telegram['timestamp']]
	
	for index in range(len(desiredValue_time)):
		if desiredValue_time[index] > desiredValue_lastTelegramTimestamp - timeWindow:
			del desiredValue_data[0:max(index-1,0)]
			del desiredValue_time[0:max(index-1,0)]
			break
	
	for index in range(len(actualValue_time)):
		if actualValue_time[index] > actualValue_lastTelegramTimestamp - timeWindow:
			del actualValue_data[0:max(index-1,0)]
			del actualValue_time[0:max(index-1,0)]
			break
	
	figure = plt.figure(num='LivePlot', figsize=(8, 4))
	desiredValue_timeSeconds = [x/1e6 for x in desiredValue_time]
	actualValue_timeSeconds = [x/1e6 for x in actualValue_time]
	
	plt.clf()
	plt.step(desiredValue_timeSeconds, desiredValue_data, where='post')
	plt.step(actualValue_timeSeconds, actualValue_data, where='post')

	plt.legend(['desiredValue', 'actualValue'], loc='lower left')
	plt.xlabel('time [s]')

	figure.canvas.flush_events()
	
	continue
	
	
	print("last telegram done")
	print(telegram)
	print("")
	print(telegram['timestamp'])
	
	
	
	#fileReadOffset = len(data)
	
	
	
	telegrams = TelegramParser.parseStream(data)
	
	
	
	desiredValue_data = []
	desiredValue_time = []

	plantOut_data = []
	plantOut_time = []

	for telegram in reversed(telegrams):
		if 'timestamp' in telegram:
			latestTimestamp = telegram['timestamp']
			break

	earliestTimestamp = latestTimestamp - 10000000
	for telegram in reversed(telegrams):
		if 'value' in telegram and 'timestamp' in telegram and telegram['timestamp'] > earliestTimestamp:
			if telegram['valueName'] == 'desiredValue':
				desiredValue_data = [telegram['value']] + desiredValue_data
				desiredValue_time = [telegram['timestamp']] + desiredValue_time
			elif telegram['valueName'] == 'actualValue':
				plantOut_data = [telegram['value']] + plantOut_data
				plantOut_time = [telegram['timestamp']] + plantOut_time

	Kp = 0
	KpFound = False
	for telegram in reversed(telegrams):
		if telegram['valid'] and 'valueName' in telegram and telegram['valueName'] == 'Kp':
			Kp = telegram['value']
			KpFound = True
			break
	Ki = 0
	KiFound = False
	for telegram in reversed(telegrams):
		if telegram['valid'] and 'valueName' in telegram and telegram['valueName'] == 'Ki':
			Ki = telegram['value']
			KiFound = True
			break

	

	#time.sleep(1)
	#print(int(round(time.time() * 1000))-millis)

	figure = plt.figure(num='LivePlot', figsize=(8, 4))
	# figure.canvas.set_window_title('LivePlot')
	plt.clf()
	plt.step([x/1000000 for x in desiredValue_time], desiredValue_data, where='post')
	plt.step([x/1000000 for x in plantOut_time], plantOut_data, where='post')

	plt.legend(['desiredValue', 'actualValue'], loc='lower left')
	plt.xlabel('time [s]')

	figure.canvas.flush_events()
