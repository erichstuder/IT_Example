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

# testlist
# - test constructor

import app.ComportHandler as ComportHandler


def test_setPort(mocker):
    comportHandler = ComportHandler.ComportHandler()
    comport = "COM666"
    comportHandler.setPort(comport)
    assert comportHandler._ComportHandler__serialPort.port == comport


def test_setBaudrate(mocker):
    comportHandler = ComportHandler.ComportHandler()
    baudrate = 9874
    comportHandler.setBaudrate(baudrate)
    assert comportHandler._ComportHandler__serialPort.baudrate == baudrate


# TODO: more tests
"""def test_write(mocker):
    serial_mock = mocker.patch("app.ComportHandler.serial.Serial")

    mockManager = mocker.Mock()
    mockManager.attach_mock(serial_mock, "serial_mock")

    comportHandler = ComportHandler.ComportHandler()
    baudrate = 9874
    comportHandler.write()

    expectedCallOrder = [
        mocker.call.serial_mock(None),
    ]
    assert mockManager.mock_calls == expectedCallOrder

    assert comportHandler._ComportHandler__serialPort.baudrate == baudrate"""
