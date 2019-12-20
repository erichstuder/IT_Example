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
import serial  # install pyserial to gain access
import sys
import os
import queue
from app.ComPortHandler import ComPortHandler
from app.TextFileViewer import TextFileViewer
import app.Preferences as Preferences
from app.TelegramParser import TelegramParser
import threading
from datetime import datetime

"""
def handleComPortInput(data):
    try:
        print(data.decode("utf-8"), end='')
    except UnicodeDecodeError:
        print('')
        print("Not utf-8: ", end='')
        print(data)
"""


def __inputReaderWorker():
    while True:
        inputQueue.put(input())


def __telegramReceived(telegram):
    with open(activeSession, 'a+') as logFile:
        logFile.write(datetime.today().strftime('%d.%m.%Y %H:%M:%S\n'))
        logFile.write(str(telegram) + '\n')
        logFile.write(telegram['valueName'] + ': ' + str(telegram['value']) + '\n\n')


def __invalidTelegram(telegram):
    with open(activeSession, 'a+') as logFile:
        logFile.write('invalid:\n')
        logFile.write(''.join('{:02x} '.format(x) for x in telegram))
        logFile.write('\n')
        logFile.write(str(bytes(telegram)))
        logFile.write('\n')
        logFile.write('\n')


inputQueue = queue.Queue()

"""
comPortReceiver = ComPortReceiver(port='COM6', baudrate=9600)
comPortReceiver.setOnConnecting(lambda: print('comPortReceiver connecting'))
comPortReceiver.setOnConnected(lambda: print('comPortReceiver connected'))
comPortReceiver.setOnReceived(handleComPortInput)
comPortReceiver.setOnError(lambda e: print('comPortReceiver error: ' + e))
comPortReceiver.setOnErrorNoExit(lambda: print('comPortReceiver errorNoExit'))
comPortReceiver.start()
"""
threading.Thread(target=__inputReaderWorker).start()
comPortHandler = ComPortHandler(port='COM8', baudrate=9600)
telegramParser = TelegramParser(telegramReceivedCallback=__telegramReceived, invalidTelegramCallback=__invalidTelegram)

preferences = Preferences.load('preferences.txt')
activeSession = preferences['activeSession']
if not os.path.exists(activeSession):
    open(activeSession, 'a').close()
TextFileViewer(activeSession)
while True:
    while not inputQueue.empty():
        inputData = inputQueue.get(block=False)
        inputData = inputData + '\r'
        comPortHandler.write(inputData)

    data = comPortHandler.read()
    if data is not None:
        telegramParser.parse(data)
    # time.sleep(0.01)


    """
    try:
        data = comPortHandler.read()
        print(data, end='')
    except UnicodeDecodeError:
        print('')
        print("Not utf-8: ", end='')
        print(data)
    time.sleep(0.01)
    """

    """
    cmd = input()
    if cmd == 'exit':
        print('goodbye ...')
        sys.stdout.flush()
        time.sleep(2)
        break
    else:
        # serialPort = serial.Serial(port='COM6', exclusive=False)
        # serialPort.baudrate = 9600
        # serialPort.write(cmd)
        print(cmd)
    """

    """
    elif cmd == 'status':
        print('status not yet implemented')
    elif cmd == 'start':
        print('start not yet implemented')
    elif cmd == 'stop':
        print('stop not yet implemented')
    else:
        print(cmd)
    """
