@echo off

regsvr32 /u /s amq.dll
gsl   amq.gsl
rem msdev amq.dsw /make
regsvr32 /s amq.dll

