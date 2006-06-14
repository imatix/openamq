#!/bin/sh

repeats=1
clients="../../../clients"

export PATH=$PATH:$clients:$PATH:$clients/stdc
echo "The test will be repeated $repeats time(s)."
time amqpcli_serial -p -m 1 -x 1073741824 -r $repeats
