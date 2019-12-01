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
    TelegramStartId = 0xAA

    class ParserState(Enum):
        WaitingForStart = 1
        WaitingForTelegramId = 2

    def __init__(self, noEndReceived):
        self.state = self.ParserState.WaitingForStart
        self.noEndReceived = noEndReceived
        # self.__inputBuffer = queue.Queue()

    def parse(self, byte):
        if byte == self.TelegramStartId:
            if self.state != self.ParserState.WaitingForStart:
                self.noEndReceived()
            self.state = self.ParserState.WaitingForTelegramId
