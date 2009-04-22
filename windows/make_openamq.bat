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
echo Updating projects from SVN...
cd \work\trunk\
svn --quiet update foreign base1 base2 openamq

:- reconfigure and regenerate all projects
cd \work\trunk\foreign
call boom configure regen
cd \work\trunk\base1
call boom configure regen
cd \work\trunk\base2
call boom configure regen
cd \work\trunk\openamq
call boom configure regen

:- Build debug packages into %IBASE_HOME%\debug
echo Preparing for debug build...
call c:\work\debug
cd \work\trunk\openamq\windows
msbuild OpenAMQ.sln /nologo /t:Clean /v:quiet

echo Building Foreign/debug...
msbuild OpenAMQ.sln /nologo /t:libzip /p:Configuration=Debug /v:quiet
cd \work\trunk\foreign
call boom install
cd \work\trunk\openamq\windows

echo Building Base2/debug...
msbuild OpenAMQ.sln /nologo /t:xitami /p:Configuration=Debug /v:quiet
cd \work\trunk\base2
call boom install
cd \work\trunk\openamq\windows

echo Building OpenAMQ/debug...
msbuild OpenAMQ.sln /nologo /t:zyre /p:Configuration=Debug /v:quiet
cd \work\trunk\openamq
call boom install
cd \work\trunk\openamq\windows

:release
:- Build release packages into %IBASE_HOME%\release
echo Preparing for release build...
call c:\work\release
cd \work\trunk\openamq\windows
msbuild OpenAMQ.sln /nologo /t:Clean /v:quiet

echo Building Foreign/release...
msbuild OpenAMQ.sln /nologo /t:libzip /p:Configuration=Release /v:quiet
cd \work\trunk\foreign
call boom install
cd \work\trunk\openamq\windows

echo Building Base2/release...
msbuild OpenAMQ.sln /nologo /t:xitami /p:Configuration=Release /v:quiet
cd \work\trunk\base2
call boom install
cd \work\trunk\openamq\windows

echo Building OpenAMQ/release...
msbuild OpenAMQ.sln /nologo /t:zyre /p:Configuration=Release /v:quiet
cd \work\trunk\openamq
call boom install
cd \work\trunk\openamq\windows

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