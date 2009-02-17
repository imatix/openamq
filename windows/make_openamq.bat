@echo off
echo Version numbers must be correct in source code.
echo Updating from SVN, rebuilding from scratch
set VERSION=%1
set PASSWORD=%2
if .%VERSION%==. (
    echo Syntax: "make_openamq version password", e.g. "make_openamq 1.4a4 Secret"
    exit /b 1
)

:- Update all projects from SVN
cd ..\..
svn update foreign base1 base2 openamq
:- reconfigure and regenerate all projects
cd foreign
call boom configure regen
cd ..\base1
call boom configure regen
cd ..\base2
call boom configure regen
cd ..\openamq
call boom configure regen

cd windows
:- Build debug packages into %IBASE_HOME%\debug
msbuild OpenAMQ.sln /nologo /t:Clean /v:quiet
msbuild OpenAMQ.sln /nologo /t:Rebuild /p:Configuration=Debug /v:quiet

:- Build release packages into %IBASE_HOME%\release
msbuild OpenAMQ.sln /nologo /t:Clean /v:quiet
msbuild OpenAMQ.sln /nologo /t:Rebuild /p:Configuration=Release /v:quiet

rm -rf *.zip
echo Building packages...
set TARGET=OpenAMQ-%VERSION%-debug
mkdir %TARGET%
mkdir %TARGET%\webpages
cp %IBASE_HOME%\debug\bin\amq_server.exe %TARGET%
cp %IBASE_HOME%\debug\bin\amq_client.exe %TARGET%
cp %IBASE_HOME%\debug\bin\amq_shell.exe  %TARGET%
cp %IBASE_HOME%\debug\bin\zyre.exe       %TARGET%
cp ..\zyre\webpages\index.html 	         %TARGET%\webpages
cp ..\zyre\webpages\http_base.css        %TARGET%\webpages
zip -rmq OpenAMQ-%VERSION%-debug.zip %TARGET%

set TARGET=OpenAMQ-%VERSION%-release
mkdir %TARGET%
mkdir %TARGET%\webpages
cp %IBASE_HOME%\release\bin\amq_server.exe %TARGET%
cp %IBASE_HOME%\release\bin\amq_client.exe %TARGET%
cp %IBASE_HOME%\release\bin\amq_shell.exe  %TARGET%
cp %IBASE_HOME%\release\bin\zyre.exe       %TARGET%
cp ..\zyre\webpages\index.html 	           %TARGET%\webpages
cp ..\zyre\webpages\http_base.css          %TARGET%\webpages
zip -rmq OpenAMQ-%VERSION%-release.zip %TARGET%

set TARGET=ibase-%VERSION%-debug
mkdir %TARGET%
cp -R %IBASE_HOME%\debug\lib %IBASE_HOME%\debug\include %TARGET%
zip -rmq ibase-%VERSION%-debug.zip %TARGET%

if not %PASSWORD%.==. (
    pscp -pw %PASSWORD% -i \work\.ssh\ph_priv.ppk *.zip ph@svpublic.imatix.com:/web/xiadmin/vhosts/download.imatix.com/openamq/unstable/
    echo * [http://download.imatix.com/openamq/unstable/OpenAMQ-%VERSION%-debug.zip OpenAMQ-%VERSION%-debug.zip] - OpenAMQ and Zyre [debug build]
    echo * [http://download.imatix.com/openamq/unstable/OpenAMQ-%VERSION%-release.zip OpenAMQ-%VERSION%-release.zip] - OpenAMQ and Zyre [release build]
    echo * [http://download.imatix.com/openamq/unstable/ibase-%VERSION%-debug.zip ibase-%VERSION%-debug.zip] - WireAPI libraries [debug build]
)