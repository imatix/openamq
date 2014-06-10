@echo off
:-
:-  Generates, builds, and runs a PAL script
:-
set _FILE=%1
set _PAL=%_FILE:.pal=%.pal
set _SRC=%_FILE:.pal=%.c
if not exist %_PAL% goto err_nf
gsl -q -quiet:1 %_PAL%
call c -l %_SRC%
goto exit

:err_nf
echo %_PAL% not found
:exit
