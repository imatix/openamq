#!/bin/sh

# args: 
# <number of processes to start>
# <name of run>
# <command ro run>

for i in `seq 1 $1`; do
    $3 >$2.$i.out 2>>$2.err &
    echo $! > $2.$i.pid
done;
