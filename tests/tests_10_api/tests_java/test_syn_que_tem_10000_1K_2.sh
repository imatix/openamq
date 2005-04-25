#!/bin/sh

repeats=4
jclients="../../../clients/java"

echo "The test will be repeated $repeats time(s)."
time java -cp $jclients:$jclients/OpenAMQJava.jar amqpcli_serial -m 10000 -x 1K -r $repeats
