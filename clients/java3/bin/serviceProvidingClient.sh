#!/bin/bash

# sets up CP env var
. ./setupclasspath.sh
echo "Classpath: $CP"
$JAVA_HOME/bin/java -server -Damqj.logging.level=INFO -cp $CP org.openamq.requestreply1.ServiceProvidingClient s2012.samhs.uk.jpmorganchase.com 7654 guest guest /test serviceQ1
