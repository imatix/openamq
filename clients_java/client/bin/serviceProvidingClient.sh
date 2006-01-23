#!/bin/bash

# sets up CP env var
. ./setupclasspath.sh

$JAVA_HOME/bin/java -cp $CP -Damqj.logging.level="INFO" org.openamq.requestreply1.ServiceProvidingClient $1 guest guest /test serviceQ 
