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

from app.TextFileViewer import TextFileViewer
import time


class TestTextFileViewer:
    def test_emptyFileName(self):
        try:
            viewer = TextFileViewer('')
            assert False  # An exception should come before we come here
        except FileNotFoundError:
            pass
        except:
            assert False

    def test_writeToFile(self, mocker):
        insert_mocked = mocker.patch('tkinter.Text.insert')
        delete_mocked = mocker.patch('tkinter.Text.delete')
        configure_mocked = mocker.patch('tkinter.Text.configure')
        tkinter_mocked = mocker.patch('tkinter.Tk')
        FileName = 'myFile.dat'
        with open(FileName, 'w+') as file:
            file.write('test\n')
        TextFileViewer(FileName)
        time.sleep(2)
        tkinter_mocked.assert_called()
        assert configure_mocked.call_count == 3
        calls = [mocker.call(background='black', foreground='white', state='disabled'),
                 mocker.call(state='normal'),
                 mocker.call(state='disabled')]
        configure_mocked.assert_has_calls(calls, any_order=False)
        delete_mocked.assert_called_once_with('1.0', 'end')
        insert_mocked.assert_called_once_with('end', 'test\n')
