@echo off
set VERSION=0.7b4
set PACKAGE=openamq-%VERSION%-win32
set HOME=c:\home

if exist %PACKAGE% rmdir/q/s %PACKAGE%
mkdir %PACKAGE%

xcopy /s/q/i %HOME%\openamq\clients %PACKAGE%\clients
xcopy /s/q/i %HOME%\openamq\servers %PACKAGE%\servers

zip -rqm %PACKAGE%.zip %PACKAGE%

echo %PACKAGE%.zip
:exit
