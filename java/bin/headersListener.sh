#!/bin/bash
#  Launcher script for OpenAMQ Java test program

#  Find Java and required JAR files
if [ -n "$JAVA_HOME" ]; then
    JAVA=$JAVA_HOME/bin/java
else
    JAVA=java
fi
if [ -n "$IBASE" -a -f "$IBASE/java/lib/openamq-java-launch.jar" ]; then
    OPENAMQ_JAVA_HOME=$IBASE/java/lib
fi
if [ -f "../dist/openamq-java-launch.jar" ]; then
    OPENAMQ_JAVA_HOME=../dist
fi
if [ -f "./dist/openamq-java-launch.jar" ]; then
    OPENAMQ_JAVA_HOME=./dist
fi
if [ -n "$OPENAMQ_JAVA_HOME/openamq-java-launch.jar" ]; then
    echo "Cannot locate openamq-java-launch.jar.  Please set either \$IBASE to"
    echo "point to the IBASE where you installed the OpenAMQ Java Client, or"
    echo "set \$OPENAMQ_JAVA_HOME to point to the top-level directory of the"
    echo "OpenAMQ Java Client distribution."
    exit 1
fi

#  Execute the script
exec $JAVA -cp $OPENAMQ_JAVA_HOME/openamq-java-launch.jar \
      -Damqj.logging.level="INFO" \
      org.openamq.headers.Listener $*
