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
from collections import namedtuple


class TelegramParser:
    __TelegramStartId = 0xAA
    __TelegramEndId = 0xBB
    __ReplacementMarker = 0xCC

    @staticmethod
    def parseStream(data: bytes):
        telegrams = []
        TelegramParser.__splitTelegramStream(data, telegrams)
        TelegramParser.__parse(telegrams)
        return telegrams

    @staticmethod
    def __splitTelegramStream(data, telegrams):
        TelegramParser.__startNewTelegram(telegrams)
        byteOld = 0
        for byte in data:
            if byteOld == TelegramParser.__TelegramEndId or byte == TelegramParser.__TelegramStartId:
                TelegramParser.__startNewTelegram(telegrams)
            telegrams[-1]['raw'] += bytes([byte])
            byteOld = byte

    @staticmethod
    def __startNewTelegram(telegrams):
        if len(telegrams) == 0:
            telegrams.append({'raw': b''})
        elif telegrams[-1]['raw'] != b'':
            telegrams.append({'raw': b''})

    @staticmethod
    def __parse(telegrams):
        for telegram in telegrams:
            if telegram['raw'][0] != TelegramParser.__TelegramStartId \
               or telegram['raw'][-1] != TelegramParser.__TelegramEndId:
                telegram['valid'] = False
                continue
            telegramRawLocal = telegram['raw']
            telegramNoReplacementMarkers = TelegramParser.__parseReplacementMarkers(telegramRawLocal)
            telegramNoStartAndEnd = telegramNoReplacementMarkers[1:-1]
            telegramNoTelegramType = TelegramParser.__parseTelegramType(telegram, telegramNoStartAndEnd)
            if telegram['telegramType'] == 'value':
                telegramNoValueName = TelegramParser.__parseValueName(telegram, telegramNoTelegramType)
                telegramNoValueType = TelegramParser.__parseValueType(telegram, telegramNoValueName)
                telegramNoValue = TelegramParser.__parseValue(telegram, telegramNoValueType)
                telegramEmpty = TelegramParser.__parseTimestamp(telegram, telegramNoValue)

            elif telegram['telegramType'] == 'string':
                pass  # todo Implement
            else:
                raise ValueError('not implemented')

            telegram['valid'] = True

    @staticmethod
    def __parseReplacementMarkers(telegramStream):
        telegramStreamWithoutReplacementMarkers = b''
        offset = 0
        for byte in telegramStream:
            if byte == TelegramParser.__ReplacementMarker:
                offset += 1
            else:
                telegramStreamWithoutReplacementMarkers += bytes([byte + offset])
                offset = 0
        return telegramStreamWithoutReplacementMarkers

    @staticmethod
    def __appendToKey(telegram, key, value):
        if key not in telegram:
            telegram.update({key: value})
        else:
            telegram[key] += value

    @staticmethod
    def __parseTelegramType(telegram, telegramNoStartAndEnd):
        telegramTypes = {
            1: 'value',
            2: 'string',
        }
        telegram['telegramType'] = telegramTypes.get(telegramNoStartAndEnd[0])
        return telegramNoStartAndEnd[1:]

    @staticmethod
    def __parseValueName(telegram, telegramNoTelegramType):
        name = ''
        nameLength = 0
        for byte in telegramNoTelegramType:
            if byte == 0:
                telegram['valueName'] = name
                return telegramNoTelegramType[nameLength+1:]
            name += chr(byte)
            nameLength += 1

    @staticmethod
    def __parseValueType(telegram, telegramNoValueName):
        valueTypes = {
            1: 'int8',
            2: 'uint8',
            3: 'ulong',
            4: 'float',
        }
        telegram['valueType'] = valueTypes.get(telegramNoValueName[0])
        return telegramNoValueName[1:]

    @staticmethod
    def __parseValue(telegram, telegramNoValueType):
        if telegram['valueType'] == 'ulong':
            telegram['value'] = struct.unpack('L', bytes(telegramNoValueType[:4]))[0]
            return telegramNoValueType[4:]
        elif telegram['valueType'] == 'float':
            telegram['value'] = struct.unpack('f', bytes(telegramNoValueType[:4]))[0]
            return telegramNoValueType[4:]

    @staticmethod
    def __parseTimestamp(telegram, telegramNoValue):
        telegram['timestamp'] = struct.unpack('L', bytes(telegramNoValue[:4]))[0]
        return telegramNoValue[4:]

"""
    __telegramRawOriginal = []
    __telegramRaw = []
    __telegram = {}
    
    def __init__(self, telegramReceivedCallback, invalidTelegramCallback):
        self.__telegramReceivedCallback = telegramReceivedCallback
        self.__invalidTelegramCallback = invalidTelegramCallback
        self.__telegramRaw.clear()
"""

"""
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
"""
