#!/bin/sh

jclients="../../../clients/java"

java -cp $jclients:$jclients/OpenAMQJava.jar EchoClient -c consumer1 -i -r "c-in" -w "java-in" &
consumer1=$!
java -cp $jclients:$jclients/OpenAMQJava.jar EchoClient -c consumer2 -i -r "c-in" -w "java-in" &
consumer2=$!
sleep 2
java -cp $jclients:$jclients/OpenAMQJava.jar EchoClient -c producer -o -d 1
status=$?
kill $consumer1
kill $consumer2
exit $status
