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

import serial  # install pyserial to gain access
import time


class State:
    Init = 1
    Connecting = 2
    Reading = 3
    ErrorNoExit = 4


comPort = "COM4"
state = State.Connecting
oldState = State.Init
while True:
    if state == State.Connecting:
        if oldState != state:
            print(">> Connecting")
            oldState = state
        try:
            serialPort = serial.Serial(comPort)
            state = State.Reading
        except serial.serialutil.SerialException as e:
            time.sleep(1)
        except Exception as e:
            state = State.ErrorNoExit
            print("Unexpected exception occurred:")
            print(e)
    elif state == State.Reading:
        if oldState != state:
            print(">> Reading")
            oldState = state
        try:
            print(serialPort.read().decode("utf-8"), end='')
        except serial.serialutil.SerialException as e:
            state = State.Connecting
        except Exception as e:
            state = State.ErrorNoExit
            print("Unexpected exception occurred:")
            print(e)
    elif state == State.ErrorNoExit:
        if oldState != state:
            print(">> ErrorNoExit")
            oldState = state
