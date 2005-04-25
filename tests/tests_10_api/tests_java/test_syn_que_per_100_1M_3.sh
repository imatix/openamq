#!/bin/sh

repeats=2
jclients="../../../clients/java"

echo "The test will be repeated $repeats time(s)."
time java -cp $jclients:$jclients/OpenAMQJava.jar amqpcli_serial -p -m 100 -x 1M -r $repeats
