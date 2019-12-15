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
from app.TelegramParser import TelegramParser
import pytest
import mock

"""
Test List:
- parse function
- error callback wenn mittendrin 0xAA
- entfernen von 0xCC
- übergabe von nicht byte grossem wert muss zu fehler führen
- soll nur unsigned char akzeptiert werden? also keine negativen werte?

- 
"""


class TestTelegramParser:
    telegramParser = None
    invalidTelegramReceived = None

    def setup_method(self):
        self.invalidTelegramReceived = mock.Mock()
        self.telegramParser = TelegramParser(self.invalidTelegramReceived)

    def test_parseEmptyTelegramWithoutEnd(self):
        self.__parseStart()
        self.invalidTelegramReceived.assert_not_called()

        self.__parseStart()
        self.invalidTelegramReceived.assert_called_once()
        self.invalidTelegramReceived.assert_called_with({'telegramStart': [0xAA]})

        self.__parseStart()
        assert self.invalidTelegramReceived.call_count == 2
        self.invalidTelegramReceived.assert_called_with({'telegramStart': [0xAA]})

    def test_parseEmptyTelegram(self):
        self.__parseStart()
        self.invalidTelegramReceived.assert_not_called()
        self.__parseEnd()
        self.invalidTelegramReceived.assert_called_once()
        self.invalidTelegramReceived.assert_called_with({'telegramStart': [0xAA], 'telegramEnd': [0xBB]})

        self.__parseStart()
        self.invalidTelegramReceived.assert_called_once()
        self.__parseEnd()
        assert self.invalidTelegramReceived.call_count == 2
        self.invalidTelegramReceived.assert_called_with({'telegramStart': [0xAA], 'telegramEnd': [0xBB]})

    def test_startAfterTelegramType(self):
        self.__parseStart()
        TelegramType = 0x01
        self.__parseTelegramType(TelegramType)
        self.invalidTelegramReceived.assert_not_called()
        self.__parseStart()
        self.invalidTelegramReceived.assert_called_once()
        self.invalidTelegramReceived.assert_called_with({'telegramStart': [0xAA], 'telegramType': [TelegramType]})

    def test_invalidTelegramType(self):
        self.__parseStart()
        TelegramType = 0x67
        self.__parseTelegramType(TelegramType)
        self.invalidTelegramReceived.assert_called_once()
        self.invalidTelegramReceived.assert_called_with({'telegramStart': [0xAA], 'telegramType': [TelegramType]})

    def test_emptyValueName(self):
        self.__setupForParsingValueName()
        self.__parseValueName('')
        self.invalidTelegramReceived.assert_not_called()

    def test_valueName(self):
        self.__setupForParsingValueName()
        self.__parseValueName('myValueName')
        self.invalidTelegramReceived.assert_not_called()

    def test_tooLongValueName(self):
        self.__setupForParsingValueName()
        tenChars = '0123456789'
        fiftyChars = tenChars + tenChars + tenChars + tenChars + tenChars
        chars102 = fiftyChars + fiftyChars + 'eg'
        self.__parseValueName(chars102)
        self.invalidTelegramReceived.assert_called_once()
        expectedTelegram = {'telegramStart': [0xAA], 'telegramType': [0x01], 'valueName': list(chars102[:100])}
        self.invalidTelegramReceived.assert_called_with(expectedTelegram)

    def __parseStart(self):
        self.telegramParser.parse(0xAA)

    def __parseEnd(self):
        self.telegramParser.parse(0xBB)

    def __parseTelegramType(self, telegramType):
        self.telegramParser.parse(telegramType)

    def __parseValueName(self, name):
        for x in name:
            self.telegramParser.parse(x)

    def __assertInvalidTelegramCallbackNotCalled(self):
        self.invalidTelegramReceived.assert_not_called()

    def __setupForParsingValueName(self):
        self.__parseStart()
        self.__parseTelegramType(telegramType=0x01)
