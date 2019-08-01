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

import sys
import glob
import serial
import os

if sys.platform.startswith('win'):
    ports = ['COM%s' % (i + 1) for i in range(256)]
elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
    # this excludes your current terminal "/dev/tty"
    ports = glob.glob('/dev/tty[A-Za-z]*')
elif sys.platform.startswith('darwin'):
    ports = glob.glob('/dev/tty.*')
else:
    raise EnvironmentError('Unsupported platform')

results = []
for port in ports:
    try:
        s = serial.Serial(port)
        s.close()
        results.append(port)
    except (OSError, serial.SerialException):
        pass

print("These are the available UART Ports:")
for result in results:
    print(result)

if sys.platform.startswith('win'):
    #a = os.system("REG QUERY HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM")
    a = os.popen("REG QUERY HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM").read()
    print(a)
else:
    raise EnvironmentError('Unsupported platform')

#https://ch.mathworks.com/matlabcentral/fileexchange/45675-identify-serial-com-devices-by-friendly-name-in-windows
#HKEY_LOCAL_MACHINE\HARDWARE\DEVICEMAP\SERIALCOMM
#HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\