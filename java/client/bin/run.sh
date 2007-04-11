#!/bin/bash
# sets up CP env var
. ./setupclasspath.sh

#need the broker for some tests, but don't want to confuse existing tests...
CP=$CP:../../broker/dist/openamqdj.jar

$JAVA_HOME/bin/java -server -cp $CP -Damqj.logging.level="INFO" $*
