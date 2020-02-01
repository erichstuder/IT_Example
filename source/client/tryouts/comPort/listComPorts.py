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
import winreg
import logging


def getDeviceParametersKeys(_key):
    _keyHandle = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, _key, 0, winreg.KEY_READ)
    _deviceParametersKeys = []
    n = 0
    try:
        while True:
            subKey = winreg.EnumKey(_keyHandle, n)
            if subKey == r"Device Parameters":
                _deviceParametersKeys.append(_key)
            else:
                _deviceParametersKeys += getDeviceParametersKeys(_key + "\\" + subKey)
            n += 1
    except OSError as ex:
        if str(ex) == r"[WinError 259] Es sind keine Daten mehr verfügbar":
            pass
        elif str(ex) == r"[WinError 5] Zugriff verweigert":
            pass
        else:
            print("unexpected exception")
    except Exception as ex:
        logging.exception(ex)
        print("unexpected exception")
    winreg.CloseKey(_keyHandle)
    return _deviceParametersKeys


# if sys.platform.startswith('win'):
#     ports = ['COM%s' % (i + 1) for i in range(256)]
# elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
#     # this excludes your current terminal "/dev/tty"
#     ports = glob.glob('/dev/tty[A-Za-z]*')
# elif sys.platform.startswith('darwin'):
#     ports = glob.glob('/dev/tty.*')
# else:
#     raise EnvironmentError('Unsupported platform')
#
# results = []
# for port in ports:
#     try:
#         s = serial.Serial(port)
#         s.close()
#         results.append(port)
#     except (OSError, serial.SerialException):
#         pass
# print("These are the available UART Ports:")
# for result in results:
#     print(result)

if sys.platform.startswith('win'):
    keyHandle = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"HARDWARE\DEVICEMAP\SERIALCOMM", 0, winreg.KEY_READ)
    comPorts = []
    i = 0
    try:
        while True:
            comPorts.append(winreg.EnumValue(keyHandle, i)[1])
            i += 1
    except OSError as e:
        if str(e) != r"[WinError 259] Es sind keine Daten mehr verfügbar":
            print("unexpected exception")
    except Exception as e:
        logging.exception(e)
        print("unexpected exception")
    winreg.CloseKey(keyHandle)

    deviceParametersKeys = getDeviceParametersKeys(r"SYSTEM\CurrentControlSet\Enum")

    comPortKeys = [None]*len(comPorts)
    for key in deviceParametersKeys:
        try:
            keyHandle = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, key + "\\Device Parameters", 0, winreg.KEY_READ)
            portName = winreg.QueryValueEx(keyHandle, "PortName")[0]
            for i in range(len(comPorts)):
                if portName == comPorts[i]:
                    comPortKeys[i] = key
            winreg.CloseKey(keyHandle)
        except OSError as e:
            if str(e) == r"[WinError 259] Es sind keine Daten mehr verfügbar":
                pass
            elif str(e) == r"[WinError 2] Das System kann die angegebene Datei nicht finden":
                pass
            else:
                print("unexpected exception")
        except Exception as e:
            logging.exception(e)
            print("unexpected exception")

    comPortFriendlyNames = []
    for key in comPortKeys:
        keyHandle = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, key, 0, winreg.KEY_READ)
        comPortFriendlyNames.append(winreg.QueryValueEx(keyHandle, "FriendlyName")[0])
        winreg.CloseKey(keyHandle)

    print("COM ports with friendly name:")
    i = 0
    for i in range(len(comPorts)):
        print("{:<6}".format(comPorts[i]) + ": " + comPortFriendlyNames[i])
else:
    raise EnvironmentError('Unsupported platform')
input()
