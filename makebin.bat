@echo on
set VERSION=0.8d4
set PACKAGE=openamq-%VERSION%-win32
set HOME=c:\home

echo Packaging version %VERSION%...
pause
if exist %PACKAGE% rmdir/q/s %PACKAGE%
mkdir %PACKAGE%

xcopy /s/q/i %HOME%\openamq\clients %PACKAGE%\clients 
xcopy /s/q/i %HOME%\openamq\servers %PACKAGE%\servers 

zip -rqm %PACKAGE%.zip %PACKAGE%

echo %PACKAGE%.zip
if exist upload.bat call upload %PACKAGE%.zip

:exit
