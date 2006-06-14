#!/bin/sh

repeats=2
jclients="../../../clients/java"

echo "The test will be repeated $repeats time(s)."
time java -cp $jclients:$jclients/OpenAMQJava.jar amqpcli_serial -m 10 -x 1M -r $repeats
