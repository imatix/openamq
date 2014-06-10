@echo off
:-
:-  boomvs.bat - invoke MS Visual Studio (6 or 7)
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
set BINDIR=%~d0%~p0

:switches
    set OPTION=%1
    if not "!OPTION:~0,1!"=="-"      goto command
    if "!OPTION!"=="--help"          goto help
    if "!OPTION!"=="-h"              goto help
    if "!OPTION!"=="--version"       goto version
    if "!OPTION!"=="-V"              goto version
    if "!OPTION!"=="--where"         goto where
    if "!OPTION!"=="--using"         goto using
    if "!OPTION!"=="--target"        goto target
    goto bad_option

:empty
    echo boomvs I: use -h or --help to see available options
    goto exit_err

:help
    echo 'boomvs' invokes Visual Studio on a dsw file
    echo.
    echo Usage: boomvs [OPTION]... COMMAND
    echo.
    echo Configuration:
    echo   -h, --help              Display this help information
    echo   -V, --version           Display version information
    echo.
    echo Options:
    echo   --where=name            Location of project file
    echo   --using=name            Name of project workspace (no extension)
    echo   --target=name           Name of target to build (VS6 project)
    echo.
    echo Commands:
    echo   build                   Build project
    echo   clean                   Clean project
    echo.
    goto exit

:version
    echo boomvs: version 1.0
    goto exit

:where
    shift
    if not "%1"=="" (
        set WHERE=%1
        if not exist %1\nul (
            echo boomvs E: directory %1 does not exist
            goto exit_err
        )
    ) else (
        echo boomvs E: no argument provided for --where option
        goto exit_err
    )
    shift
    goto switches

:using
    shift
    if not "%1"=="" (
        set USING=%1
    ) else (
        echo boomvs E: no argument provided for --using option
        goto exit_err
    )
    shift
    goto switches

:target
    shift
    if not "%1"=="" (
        set TARGET=%1
    ) else (
        echo boomvs E: no argument provided for --target option
        goto exit_err
    )
    shift
    goto switches

:bad_option
    echo boomvs E: invalid option '!OPTION!' - use --help to see available options
    goto exit_err

:command
    set ACTION=%1
    if "!WHERE!"=="" (
        echo boomvs E: please specify --path option
        goto exit_err
    )
    if "!USING!"=="" (
        echo boomvs E: please specify --using option
        goto exit_err
    )
    if "!TARGET!"=="" (
        echo boomvs E: please specify --target option
        goto exit_err
    )
    pushd !WHERE!
    if defined VS71COMNTOOLS (
        set CONVERT=!BINDIR!convert71.js
        goto msvs71
    ) else if defined VS80COMNTOOLS (
        set CONVERT=!BINDIR!convert80.js
        goto msvs71
    )
:msvs6
    if "!ACTION!"=="clean" (
        set ACTION=/clean
    ) else (
        set ACTION=
    )
    if exist !USING!.dsw (
        set TYPE=dsw
    ) else (
        if exist !USING!.dsp (
            set TYPE=dsp
        ) else (
            echo boomvs E: cannot find !USING!.dsw or !USING!.dsp - aborting
            popd
            goto exit_err
        )
    )
    if "!BOOM_MODEL_RELEASE!" == "1" (
        msdev !USING!.!TYPE! /make "!TARGET! - Win32 Release" !ACTION!
        
        if errorlevel 1 (
            echo boomvs E: build FAILED
            goto exit_err
        )
    ) else (
        if "!BOOM_MODEL_DEBUG!" == "1" (
            msdev !USING!.!TYPE! /make "!TARGET! - Win32 Debug" !ACTION!
            if errorlevel 1 (
                echo boomvs E: build FAILED
                goto exit_err
            )
        ) else (
            echo boomvs E: BOOM_MODEL invalid or not set
            goto exit_err
        )
    )
    popd
    goto exit
    
:msvs71
    if not exist !CONVERT! (
        echo boomvs E: can't find !CONVERT! - aborting
        goto exit_err
    )
    if not exist !TARGET!.sln cscript !CONVERT! !TARGET!.dsp
    if errorlevel 1 (
        echo boomvs E: !CONVERT! failed - aborting
        goto exit_err
    )
    if "!BOOM_MODEL_RELEASE!" == "1" (
        devenv !TARGET!.sln /!ACTION! "Release"
        if errorlevel 1 (
            echo boomvs E: build FAILED
            goto exit_err
        )
    ) else (
        if "!BOOM_MODEL_DEBUG!" == "1" (
            devenv !TARGET!.sln /!ACTION! "Debug"
            if errorlevel 1 (
                echo boomvs E: build FAILED
                goto exit_err
            )
        ) else (
            echo boomvs E: BOOM_MODEL invalid or not set
            goto exit_err
        )
    )
    popd
    goto exit

:exit_err
exit /b 1

:exit
exit /b 0
