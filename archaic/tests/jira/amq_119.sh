#!/bin/sh
#
#  Helper script to run lots of instances of amq_nop at once
#

for i in `yes | head -1000`; do
    ./amq_119 &
done

wait

