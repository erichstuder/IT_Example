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

import Client
import builtins


def test_constructor(mocker):
    comportHandler_mock = mocker.patch("Client.ComportHandler")

    mockManager = mocker.Mock()
    mockManager.attach_mock(comportHandler_mock, "comportHandler_mock")

    client = Client.Client()

    expectedCallOrder = [
        mocker.call.comportHandler_mock(),
    ]
    assert mockManager.mock_calls == expectedCallOrder


def test_setComport(mocker):
    comport = "COM23"
    input_mock = mocker.patch("builtins.input", side_effect=["set comport " + comport + "\n", "exit\n"])
    print_mock = mocker.patch("builtins.print")
    comportHandler_setPort_mock = mocker.patch("Client.ComportHandler.setPort")

    mockManager = mocker.Mock()
    mockManager.attach_mock(input_mock, "input_mock")
    mockManager.attach_mock(print_mock, "print_mock")
    mockManager.attach_mock(comportHandler_setPort_mock, "comportHandler_setPort_mock")

    client = Client.Client()
    client.run()

    expectedCallOrder = [
        mocker.call.input_mock(),
        mocker.call.comportHandler_setPort_mock(port=comport),
        mocker.call.print_mock("  comport set to: " + comport),
        mocker.call.input_mock(),
        mocker.call.print_mock("  goodbye..."),
    ]
    assert mockManager.mock_calls == expectedCallOrder


def test_setBaudrate(mocker):
    baudrate = "1234"
    input_mock = mocker.patch("builtins.input", side_effect=["set baudrate " + baudrate + "\n", "exit\n"])
    print_mock = mocker.patch("builtins.print")
    comportHandler_setBaudrate_mock = mocker.patch("Client.ComportHandler.setBaudrate")

    mockManager = mocker.Mock()
    mockManager.attach_mock(input_mock, "input_mock")
    mockManager.attach_mock(print_mock, "print_mock")
    mockManager.attach_mock(comportHandler_setBaudrate_mock, "comportHandler_setBaudrate_mock")

    client = Client.Client()
    client.run()

    expectedCallOrder = [
        mocker.call.input_mock(),
        mocker.call.comportHandler_setBaudrate_mock(baudrate=baudrate),
        mocker.call.print_mock("  baudrate set to: " + baudrate),
        mocker.call.input_mock(),
        mocker.call.print_mock("  goodbye..."),
    ]
    assert mockManager.mock_calls == expectedCallOrder


def test_runNonexistentFile(mocker):
    input_mock = mocker.patch("builtins.input", side_effect=["run myTextNonExistent.txt\n", "exit\n"])
    print_mock = mocker.patch("builtins.print")

    mockManager = mocker.Mock()
    mockManager.attach_mock(input_mock, "input_mock")
    mockManager.attach_mock(print_mock, "print_mock")

    client = Client.Client()
    client.run()

    expectedCallOrder = [
        mocker.call.input_mock(),
        mocker.call.print_mock("  error: file not found"),
        mocker.call.input_mock(),
        mocker.call.print_mock("  goodbye..."),
    ]
    assert mockManager.mock_calls == expectedCallOrder


def test_runFile(mocker, tmpdir):
    textFile = "myText.txt"
    with open(textFile, "w") as file:
        file.write("exit\n")

    input_mock = mocker.patch("builtins.input", side_effect=["run " + textFile + "\n"])
    print_mock = mocker.patch("builtins.print")

    mockManager = mocker.Mock()
    mockManager.attach_mock(input_mock, "input_mock")
    mockManager.attach_mock(print_mock, "print_mock")

    client = Client.Client()
    client.run()

    expectedCallOrder = [
        mocker.call.input_mock(),
        mocker.call.print_mock("  running: myText.txt"),
        mocker.call.print_mock("  goodbye..."),
    ]
    assert mockManager.mock_calls == expectedCallOrder


def test_exit(mocker):
    input_mock = mocker.patch("builtins.input", side_effect=["exit\n"])
    print_mock = mocker.patch("builtins.print")

    mockManager = mocker.Mock()
    mockManager.attach_mock(input_mock, "input_mock")
    mockManager.attach_mock(print_mock, "print_mock")

    client = Client.Client()
    client.run()

    expectedCallOrder = [
        mocker.call.input_mock(),
        mocker.call.print_mock("  goodbye..."),
    ]
    assert mockManager.mock_calls == expectedCallOrder


def test_sendToServer(mocker):
    cmdToServer = "blabla"
    input_mock = mocker.patch("builtins.input", side_effect=[cmdToServer + "\n", "exit\n"])
    print_mock = mocker.patch("builtins.print")
    comportHandler_write_mock = mocker.patch("Client.ComportHandler.write")

    mockManager = mocker.Mock()
    mockManager.attach_mock(input_mock, "input_mock")
    mockManager.attach_mock(print_mock, "print_mock")
    mockManager.attach_mock(comportHandler_write_mock, "comportHandler_write_mock")

    client = Client.Client()
    client.run()

    expectedCallOrder = [
        mocker.call.input_mock(),
        mocker.call.comportHandler_write_mock(cmdToServer + "\r"),
        mocker.call.print_mock("  sent to server: " + cmdToServer),
        mocker.call.input_mock(),
        mocker.call.print_mock("  goodbye..."),
    ]
    assert mockManager.mock_calls == expectedCallOrder


def test_nothingToReadFromServer(mocker):
    mocker.patch("builtins.input", return_value="exit\n")
    mocker.patch("Client.ComportHandler.read", return_value=None)
    mocker.patch("builtins.open")

    client = Client.Client()
    client.run()

    Client.ComportHandler.read.assert_called_with()
    builtins.open.assert_not_called()


def test_readFromServer(mocker):
    mocker.patch("builtins.input", return_value="exit\n")
    mocker.patch("Client.ComportHandler.read", side_effect=[42, None])
    mocker.patch("builtins.open")

    client = Client.Client()
    client.run()

    Client.ComportHandler.read.assert_called_with()
    builtins.open.assert_called_with("mySession.session", "a+b")
