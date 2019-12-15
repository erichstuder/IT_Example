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

import queue
from enum import Enum


class TelegramParser:
    __TelegramStartId = 0xAA
    __TelegramEndId = 0xBB

    __telegram = {}

    class ParserState(Enum):
        WaitingForStart = 1
        WaitingForTelegramType = 2
        ReadingValueName = 3
        WaitingForEnd = 4

    def __init__(self, invalidTelegramCallback):
        self.state = self.ParserState.WaitingForStart
        self.invalidTelegramCallback = invalidTelegramCallback

    def parse(self, byte):
        if byte == self.__TelegramStartId:
            self.__handleTelegramStart(byte)
        elif byte == self.__TelegramEndId:
            self.__handleTelegramEnd(byte)
        elif self.state == self.ParserState.WaitingForTelegramType:
            self.__handleTelegramType(byte)
        elif self.state == self.ParserState.ReadingValueName:
            self.__handleValueName(byte)

    def __handleTelegramStart(self, byte):
        self.__handleUnexpectedStateTransition(expectedState=self.ParserState.WaitingForStart)
        self.__startTelegramParsing()
        self.state = self.ParserState.WaitingForTelegramType

    def __handleTelegramEnd(self, byte):
        self.__appendToTelegram('telegramEnd', byte)
        self.__handleUnexpectedStateTransition(expectedState=self.ParserState.WaitingForEnd)
        self.state = self.ParserState.WaitingForStart

    def __handleTelegramType(self, byte):
        self.__appendToTelegram('telegramType', byte)
        if byte == 0x67:
            self.__handleInvalidTelegram()
        else:
            self.state = self.ParserState.ReadingValueName

    def __handleValueName(self, byte):
        self.__appendToTelegram('valueName', byte)
        if len(self.telegram['valueName']) >= 100:
            self.__handleInvalidTelegram()
        # if byte != '\0':
        #     self.state = self.ParserState.ReadingValueDataType

    def __handleUnexpectedStateTransition(self, expectedState):
        if self.state != expectedState:
            self.__handleInvalidTelegram()

    def __handleInvalidTelegram(self):
        self.invalidTelegramCallback(dict(self.telegram))
        self.state = self.ParserState.WaitingForStart

    def __startTelegramParsing(self):
        self.telegram = {'telegramStart': [self.__TelegramStartId]}

    def __appendToTelegram(self, field, byte):
        if field in self.telegram:
            self.telegram[field].append(byte)
        else:
            self.telegram[field] = [byte]
