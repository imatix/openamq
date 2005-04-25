#!/bin/sh

repeats=2
clients="../../../clients"

export PATH=$PATH:$clients:$PATH:$clients/agnostic
echo "The test will be repeated $repeats time(s)."
time amqpcli_serial -m 100 -x 1048576 -i 0 -r $repeats
