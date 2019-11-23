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
import threading


class ComPortReceiverState:
    Stopped = 1
    Connecting = 2
    Reading = 3
    ErrorNoExit = 4


class ComPortReceiver:
    def __init__(self, port, baudrate):
        self.t = threading.Thread(target=self.__comPortReceiverWorker, args=(port, baudrate))
        self.__onConnecting = None
        self.__onConnected = None
        self.__onDataReceived = None
        self.__onError = None
        self.__onErrorNoExit = None

    def start(self):
        self.t.start()

    def setOnConnecting(self, onConnecting):
        self.__onConnecting = onConnecting

    def setOnConnected(self, onConnected):
        self.__onConnected = onConnected

    def setOnReceived(self, onDataReceived):
        self.__onDataReceived = onDataReceived

    def setOnError(self, onError):
        self.__onError = onError

    def setOnErrorNoExit(self, onErrorNoExit):
        self.__onErrorNoExit = onErrorNoExit

    def __comPortReceiverWorker(self, port, baudrate):
        state = ComPortReceiverState.Connecting
        oldState = ComPortReceiverState.Stopped
        serialPort = None
        while True:
            if state == ComPortReceiverState.Connecting:
                if oldState != state:
                    self.__onConnecting()
                    oldState = state
                try:
                    serialPort = serial.Serial(port=port)
                    serialPort.baudrate = baudrate
                    state = ComPortReceiverState.Reading
                except serial.serialutil.SerialException as e:
                    time.sleep(1)
                except Exception as e:
                    state = ComPortReceiverState.ErrorNoExit
                    self.__onError(e)
            elif state == ComPortReceiverState.Reading:
                if oldState != state:
                    self.__onConnected()
                    oldState = state
                try:
                    data = serialPort.read()
                    self.__onDataReceived(data)
                except serial.serialutil.SerialException as e:
                    state = ComPortReceiverState.Connecting
                except Exception as e:
                    state = ComPortReceiverState.ErrorNoExit
                    self.__onError(e)
            elif state == ComPortReceiverState.ErrorNoExit:
                if oldState != state:
                    self.__onErrorNoExit()
                    print(">> ErrorNoExit")
                    oldState = state
