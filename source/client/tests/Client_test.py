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

import Client as Client
import time
# from unittest.mock import Mock
# from unittest.mock import patch
# import os
# from datetime import datetime

"""
test list:
- senden an com port

"""


def test_inputFromKeyboard(mocker):
    textFromInput = 'This is a test. \n'
    keyboardInput_mocked = mocker.patch('builtins.input', return_value=textFromInput)
    client = Client.Client()
    time.sleep(0.1)  # give enough time to read from keyboard
    keyboardInput_mocked.assert_called()
    a = client.getKeyboardInputQueue().get()
    assert client.getKeyboardInputQueue().get() == textFromInput


