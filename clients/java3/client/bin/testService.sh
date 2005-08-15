#!/bin/bash

# sets up CP env var
. ./setupclasspath.sh
echo "Classpath: $CP"
$JAVA_HOME/bin/java -cp $CP org.openamq.requestreply1.TestService 192.168.55.63 7654 piemuncher1 x x
