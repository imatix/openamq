@echo off
:-
:-  boom.bat - Boom batch file
:-
:-  Copyright (c) 1996-2009 iMatix Corporation
:-
:-  This program is free software; you can redistribute it and/or modify
:-  it under the terms of the GNU General Public License as published by
:-  the Free Software Foundation; either version 2 of the License, or (at
:-  your option) any later version.
:-
:-  This program is distributed in the hope that it will be useful, but
:-  WITHOUT ANY WARRANTY; without even the implied warranty of
:-  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
:-  General Public License for more details.
:-
:-  For information on alternative licensing for OEMs, please contact
:-  iMatix Corporation.
:-

:-  Start CMD.EXE version detection
verify other 2>nul
setlocal enableextensions
if errorlevel 0 goto __cmd_ok
echo %0: This command requires command extensions version 2 in CMD.EXE.  
echo %0: Please use a supported system (Windows 2000 or newer).
exit
:__cmd_ok
setlocal enabledelayedexpansion
:-  End CMD.EXE version detection

setlocal
if %1.==. goto empty

:switches
    set OPTION=%1
    if not "!OPTION:~0,1!"=="-"      goto command
    if "!OPTION!"=="--help"          goto help
    if "!OPTION!"=="-h"              goto help
    if "!OPTION!"=="--verbose"       goto verbose
    if "!OPTION!"=="-v"              goto verbose
    if "!OPTION!"=="--version"       goto version
    if "!OPTION!"=="-V"              goto version
    goto bad_option

:empty
    echo boom I: use -h or --help to see available options
    exit /b 1

:help
    echo 'boom' prepares and executes the build process.
    echo.
    echo Usage: boom [OPTION]... [COMMAND]...
    echo.
    echo Configuration:
    echo   -h, --help              Display this help information
    echo   -v, --verbose           Trace build commands
    echo   -V, --version           Display version information
    echo.
    echo Commands:
    echo   configure               Configure project (generates boomake.bat)
    echo   build                   Build project
    echo   test                    Test project
    echo   compile                 Build project w/o regenerating source
    echo   install                 Install project
    echo   clean                   Clean project
    echo   distsrc                 Create project source package
    echo   all                     clean, configure, build, install
    echo   help                    Show list of allowed commands
    echo.
    exit /b 0

:verbose
    set BOOM_TRACE=1
    shift
    goto switches

:version
    gsl3 -quiet -showver boom
    exit /b 0

:bad_option
    echo boom E: invalid option '!OPTION!' - use --help to see available options
    exit /b 1

:-   Now process all commands
:command
    set DO_CONFIG=no
    set DO_ALL=no

    if "%1"=="all" (
        if not exist stamp_source set DO_CONFIG=yes
        if exist boomake.bat call boomake clean
        set DO_ALL=yes
        shift
    )
    if "%1"=="configure" (
        if not exist stamp_source set DO_CONFIG=yes
        shift
    )
    if !DO_CONFIG!==yes (
        gsl3 -quiet >nul 2>nul
        if errorlevel 1 (
            echo "boom E: gsl3 was not found on the PATH, please install it"
            exit /b 1
        )
        :-  If we are using Subversion, pass SVN_REVISION to Boom
        set SVN_REVISION=
        if exist .svn\nul (
            for /f "usebackq tokens=1,2 delims=: " %%v in (`svn info`) do if "%%v"=="Revision" set SVN_REVISION=%%w
        )
        if !SVN_REVISION!.==. (
            gsl3 -quiet boom
        ) else (
            gsl3 -quiet -svn_revision:!SVN_REVISION! boom
        )
        if errorlevel 1 exit /b 1
        if exist Makefile.win32 (
            copy /y Makefile.win32 Makefile >nul 
        )
    )
    if !DO_ALL!==yes (
        call boomake build test install
        if errorlevel 1 exit /b 1
        exit /b 0
    )
    if not "%1"=="" (
        if exist boomake.bat (
            call boomake %1 %2 %3 %4 %5 %6 %7 %8 %9
        ) else (
            echo boom E: invalid option '%1' - use --help to see available options
            exit /b 1
        )
    )
