#!/bin/bash

# sets up CP env var
. ./setupclasspath.sh

$JAVA_HOME/bin/java -cp $CP org.openamq.requestreply1.ServiceRequestingClient 192.168.55.63 7654 piemuncher1 "$@"
