#!/bin/sh

repeats=4
clients="../../../clients"

export PATH=$PATH:$clients:$PATH:$clients/agnostic/stdc
echo "The test will be repeated $repeats time(s)."
time test_client producer -h /test -n $(($repeats * 10000)) -l 1024 -i 0
