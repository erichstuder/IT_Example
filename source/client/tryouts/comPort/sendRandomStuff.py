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
import datetime

comPort = "COM6"
serialPort = serial.Serial(comPort)
serialPort.baudrate = 38400

counter = 1
while True:
    myString = str(datetime.datetime.now()).encode()
    myString += (" counter: " + format(counter, "08")).encode()
    myString += " some more data: 1041314, 27,   0,  0,26,0,0.00,  150,58,100,0.00,  32,80,  15.00,0.10,0.00,1,  0,0,40,30,50,3,70,75,85,325,350,375,285,2,2,1200".encode()
    myString += "\r\n".encode()
    print(myString)
    serialPort.write(myString)
    counter += 1
    time.sleep(1)
