#!/bin/bash

# sets up CP env var
. ./setupclasspath.sh
CP=$CP:../lib/sonic/sonic_Client.jar
CP=$CP:../lib/activemq/activeio-core-3.0-beta1.jar
CP=$CP:../lib/activemq/activemq-core-4.0-RC2.jar
CP=$CP:../lib/activemq/geronimo-j2ee-management_1.0_spec-1.0.jar

$JAVA_HOME/bin/java -server -Xmx512m -Xms512m -XX:NewSize=150m -cp $CP -Damqj.logging.level="INFO" org.openamq.topic.Publisher $*
