@echo off
net stop "World Wide Web Publishing Service"
net stop "IIS Admin Service"

regsvr32 /u /s amq_client.dll
gsl   amq.gsl
msdev amq.dsw /make
regsvr32 /s amq_client.dll

net start "IIS Admin Service"
net start "World Wide Web Publishing Service"
warble

