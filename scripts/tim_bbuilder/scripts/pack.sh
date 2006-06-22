#!/bin/sh
BBUILDROOT=$1
REV=$2

if [ "x$2" = "x" ]; then
    echo "usage $0 bbuildroot rev"
    exit -2;
fi

rm -rf /tmp/OpenAMQ-bin-$REV
mkdir -p /tmp/OpenAMQ-bin-$REV
cat $BBUILDROOT/scripts/install.sh | sed -e "s/@@REV@@/$REV/" > /tmp/OpenAMQ-bin-$REV/install.sh
chmod +x /tmp/OpenAMQ-bin-$REV/install.sh
cp $BBUILDROOT/packages/OpenAMQ-bin-*-$REV.tar.gz /tmp/OpenAMQ-bin-$REV/
cd /tmp
tar zcv OpenAMQ-bin-$REV -f $BBUILDROOT/collections/OpenAMQ-bin-$REV.tar.gz
rm -rf /tmp/OpenAMQ-bin-$REV
