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

import tkinter as tk
import threading
import os


class TextFileViewer:
    __root = None
    __textWidget = None
    __filePath = None

    def __worker(self):
        self.__root = tk.Tk()
        self.__root.title(self.__filePath)
        # root.iconbitmap('dummy.ico')
        self.__textWidget = tk.Text(self.__root, height=20, width=80)
        self.__textWidget.configure(background='black', foreground='white', state='disabled')
        self.__textWidget.pack()
        self.__updater()
        self.__root.mainloop()

    def __updater(self):
        with open(self.__filePath, 'r') as file:
            self.__textWidget.configure(state='normal')
            self.__textWidget.delete('1.0', tk.END)
            self.__textWidget.insert(tk.END, file.read())
            self.__textWidget.configure(state='disabled')
        self.__root.after(1000, self.__updater)

    def __init__(self, filePath):
        if not os.path.exists(filePath):
            raise FileNotFoundError
        self.__filePath = filePath
        threading.Thread(target=self.__worker).start()
