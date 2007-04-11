#!/bin/bash
PROFILER_ARGS=
PROFILE=$1
shift
# sets up CP env var
. ./setupclasspath.sh

$JAVA_HOME/bin/java -server -Xmx1024m -Xms1024m -XX:NewSize=300m -cp $CP $PROFILER_ARGS -Damqj.logging.level="INFO" org.openamq.requestreply1.ServiceProvidingClient $1 guest guest /test serviceQ 
