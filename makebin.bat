@echo off
set VERSION=0.8a0
set PACKAGE=openamq-%VERSION%-win32
set HOME=c:\home

echo Packaging version %VERSION%...
pause
if exist %PACKAGE% rmdir/q/s %PACKAGE%
mkdir %PACKAGE%

xcopy /s/q/i %HOME%\openamq\clients %PACKAGE%\clients >nul
xcopy /s/q/i %HOME%\openamq\servers %PACKAGE%\servers >nul

zip -rqm %PACKAGE%.zip %PACKAGE%

echo %PACKAGE%.zip
:exit
