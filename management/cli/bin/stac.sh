#!/bin/sh

COREROOT=../../core
AMQROOT=../../../clients_java

CP=../lib/jython/jython.jar
CP=$CP:../dist/openamq-stac.jar
CP=$CP:$COREROOT/dist/openamq-management-common.jar
CP=$CP:$COREROOT/lib/log4j/log4j-1.2.9.jar
CP=$CP:$COREROOT/lib/xmlbeans/jsr173_api.jar
CP=$CP:$COREROOT/lib/xmlbeans/resolver.jar
CP=$CP:$COREROOT/lib/xmlbeans/xbean.jar
CP=$CP:$COREROOT/lib/xmlbeans/xbean_xpath.jar
CP=$CP:$COREROOT/lib/xmlbeans/xmlpublic.jar
CP=$CP:$COREROOT/lib/xmlbeans/saxon8.jar
CP=$CP:$AMQROOT/dist/openamqj-common.jar
CP=$CP:$AMQROOT/dist/openamqj-jms.jar
CP=$CP:$AMQROOT/lib/mina/mina-0.7.3.jar
CP=$CP:$AMQROOT/lib/jms/jms.jar
CP=$CP:$AMQROOT/lib/util-concurrent/backport-util-concurrent.jar
CP=$CP:$AMQROOT/lib/jakarta-commons/commons-collections-3.1.jar

$JAVA_HOME/bin/java -Damqj.logging.level="ERROR" -cp $CP org.openamq.stac.Stac