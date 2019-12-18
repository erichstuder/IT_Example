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
import mock

"""
Test List:
- entfernen von 0xCC
"""


class TestTelegramParser:
    __ValidTelegram = [0xAA, 0x01] + list('myValueName\0') + [0x01, 0xCC, 0xBA, 0x04, 0x03, 0x02, 0x01, 0xBB]

    def setup_method(self):
        self.invalidTelegram = mock.Mock()
        self.telegramReceived = mock.Mock()
        self.telegramParser = TelegramParser(self.telegramReceived, self.invalidTelegram)

    def test_parseEmptyTelegramWithoutEnd(self):
        self.__parseStart()
        self.telegramReceived.assert_not_called()
        self.invalidTelegram.assert_not_called()

        self.__parseStart()
        self.__assertInvalidTelegram([0xAA])

        self.__parseStart()
        self.telegramReceived.assert_not_called()
        assert self.invalidTelegram.call_count == 2
        self.invalidTelegram.assert_called_with([0xAA])

    def test_parseEmptyTelegram(self):
        self.__parseStart()
        self.telegramReceived.assert_not_called()
        self.invalidTelegram.assert_not_called()
        self.__parseEnd()
        self.__assertInvalidTelegram([0xAA, 0xBB])

        self.__parseStart()
        self.telegramReceived.assert_not_called()
        self.invalidTelegram.assert_called_once()
        self.__parseEnd()
        self.telegramReceived.assert_not_called()
        assert self.invalidTelegram.call_count == 2
        self.invalidTelegram.assert_called_with([0xAA, 0xBB])

    def test_startAfterTelegramType(self):
        self.__parseStart()
        TelegramType = 0x01
        self.__parseTelegramType(TelegramType)
        self.telegramReceived.assert_not_called()
        self.invalidTelegram.assert_not_called()
        self.__parseStart()
        self.__assertInvalidTelegram([0xAA, TelegramType])

    def test_invalidTelegramType(self):
        telegram = self.__telegramWithField('telegramType', 0x67)
        self.__parseTelegram(telegram)
        self.__assertInvalidTelegram(telegram)

    def test_emptyValueName(self):
        telegram = self.__telegramWithField('valueName', '')
        self.__parseTelegram(telegram)
        self.__assertInvalidTelegram(telegram)

    def test_emptyValueNameTerminated(self):
        telegram = self.__telegramWithField('valueName', '\0')
        self.__parseTelegram(telegram)
        expectedTelegram = {'telegramType': 0x01, 'valueName': '', 'valueType': 0x01, 'value': 0xBB,
                            'timestamp': 0x04030201}
        self.__assertValidTelegram(expectedTelegram)

    def test_invalidValueType(self):
        telegram = self.__telegramWithField('valueType', 0x33)
        self.__parseTelegram(telegram)
        self.__assertInvalidTelegram(telegram)

    def test_tooLongTelegram(self):
        tooLongTelegram = self.__ValidTelegram[0:-1] + [0xEE] + [self.__ValidTelegram[-1]]
        self.__parseTelegram(tooLongTelegram)
        self.__assertInvalidTelegram(tooLongTelegram)

    def test_validTelegram(self):
        self.__parseTelegram(self.__ValidTelegram)
        expectedTelegram = {'telegramType': 0x01, 'valueName': 'myValueName', 'valueType': 0x01, 'value': 0xBB,
                            'timestamp': 0x04030201}
        self.__assertValidTelegram(expectedTelegram)

    def __parseStart(self):
        self.telegramParser.parse(0xAA)

    def __parseEnd(self):
        self.telegramParser.parse(0xBB)

    def __parseTelegramType(self, telegramType):
        self.telegramParser.parse(telegramType)

    def __parseTelegram(self, telegram):
        localTelegram = telegram.copy()
        if len(localTelegram) > 0:
            self.telegramParser.parse(localTelegram.pop(0))
            self.__parseTelegram(localTelegram)

    def __telegramWithField(self, field, value):
        telegram = self.__ValidTelegram.copy()
        if field == 'telegramType':
            telegram[1] = value
        elif field == 'valueName':
            while telegram[2] != '\0':
                del telegram[2]
            if len(value) > 0:
                stringReverse = value[::-1]
                while stringReverse != '\0':
                    telegram.insert(2, stringReverse[0])
                    stringReverse = stringReverse[1:]
            else:
                del telegram[2]
        elif field == 'valueType':
            telegram[telegram[2:].index('\0') + 2 + 1] = value
        return telegram

    def __assertInvalidTelegram(self, parsedData):
        self.telegramReceived.assert_not_called()
        self.invalidTelegram.assert_called_once()
        self.invalidTelegram.assert_called_with(parsedData)

    def __assertValidTelegram(self, expectedTelegram):
        self.telegramReceived.assert_called_once()
        self.telegramReceived.assert_called_with(expectedTelegram)
        self.invalidTelegram.assert_not_called()

    # def __parseValueName(self, name):
    #     for x in name:
    #         self.telegramParser.parse(x)
    #
    # def __assertInvalidTelegramCallbackNotCalled(self):
    #     self.invalidTelegram.assert_not_called()
    #
    # def __setupForParsingValueName(self):
    #     self.__parseStart()
    #     self.__parseTelegramType(telegramType=0x01)
    #
    # def __setupForParsingValueType(self):
    #     self.__setupForParsingValueName()
    #     self.__parseValueName('randomValueName\0')
    #
    # def __setupForParsingValue(self):
    #     self.__setupForParsingValue()
    #     self.telegramParser.parse(0x01)
