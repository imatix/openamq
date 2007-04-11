#!/bin/bash

# sets up CP env var
. ./setupclasspath.sh

$JAVA_HOME/bin/java -cp $CP -Damqj.logging.level="INFO" org.openamq.headers.Listener $*
