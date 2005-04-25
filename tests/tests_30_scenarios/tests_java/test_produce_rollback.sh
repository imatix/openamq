#!/bin/sh

repeats=2
jclients="../../../clients/java"

echo "The test will be repeated $repeats time(s)."
java -cp $jclients:$jclients/OpenAMQJava.jar amqpcli_transactions -a 1 -r $repeats
