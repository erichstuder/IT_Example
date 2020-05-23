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
		for index in range(len(data)-1, 0, -1):
			byte = data[index]
			if byte == TelegramParser.__TelegramEndId:
				endIndex = index;
			elif byte == TelegramParser.__TelegramStartId:
				startIndex = index;
				telegrams = []
				TelegramParser.__splitTelegramStream(data[startIndex:endIndex+1], telegrams)
				TelegramParser.__parse(telegrams)
				telegram = telegrams[0]
				if telegram["valueName"] == valueName and telegram["valid"] == True:
					return telegram
		return None

	@staticmethod
	def getTelegramsAfterTimestamp(data: bytes, valueName, lowestTimestamp):
		telegrams = [];
		for index in range(len(data)-1, 0, -1):
			byte = data[index]
			if byte == TelegramParser.__TelegramEndId:
				endIndex = index;
			elif byte == TelegramParser.__TelegramStartId:
				startIndex = index;
				telegramsTemp = []
				TelegramParser.__splitTelegramStream(data[startIndex:endIndex+1], telegramsTemp)
				TelegramParser.__parse(telegramsTemp)
				telegram = telegramsTemp[0]
				if telegram["timestamp"] <= lowestTimestamp:
					return telegrams
				elif telegram["valueName"] == valueName and telegram["valid"] == True:
					telegrams.insert(0, telegram)
		return None

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
				telegramEmpty = TelegramParser.__parseString(telegram, telegramNoTelegramType)
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