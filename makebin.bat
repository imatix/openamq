@echo off
set VERSION=0.7b0
set PACKAGE=openamq-%VERSION%-win32
set HOME=c:\home

if exist %PACKAGE% rmdir/q/s %PACKAGE%
mkdir %PACKAGE%

xcopy /s/q/i %HOME%\openamq-client %PACKAGE%\openamq-client
xcopy /s/q/i %HOME%\openamq-server %PACKAGE%\openamq-server

zip -rqm %PACKAGE%.zip %PACKAGE%

echo %PACKAGE%.zip
:exit
