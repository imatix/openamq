#!/bin/bash

# sets up CP env var
. ./setupWLclasspath.sh
echo "Classpath: $CP"
$JAVA_HOME/bin/java -server -cp $CP org.openamq.weblogic.ServiceProvider t3://s2012.samhs.uk.jpmorganchase.com:7001 snowthQueue
