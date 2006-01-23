#!/bin/bash

# sets up CP env var
. ./setupclasspath.sh

thehosts=$1
shift
echo $thehosts
$JAVA_HOME/bin/java -cp $CP -Damqj.logging.level="INFO" org.openamq.requestreply1.ServiceRequestingClient $thehosts guest guest /test serviceQ "$@"
