@echo off
if %1.==. goto exit
:next
    if exist %1.icl goto icl
    if exist %1.smt goto smt
    if exist %1.xml goto xml
    if exist %1.c   goto c
    goto exit

:icl
    gsl %1.icl
    call c %1
    call c -r libamq %1
    call c -l %1_test
    call c -L amqpsrv
    goto done

:smt
    gsl %1.smt
    call c %1
    call c -r libamq %1
    call c -L amqpsrv
    goto done

:xml
    gsl %1.xml
    call c %1
    call c -r libamq %1
    goto done

:c
    call c %1
    call c -r libamq %1
    shift
    goto done

:done
    shift
    goto next
    
:exit
