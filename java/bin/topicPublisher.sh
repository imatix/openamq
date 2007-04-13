#!/bin/bash

#  Launcher script for OpenAMQ/JMS test program

#  Find Java and required JAR files
LAUNCHER_JAR=openamq-jms-launch.jar
if [ -n "$JAVA_HOME" ]; then
    JAVA=$JAVA_HOME/bin/java
else
    JAVA=java
fi
OPENAMQ_JAVA_HOME=
if [ -n "$IBASE" -a -f "$IBASE/java/lib/$LAUNCHER_JAR" ]; then
    OPENAMQ_JAVA_HOME=$IBASE/java/lib
fi
if [ -f "../dist/$LAUNCHER_JAR" ]; then
    OPENAMQ_JAVA_HOME=../dist
fi
if [ -f "./dist/$LAUNCHER_JAR" ]; then
    OPENAMQ_JAVA_HOME=./dist
fi
if [ ! -f "$OPENAMQ_JAVA_HOME/$LAUNCHER_JAR" ]; then
    cat <<EOM
Could not locate $LAUNCHER_JAR in any of:

\$IBASE/java/lib/$LAUNCHER_JAR
../dist/$LAUNCHER_JAR
./dist/$LAUNCHER_JAR

Please either set \$IBASE to point to the IBASE where you installed 
OpenAMQ/JMS, or run this script from the top-level directory of the
OpenAMQ/JMS distribution.
EOM
    exit 1
fi

#  Execute the test
exec $JAVA -cp $OPENAMQ_JAVA_HOME/openamq-jms-launch.jar \
      -Damqj.logging.level="INFO" \
      org.openamq.topic.Publisher $*
