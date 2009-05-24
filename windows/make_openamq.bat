@echo off
echo Version numbers must be correct in source code.
echo Updating from SVN, rebuilding from scratch
set VERSION=%1
set PASSWORD=%2
set QUICK=%3
if .%VERSION%==. (
    echo Syntax: "make_openamq version password [-quick]", e.g. "make_openamq 1.4a4 Secret -quick"
    exit /b 1
)

goto continue

if %QUICK%.==. (
    :- Update all projects from SVN
    echo Updating projects from SVN...
    cd \work\trunk\
    svn --quiet update foreign base1 base2 openamq

    :- reconfigure all projects
    cd \work\trunk\foreign
    call boom configure
    cd \work\trunk\base1
    call boom configure
    cd \work\trunk\base2
    call boom configure
    cd \work\trunk\openamq
    call boom configure
)
:- Build debug packages into %IBASE_HOME%\debug
echo Preparing for debug build...
call c:\work\debug
cd \work\trunk\openamq\windows
msbuild OpenAMQ.sln /nologo /t:Clean /v:quiet
set CONFIG=Debug

call build_target foreign         libzip
call build_target base2\xnf
call build_target base2\icl       libicl
call build_target base2\ipr       libipr
call build_target base2\smt       libsmt
call build_target base2\opf       libopf
call build_target base2\asl       libasl
call build_target base2\http      libhttp
call build_target base2\xitami    xitami
call build_target openamq\common  libamq_common
call build_target openamq\server  amq_server
call build_target openamq\wireapi amq_client
call build_target openamq\shell   amq_shell
call build_target openamq\zyre    zyre

:continue
call c:\work\release
cd \work\trunk\openamq\windows
msbuild OpenAMQ.sln /nologo /t:Clean /v:quiet
set CONFIG=Release

call build_target foreign         libzip
call build_target base2\xnf
call build_target base2\icl       libicl
call build_target base2\ipr       libipr
call build_target base2\smt       libsmt
call build_target base2\opf       libopf
call build_target base2\asl       libasl
call build_target base2\http      libhttp
call build_target base2\xitami    xitami
call build_target openamq\common  libamq_common
call build_target openamq\server  amq_server
call build_target openamq\wireapi amq_client
call build_target openamq\shell   amq_shell
call build_target openamq\zyre    zyre

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