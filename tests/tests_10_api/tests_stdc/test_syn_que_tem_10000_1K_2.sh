#!/bin/sh

repeats=4
clients="../../../clients"

export PATH=$PATH:$clients:$PATH:$clients/stdc
echo "The test will be repeated $repeats time(s)."
time amqpcli_serial -m 10000 -x 1024 -r $repeats
