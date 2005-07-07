#!/bin/bash

# sets up CP env var
. ./setupWLclasspath.sh

$JAVA_HOME/bin/java -server  -Damqj.logging.level=INFO -cp $CP org.openamq.weblogic.ServiceRequestingClient t3://s2012.samhs.uk.jpmorganchase.com:7001 snowthQueue "$@"
