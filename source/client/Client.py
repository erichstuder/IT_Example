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

# import time
# import serial  # install pyserial to gain access
# import sys
# import os
import queue
from app.ComPortHandler import ComPortHandler
# from app.TextFileViewer import TextFileViewer
# import app.Preferences as Preferences
# from app.TelegramParser import TelegramParser
import threading
import time
# from datetime import datetime

"""
def handleComPortInput(data):
    try:
        print(data.decode("utf-8"), end='')
    except UnicodeDecodeError:
        print('')
        print("Not utf-8: ", end='')
        print(data)
"""


class Client:
    def __init__(self):
        self.__keyboardInputQueue = queue.Queue()
        self.__keyboardReaderThread = threading.Thread(target=self.__keyboardReaderWorker)
        self.__keyboardReaderThread.daemon = True
        self.__keyboardReaderThread.start()
        self.__running = True
        self.__comPortHandler = ComPortHandler()

    def __keyboardReaderWorker(self):
        while True:
            self.__keyboardInputQueue.put(input())

    # only for testing at the moment
    def getKeyboardInputQueue(self):
        return self.__keyboardInputQueue

    def run(self):
        while self.__running:
            while not self.__keyboardInputQueue.empty():
                self.__keyboardInputParser(self.__keyboardInputQueue.get())

            while True:
                data = self.__comPortHandler.read()
                if data is not None:
                    with open('mySession.session', 'a+b') as sessionFile:
                        sessionFile.write(data)
                else:
                    break

            time.sleep(0.1)

    def __keyboardInputParser(self, keyBoardInput):
        if keyBoardInput == 'list comports':
            self.__comPortHandler.getFriendlyNames()
        elif keyBoardInput.startswith('start'):
            self.__comPortHandler.configure(port='COM4', baudrate=9600)
            print('started')
        elif keyBoardInput == 'exit':
            self.__running = False
        else:
            self.__comPortHandler.write(keyBoardInput + '\r')
            print('sent: ' + keyBoardInput)
            # send to server


if __name__ == "__main__":
    Client().run()

# def __telegramReceived(telegram):
#     with open(activeSession, 'a+') as logFile:
#         logFile.write(datetime.today().strftime('%d.%m.%Y %H:%M:%S\n'))
#         logFile.write(str(telegram) + '\n')
#         logFile.write(telegram['valueName'] + ': ' + str(telegram['value']) + '\n\n')


# def __invalidTelegram(telegram):
#     with open(activeSession, 'a+') as logFile:
#         logFile.write('invalid:\n')
#         logFile.write(''.join('{:02x} '.format(x) for x in telegram))
#         logFile.write('\n')
#         logFile.write(str(bytes(telegram)))
#         logFile.write('\n')
#         logFile.write('\n')




"""
comPortReceiver = ComPortReceiver(port='COM6', baudrate=9600)
comPortReceiver.setOnConnecting(lambda: print('comPortReceiver connecting'))
comPortReceiver.setOnConnected(lambda: print('comPortReceiver connected'))
comPortReceiver.setOnReceived(handleComPortInput)
comPortReceiver.setOnError(lambda e: print('comPortReceiver error: ' + e))
comPortReceiver.setOnErrorNoExit(lambda: print('comPortReceiver errorNoExit'))
comPortReceiver.start()
"""




"""
comPortHandler = ComPortHandler(port='COM8', baudrate=9600)
telegramParser = TelegramParser(telegramReceivedCallback=__telegramReceived, invalidTelegramCallback=__invalidTelegram)
"""

"""
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
