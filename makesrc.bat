@echo off
:-
:-  Make full source package
:-
set VERSION=0.6h4
set PACKAGE=openamq-%VERSION%-src.zip
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

echo Preparing openamq/rfc...
cd %HOME%\products\amq\rfc
rm -f *.zip
call boom
call boom distsrc

echo Preparing openamq/core...
cd %HOME%\products\amq\openamq\core
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

mkdir amq_rfc
cd    amq_rfc
unzip -q %HOME%/products/amq/rfc/*.zip
cd ..

mkdir openamq
cd    openamq
unzip -q %HOME%/products/amq/openamq/core/*.zip
cd ..

echo #  We build / install these 3 products... > build.sh
echo cd base;    sh build.sh; cd ..           >> build.sh
echo cd base2;   sh build.sh; cd ..           >> build.sh
echo cd openamq; sh boomake build; cd ..      >> build.sh
echo #  To run the openamq server: amqpsrv    >> build.sh
copy ..\BUILDING .

cd ..
echo Building OpenAMQ distribution...
zip -rq %PACKAGE% openamq-%VERSION%

echo cd www/pub/amq>response
echo bin>>response
echo put %PACKAGE%>>response
echo quit>>response
:- ftp imatix.net < response
rm response
echo Uploaded to: http://imatix.net/pub/amq/%PACKAGE%


:exit
