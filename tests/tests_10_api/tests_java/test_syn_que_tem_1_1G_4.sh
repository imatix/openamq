#!/bin/sh

repeats=1
jclients="../../../clients/java"

echo "The test will be repeated $repeats time(s)."
time java -Xmx256M -cp $jclients:$jclients/OpenAMQJava.jar amqpcli_serial -m 1 -x 1G -r $repeats
