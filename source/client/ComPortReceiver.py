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


def comPortReceiverWorker(port, baudrate, onReceiveCallback):
    state = ComPortReceiverState.Connecting
    oldState = ComPortReceiverState.Stopped
    serialPort = None
    data = None
    while True:
        if state == ComPortReceiverState.Connecting:
            if oldState != state:
                print(">> Connecting")
                oldState = state
            try:
                serialPort = serial.Serial(port)
                serialPort.baudrate = baudrate
                state = ComPortReceiverState.Reading
            except serial.serialutil.SerialException as e:
                time.sleep(1)
            except Exception as e:
                state = ComPortReceiverState.ErrorNoExit
                print("Unexpected exception occurred: ")
                print(e)
        elif state == ComPortReceiverState.Reading:
            if oldState != state:
                print(">> Reading")
                oldState = state
            try:
                data = serialPort.read()
                # print(data.decode("utf-8"), end='')
                onReceiveCallback(data.decode("utf-8"))
            except serial.serialutil.SerialException as e:
                state = ComPortReceiverState.Connecting
            except UnicodeDecodeError:
                print('')
                print("Not utf-8: ", end='')
                print(data)
            except Exception as e:
                state = ComPortReceiverState.ErrorNoExit
                print("Unexpected exception occurred: ")
                print(e)
        elif state == ComPortReceiverState.ErrorNoExit:
            if oldState != state:
                print(">> ErrorNoExit")
                oldState = state


class ComPortReceiver:
    def __init__(self, port, baudrate, onReceiveCallback):
        t = threading.Thread(target=comPortReceiverWorker, args=(port, baudrate, onReceiveCallback))
        t.start()
