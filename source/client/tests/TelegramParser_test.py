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
- parse function
- error callback wenn mittendrin 0xAA
- entfernen von 0xCC
- übergabe von nicht byte grossem wert muss zu fehler führen
- soll nur unsigned char akzeptiert werden? also keine negativen werte?
"""


def test_parseEmptyTelegramWithoutEnd(mocker):
    noEndReceived = mock.Mock()
    telegramParser = TelegramParser(noEndReceived)
    telegramParser.parse(0xAA)
    noEndReceived.assert_not_called()
    telegramParser.parse(0xAA)
    noEndReceived.assert_called_once()

    telegramParser.parse(0xAA)
    assert noEndReceived.call_count == 2
