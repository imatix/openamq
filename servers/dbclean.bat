@echo off
cd vh_test
del /q/s data spool store >nul
cd ..

cd vh_default
del /q/s data spool store >nul
cd ..

