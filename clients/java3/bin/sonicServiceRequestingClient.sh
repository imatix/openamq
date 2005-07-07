#!/bin/bash

# sets up CP env var
. ./setupSonicclasspath.sh

$JAVA_HOME/bin/java -server  -Damqj.logging.level=INFO -cp $CP org.openamq.sonic.ServiceRequestingClient s2012.samhs.uk.jpmorganchase.com:2606 serviceQ "$@"
