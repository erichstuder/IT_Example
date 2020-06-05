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
	def parseLastValidTelegram(data: bytes, valueName):
		lastDataIndex = len(data)-1
		endIndex = lastDataIndex
		for index in range(lastDataIndex, 0, -1):
			byte = data[index]
			if byte == TelegramParser.__TelegramEndId:
				endIndex = index;
			elif byte == TelegramParser.__TelegramStartId:
				startIndex = index;
				telegrams = TelegramParser.__splitTelegramStream(data[startIndex:endIndex+1])
				TelegramParser.__parse(telegrams)
				telegram = telegrams[0]
				if telegram["valid"] == True and telegram["valueName"] == valueName:
					return telegram
		return None

	@staticmethod
	def getTelegramsAfterTimestamp(data: bytes, valueName, lowestTimestamp):
		telegrams = [];
		lastDataIndex = len(data)-1
		endIndex = lastDataIndex
		for index in range(lastDataIndex, 0, -1):
			byte = data[index]
			if byte == TelegramParser.__TelegramEndId:
				endIndex = index;
			elif byte == TelegramParser.__TelegramStartId:
				startIndex = index;
				telegramsTemp = TelegramParser.__splitTelegramStream(data[startIndex:endIndex+1])
				TelegramParser.__parse(telegramsTemp)
				telegram = telegramsTemp[0]
				if telegram["valid"] == True:
					if telegram["timestamp"] <= lowestTimestamp:
						return telegrams
					elif telegram["valueName"] == valueName:
						telegrams.insert(0, telegram)
		return None

	@staticmethod
	def parseStream(data: bytes):
		telegrams = TelegramParser.__splitTelegramStream(data)
		TelegramParser.__parse(telegrams)
		return telegrams

	@staticmethod
	def __splitTelegramStream(data):
		telegrams = [];
		if len(data) == 0:
			return telegrams
		
		TelegramParser.__startNewTelegram(telegrams)
		byteOld = 0
		for byte in data:
			if byteOld == TelegramParser.__TelegramEndId or byte == TelegramParser.__TelegramStartId:
				TelegramParser.__startNewTelegram(telegrams)
			telegrams[-1]['raw'] += bytes([byte])
			byteOld = byte
		return telegrams

	@staticmethod
	def __startNewTelegram(telegrams):
		if len(telegrams) == 0:
			telegrams.append({'raw': b''})
		elif telegrams[-1]['raw'] != b'':
			telegrams.append({'raw': b''})

	@staticmethod
	def __parse(telegrams):
		for telegram in telegrams:
                        try:
                                telegramNoStartAndEnd = TelegramParser.__parseStartAndEnd(telegram['raw'])
                                telegramNoReplacementMarkers = TelegramParser.__parseReplacementMarkers(telegramNoStartAndEnd)
                                telegramNoTelegramType = TelegramParser.__parseTelegramType(telegram, telegramNoReplacementMarkers)
                                if telegram['telegramType'] == 'value':
                                        telegramNoValueName = TelegramParser.__parseValueName(telegram, telegramNoTelegramType)
                                        telegramNoValueType = TelegramParser.__parseValueType(telegram, telegramNoValueName)
                                        telegramNoValue = TelegramParser.__parseValue(telegram, telegramNoValueType)
                                        telegramEmpty = TelegramParser.__parseTimestamp(telegram, telegramNoValue)
                                elif telegram['telegramType'] == 'string':
                                        telegramEmpty = TelegramParser.__parseString(telegram, telegramNoTelegramType)
                                else:
                                        raise ValueError('unknown telegram type')

                                telegram['valid'] = True
                        except ValueError:
                                telegram['valid'] = False

	@staticmethod
	def __parseStartAndEnd(telegramStream):
		if telegramStream[0] != TelegramParser.__TelegramStartId or telegramStream[-1] != TelegramParser.__TelegramEndId:
			raise ValueError('Parsing Start and End failed')
		return telegramStream[1:-1]

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
	def __parseTelegramType(telegram, telegramNoStartAndEnd):
		telegramTypes = {
			1: 'value',
			2: 'string',
		}
		telegramType = telegramTypes.get(telegramNoStartAndEnd[0])
		if telegramType == None:
			raise ValueError('invalid telegram type')
		telegram['telegramType'] = telegramType
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
		raise ValueError('value name has no terminator')

	@staticmethod
	def __parseValueType(telegram, telegramNoValueName):
		valueTypes = {
			1: 'int8',
			2: 'uint8',
			3: 'ulong',
			4: 'float',
		}
		if len(telegramNoValueName) == 0:
			raise ValueError('no value to parse')
		valueType = valueTypes.get(telegramNoValueName[0])
		if valueType == None:
			raise ValueError('invalid value type')
		telegram['valueType'] = valueType
		return telegramNoValueName[1:]

	@staticmethod
	def __parseValue(telegram, telegramNoValueType):
		if telegram['valueType'] == 'ulong':
			if len(telegramNoValueType) < 4:
				raise ValueError('not enough bytes to parse ulong')
			telegram['value'] = struct.unpack('L', bytes(telegramNoValueType[:4]))[0]
			return telegramNoValueType[4:]
		elif telegram['valueType'] == 'float':
			if len(telegramNoValueType) < 4:
				raise ValueError('not enough bytes to parse float')
			telegram['value'] = struct.unpack('f', bytes(telegramNoValueType[:4]))[0]
			return telegramNoValueType[4:]

	@staticmethod
	def __parseTimestamp(telegram, telegramNoValue):
		if len(telegramNoValue) < 4:
			raise ValueError('not enough bytes to parse timestamp')
		telegram['timestamp'] = struct.unpack('L', bytes(telegramNoValue[:4]))[0]
		return telegramNoValue[4:]

	@staticmethod
	def __parseString(telegram, telegramNoTelegramType):
		name = ''
		nameLength = 0
		for byte in telegramNoTelegramType:
			if byte == 0:
				telegram['value'] = name
				return telegramNoTelegramType[nameLength + 1:]
			name += chr(byte)
			nameLength += 1
		raise ValueError('string has no terminator')
