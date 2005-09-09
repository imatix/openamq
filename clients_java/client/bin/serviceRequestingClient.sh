#!/bin/bash

# sets up CP env var
. ./setupclasspath.sh

thehost=$1
shift
echo $thehost
$JAVA_HOME/bin/java -cp $CP -Damqj.logging.level="INFO" org.openamq.requestreply1.ServiceRequestingClient $thehost 7654 guest guest /test serviceQ "$@"
