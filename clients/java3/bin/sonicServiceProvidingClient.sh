#!/bin/bash

# sets up CP env var
. ./setupSonicclasspath.sh
echo "Classpath: $CP"
$JAVA_HOME/bin/java -server -Xmx256m -cp $CP org.openamq.sonic.ServiceProvidingClient s2012.samhs.uk.jpmorganchase.com:2606 serviceQ
