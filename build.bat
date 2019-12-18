:: IT - Internal Tracer
:: Copyright (C) 2019 Erich Studer
::
:: This program is free software: you can redistribute it and/or modify
:: it under the terms of the GNU Affero General Public License as published
:: by the Free Software Foundation, either version 3 of the License, or
:: (at your option) any later version.
::
:: This program is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
:: GNU Affero General Public License for more details.
::
:: You should have received a copy of the GNU Affero General Public License
:: along with this program.  If not, see <://www.gnu.org/licenses/>.

@echo off
echo.

echo Building library for pytest with MSBuild
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" "%~dp0\source\deviceWithServer\itLibrary\itLibrary.sln" /property:Configuration=Release >nul 2>&1
if errorlevel 1 (
	echo   MSBuild failed!!!
) else (
	echo   MSBuild success
)

echo.

echo Execute pytest
pytest "%~dp0\source\client\tests" >nul 2>&1
if errorlevel 1 (
	echo   pytest failed!!!
) else (
	echo   pytest success
)

echo.

echo Execute cpp unit tests
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" "%~dp0\source\deviceWithServer\deviceWithServerTest\deviceWithServerTest.sln" /property:Configuration=Debug >nul 2>&1
"%~dp0\source\deviceWithServer\deviceWithServerTest\x64\Debug\deviceWithServerTest.exe" >nul 2>&1
if errorlevel 1 (
	echo   unit tests failed!!!
) else (
	echo   unit tests success
)

echo.
pause
