@echo off
if %1.==. goto exit
:next
    if exist %1.icl goto icl
    if exist %1.smt goto smt
    if exist %1.xml goto xml
    if exist %1.c   goto c
    goto exit

:icl
    for %%a in (%1*.icl) do gsl -q %%a
    for %%a in (%1*.c)   do call c -q %%a
    for %%a in (%1*.obj) do call c -q -r libamq %%a
    call c -L %1_test
    call c -L amqpsrv
    goto done

:smt
    gsl -q %1.smt
    call c -q %1
    call c -q -r libamq %1 
    call c -L amqpsrv
    goto done

:xml
    gsl -q %1.xml
    call c -q %1
    call c -q -r libamq %1
    goto done

:c
    call c -q %1
    call c -q -r libamq %1
    shift
    goto done

:done
    shift
    goto next
    
:exit
