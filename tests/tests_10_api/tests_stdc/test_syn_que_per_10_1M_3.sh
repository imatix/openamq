#!/bin/sh

repeats=2
clients="../../../clients"

export PATH=$PATH:$clients:$PATH:$clients/stdc
echo "The test will be repeated $repeats time(s)."
time amqpcli_serial -p -m 10 -x 1048576 -r $repeats
