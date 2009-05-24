@echo off
set PROJECT=%1
set TARGET=%2
cd \work\trunk\%PROJECT%
call boom regen
cd \work\trunk\openamq\windows
if not .%TARGET%==. msbuild OpenAMQ.sln /nologo /t:%TARGET% /p:Configuration=%CONFIG% /v:normal
cd \work\trunk\%PROJECT%
call boom install
cd \work\trunk\openamq\windows
