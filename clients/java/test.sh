#!/bin/sh

distjms=$IBASE/openamqj/dist
distgrm=$IBASE/grmj/dist
lib=$IBASE/openamqj/lib
grmlib=$IBASE/grmj/lib

libs="$lib/commons-collections/commons-collections-3.1.jar:$lib/util-concurrent/backport-util-concurrent.jar:$lib/mina/mina-core-0.9.5-SNAPSHOT.jar:$lib/jms/jms.jar:$lib/logging-log4j/log4j-1.2.13.jar:$lib/slf4j/slf4j-simple.jar:lib/spring.jar:lib/commons-logging.jar"

$JAVA_HOME/bin/java -classpath "$libs:." org.openamq.Test
