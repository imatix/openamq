@echo off
:-
:-  Make full source package
:-
set VERSION=0.7b0
set PACKAGE=openamq-%VERSION%-src
set HOME=c:\imatix\work

echo Preparing base...
cd %HOME%\products\base
rm -f *.zip
call boom
call boom distsrc

echo Preparing base2...
cd %HOME%\products\base2
rm -f *.zip
call boom
call boom distsrc

echo Preparing openamq...
cd %HOME%\products\openamq
rm -f *.zip
call boom
call boom distsrc

rmdir/q/s openamq-%VERSION%
mkdir openamq-%VERSION%
cd    openamq-%VERSION%

mkdir base
cd    base
unzip -q %HOME%/products/base/*.zip
cd ..

mkdir base2
cd    base2
unzip -q %HOME%/products/base2/*.zip
cd ..

mkdir openamq
cd    openamq
unzip -q %HOME%/products/openamq/*.zip
cd ..

echo :-  We build / install these 3 products...> build.bat
echo cd base>>build.bat
echo build.bat>>build.bat
echo cd ..>>build.bat
echo cd base2>>build.bat
echo build.bat>>build.bat
echo cd ..>>build.bat
echo cd openamq>>build.bat
echo build.bat>>build.bat
echo cd ..>>build.bat
echo :-  To run the openamq server: amqpsrv    >> build.bat

cd ..
echo Building OpenAMQ distribution...
zip -rq %PACKAGE% openamq-%VERSION%

:exit
