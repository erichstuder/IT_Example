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


class ComPortHandler:
    def __init__(self, port, baudrate):
        self.__serialPort = serial.Serial(None)
        self.__serialPort.port = port
        self.__serialPort.baudrate = baudrate
        self.__serialPort.timeout = 0
        self.__onError = None

    def __open(self):
        try:
            self.__serialPort.open()
        except serial.serialutil.SerialException as e:
            pass  # connection failed: most probably a problem with the other device or just not connected
        except Exception as e:
            self.__onError('Unexpected exception while connecting to comport: ' + str(e))

    def write(self, data):
        if not self.__serialPort.is_open:
            self.__open()
        try:
            self.__serialPort.write(data.encode())
        except serial.serialutil.SerialException as e:
            self.__onError('Could not write data')
        except Exception as e:
            self.__onError('Unexpected exception while writing to comport: ' + str(e))

    def read(self):
        if not self.__serialPort.is_open:
            self.__open()
        try:
            return self.__serialPort.read()
        except serial.serialutil.SerialException as e:
            pass  # connection failed: most probably a problem with the other device or just not connected
        except Exception as e:
            self.__onError('Unexpected exception while reading from comport: ' + str(e))
