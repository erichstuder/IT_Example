echo off

echo Building library for pytest with MSBuild
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" "%~dp0\source\deviceWithServer\itLibrary\itLibrary.sln" /property:Configuration=Release >nul 2>&1
if errorlevel 1 (
	echo MSBuild failed!!!
) else (
	echo MSBuild success
)

echo.

echo Execute pytest
pytest "%~dp0\source\client\tests" >nul 2>&1
if errorlevel 1 (
	echo pytest failed!!!
) else (
	echo pytest success
)

