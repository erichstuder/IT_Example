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

import struct


class TelegramParser:
    __TelegramStartId = 0xAA
    __TelegramEndId = 0xBB
    __ReplacementMarker = 0xCC

    __ValueTypeInt8 = 0x01
    __ValueTypeUint8 = 0x02
    __ValueTypeFloat = 0x03

    __TelegramType_SingleRequestAnswer = 0x01

    __telegramRawOriginal = []
    __telegramRaw = []
    __telegram = {}

    def __init__(self, telegramReceivedCallback, invalidTelegramCallback):
        self.__telegramReceivedCallback = telegramReceivedCallback
        self.__invalidTelegramCallback = invalidTelegramCallback
        self.__telegramRaw.clear()

    def parse(self, byte):
        if byte == self.__TelegramStartId and len(self.__telegramRaw) != 0:
            self.__parseTelegram()
        self.__telegramRaw.append(byte)
        if byte == self.__TelegramEndId:
            self.__parseTelegram()

    def __parseTelegram(self):
        self.__telegram.clear()
        try:
            self.__telegramRawOriginal = self.__telegramRaw.copy()
            self.__handleReplacementMarkers()
            self.__handleTelegramStart(index=0)
            self.__handleTelegramType(index=1)
            valueNameLength = self.__handleValueName(index=2)
            (valueType, valueLength) = self.__handleValueType(index=valueNameLength+3)
            self.__handleValue(index=valueNameLength+4, valueType=valueType)
            totalOffset = valueNameLength + (valueLength - 1)
            self.__handleTimestamp(index=totalOffset+5)
            self.__handleTelegramEnd(index=totalOffset+9)
            self.__telegramReceivedCallback(self.__telegram.copy())
        except (IndexError, ValueError):
            self.__handleInvalidTelegram()
        finally:
            self.__telegramRaw.clear()

    def __handleReplacementMarkers(self, startIndex=0):
        if startIndex >= len(self.__telegramRaw):
            pass
        elif self.__telegramRaw[startIndex] == self.__ReplacementMarker:
            self.__telegramRaw[startIndex+1] += 1
            del self.__telegramRaw[startIndex]
            self.__handleReplacementMarkers(startIndex+2)
        else:
            self.__handleReplacementMarkers(startIndex+1)

    def __handleTelegramStart(self, index):
        if self.__telegramRaw[index] != self.__TelegramStartId:
            raise ValueError

    def __handleTelegramType(self, index):
        byte = self.__telegramRaw[index]
        if byte == self.__TelegramType_SingleRequestAnswer:
            self.__telegram['telegramType'] = byte
            return 1
        else:
            raise ValueError

    def __handleValueName(self, index):
        byte = self.__telegramRaw[index]
        if 'valueName' not in self.__telegram:
            self.__telegram['valueName'] = ''
        if byte == ord('\0'):
            return 0
        else:
            self.__telegram['valueName'] += chr(byte)
            return self.__handleValueName(index + 1) + 1

    def __handleValueType(self, index):
        byte = self.__telegramRaw[index]
        self.__telegram['valueType'] = byte
        if byte == self.__ValueTypeInt8 or byte == self.__ValueTypeUint8:
            return byte, 1
        elif byte == self.__ValueTypeFloat:
            return byte, 4
        else:
            raise ValueError

    def __handleValue(self, index, valueType):
        if valueType == self.__ValueTypeInt8 or valueType == self.__ValueTypeUint8:
            self.__telegram['value'] = self.__telegramRaw[index]
        elif valueType == self.__ValueTypeFloat:
            self.__telegram['value'] = struct.unpack('f', bytes(self.__telegramRaw[index:index+4]))[0]

    def __handleTimestamp(self, index):
        self.__telegram['timestamp'] = int.from_bytes(self.__telegramRaw[index:index+4], byteorder='big', signed=False)

    def __handleTelegramEnd(self, index):
        if self.__telegramRaw[index] != self.__TelegramEndId:
            raise ValueError
        if len(self.__telegramRaw) != index + 1:
            raise ValueError

    def __handleInvalidTelegram(self):
        self.__invalidTelegramCallback(self.__telegramRawOriginal)
