@echo off
:-  Launcher script for OpenAMQ/JMS test program

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

:-  Find Java and required JAR files
set _LAUNCHER_JAR=openamq-jms-launch.jar
if defined JAVA_HOME (
    set _JAVA=!JAVA_HOME!\bin\java
) else (
    set _JAVA=java
)
set _OPENAMQ_JAVA_HOME=
if exist !IBASE!\java\lib\!_LAUNCHER_JAR! (
    set _OPENAMQ_JAVA_HOME=!IBASE!\java\lib
) else if exist ..\dist\!_LAUNCHER_JAR! (
    set _OPENAMQ_JAVA_HOME=..\dist
) else if exist .\dist\!_LAUNCHER_JAR! (
    set _OPENAMQ_JAVA_HOME=.\dist
)
if not exist !_OPENAMQ_JAVA_HOME!\!_LAUNCHER_JAR! (
    echo Could not locate !_LAUNCHER_JAR! in any of:
    echo %%IBASE%%\java\lib\!_LAUNCHER_JAR!
    echo ..\dist\!_LAUNCHER_JAR!
    echo .\dist\!_LAUNCHER_JAR!
    echo Please either set %%IBASE%% to point to the IBASE where you installed
    echo OpenAMQ/JMS, or run this script from the top-level directory of the
    echo OpenAMQ/JMS distribution.
    exit /b 1
)

:-  Execute the test
!_JAVA! -cp !_OPENAMQ_JAVA_HOME!/!_LAUNCHER_JAR! -Damqj.logging.level="INFO" org.openamq.headers.Listener %*
