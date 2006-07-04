#!/bin/sh

lib=$IBASE/amqpj/lib
dist=$IBASE/amqpj/dist

libs="$lib/mina-core-0.9.5-SNAPSHOT.jar:$lib/log4j-1.2.13.jar:$dist/amqpj.jar"

$JAVA_HOME/bin/java -classpath "$libs:." org.openamq.Test
