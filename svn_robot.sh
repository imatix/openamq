#! /bin/sh
#
#   Monitor projects and rebuild constantly
#   Run in openamq directory
#
while true; do
    cd ../base2
    if [ `svn update | wc -l` -gt 1 ]; then
        echo "Base2 changed - rebuilding"
        boom all
        rm ../openamq/readme.txt        #   Force rebuild of OpenAMQ
    fi
    cd ../openamq
    if [ `svn update | wc -l` -gt 1 ]; then
        echo "OpenAMQ changed - rebuilding"
        boom all
    fi
    sleep 5m
done