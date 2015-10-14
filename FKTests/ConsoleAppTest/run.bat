@echo off
setlocal enabledelayedexpansion
set string=
FOR /F "tokens=1" %%I IN (testList.txt) DO (
	set string=!string! %%I
)
"../../tests/bin/ConsoleAppTest.exe" -o "result.txt" !string!