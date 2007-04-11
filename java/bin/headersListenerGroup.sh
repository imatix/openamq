#!/bin/sh

for i; do
    ./headersListener.sh -host 10.0.0.1 -port 5672 >$i.out 2>$i.err &
    echo $! > $i.pid
done;
