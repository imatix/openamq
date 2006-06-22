#!/bin/sh
REV=@@REV@@

if [ -d /opt ]; then
    INSTALLDIR=/opt/OpenAMQ-r$REV
elif [ -d /usr/local ]; then
    INSTALLDIR=/usr/local/OpenAMQ-r$REV
else 
    INSTALLDIR=~/OpenAMQ-r$REV
fi

echo
echo
echo "This script will install OpenAMQ rev $REV"
echo "please confirm the install directory, or enter a new one"
echo
echo -n "[$INSTALLDIR] (press enter for this default) "
read DIR

if [ ! "x$DIR" = "x" ]; then
    INSTALLDIR=$DIR
fi

echo
echo "About to install into '$INSTALLDIR' ..."
echo "Press enter to contine or ctrl-c to abort..."
read
echo "Please choose which flavor of OpenAMQ you would like to install"
echo
echo "    1. mt,release (multithreaded production server)"
echo "    2. st,release (singlethreaded production server)"
echo "    3. mt,debug   (multithreaded with debug information)"
echo "    4. st,debug   (singlethreaded with debug information)"
echo
echo -n "[1,2,3,4] "
read FLAVOR

if [ "x$FLAVOR" = "x1" ]; then
    PACKAGE="OpenAMQ-bin-mt-release-$REV.tar.gz"
elif [ "x$FLAVOR" = "x2" ]; then
    PACKAGE="OpenAMQ-bin-st-release-$REV.tar.gz"
elif [ "x$FLAVOR" = "x3" ]; then
    PACKAGE="OpenAMQ-bin-mt-debug-$REV.tar.gz"
elif [ "x$FLAVOR" = "x4" ]; then
    PACKAGE="OpenAMQ-bin-st-debug-$REV.tar.gz"
fi

if [ ! -f $PACKAGE ]; then
    echo "something went wrong, package '$PACKAGE' not found..."
    exit -1;
fi

mkdir -p $INSTALLDIR/bin || (echo "error creating $INSTALLDIR/bin" && exit -1)
mkdir -p $INSTALLDIR/etc || (echo "error creating $INSTALLDIR/etc" && exit -1)
mkdir -p $INSTALLDIR/lib || (echo "error creating $INSTALLDIR/lib" && exit -1)

tar zxf $PACKAGE
cp -f ${PACKAGE%%.tar.gz}/bin/amq_server $INSTALLDIR/bin/ && echo "amq_server installed in $INSTALLDIR/bin"
cp -f ${PACKAGE%%.tar.gz}/bin/amq_client $INSTALLDIR/bin/ && echo "amq_client installed in $INSTALLDIR/bin"
cp -f ${PACKAGE%%.tar.gz}/lib/libopenamq.a $INSTALLDIR/lib/ && echo "libopenamq.a installed in $INSTALLDIR/lib"
cp -f ${PACKAGE%%.tar.gz}/etc/amq_server_base.cfg $INSTALLDIR/etc/ && echo "amq_server_base.cfg installed in $INSTALLDIR/etc"

if [ ! -f $INSTALLDIR/etc/amq_server.cfg ]; then
	echo
	echo "The installer will ask passwords for the default user accounts"
	echo
	echo "Enter password for following users (changes can be made later)"

	stty -echo
	echo -n "guest: "
	read GUESTPASS
	echo
	echo -n "super: "
	read SUPERPASS
	echo
	echo -n "cluster: "
	read CLUSTERPASS
	echo
	echo -n "console: "
	read CONSOLEPASS
	echo
	stty echo
	echo "<?xml?>" 										>  $INSTALLDIR/etc/amq_server.cfg
	echo "<config>"								 		>> $INSTALLDIR/etc/amq_server.cfg
	echo "\t<user name = \"guest\"   password = \"$GUESTPASS\"   group = \"normal" />\"	>> $INSTALLDIR/etc/amq_server.cfg
	echo "\t<user name = \"super\"   password = \"$SUPERPASS\"   group = \"super" />\"	>> $INSTALLDIR/etc/amq_server.cfg
	echo "\t<user name = \"cluster\" password = \"$CLUSTERPASS\" group = \"cluster" />\"	>> $INSTALLDIR/etc/amq_server.cfg
	echo "\t<user name = \"console\" password = \"$CONSOLEPASS\" group = \"console" />\"	>> $INSTALLDIR/etc/amq_server.cfg
	echo "</config>" 									>> $INSTALLDIR/etc/amq_server.cfg
fi
echo

echo "+------------------------------------------------------"
echo "| OpenAMQ r$REV installed in $INSTALLDIR"
echo "+------------------------------------------------------"
echo "| you could change your PATH environment to"
echo "|"
echo "|    PATH=$INSTALLDIR/bin:\$PATH"
echo "|"
echo "| so you can start the server like this:"
echo "|"
echo "|    \$ amq_server"
echo "+------------------------------------------------------"
echo 
echo 
