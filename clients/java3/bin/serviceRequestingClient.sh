#!/bin/bash

# sets up CP env var
. ./setupclasspath.sh

$JAVA_HOME/bin/java -server -Damqj.logging.level=INFO -cp $CP org.openamq.requestreply1.ServiceRequestingClient s2012.samhs.uk.jpmorganchase.com 7654 guest guest /test serviceQ1 "$@"
