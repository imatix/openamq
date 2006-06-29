#!/bin/sh

lib=$IBASE/lib/java

libs="$lib/mina-core-0.9.5-SNAPSHOT.jar:$lib/log4j-1.2.13.jar:bin"

$JAVA_HOME/bin/java -classpath "$libs:." org.openamq.Test
