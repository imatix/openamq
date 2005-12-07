#!/bin/sh

JMSCTS_PATH=/home/guso/harness/jmscts-0.5-b2

distjms="$IBASE/openamqj/dist"
lib="$IBASE/openamqj/lib"
lib2="$JMSCTS_PATH/lib/"
libs="$lib/jakarta-commons/commons-collections-3.1.jar:$lib/util-concurrent/backport-util-concurrent.jar:$lib/mina/mina-0.7.3.jar:$lib/jms/jms.jar:$lib/logging-log4j/log4j-1.2.9.jar:$distjms/openamqj-common.jar:$distjms/openamqj-jms.jar"
libs2="$lib2/ant-1.5.3-1.jar:$lib2/junit-3.8.1.jar:$lib2/ant-optional-1.5.3-1.jar:$lib2/log4j-1.2.7.jar:$lib2/castor-0.9.5.jar:$lib2/openjms-provider-0.5-b2.jar:$lib2/commons-cli-1.0.jar:$lib2/oro-2.0.7.jar:$lib2/commons-collections-2.1.jar:$lib2/xalan-2.5.1.jar:$lib2/commons-logging-1.0.2.jar:$lib2/xdoclet-1.2b2.jar:$lib2/concurrent-1.3.2.jar:$lib2/xdoclet-xdoclet-module-1.2b2.jar:$lib2/exolabcore-0.3.7.jar:$lib2/xdoclet-xjavadoc-uc-1.2b2.jar:$lib2/jms-1.0.2a.jar:$lib2/xerces-2.3.0.jar:$lib2/jmscts-0.5-b2.jar:$lib2/xml-apis-1.0.b2.jar"

javac -classpath $libs:$libs2 $JMSCTS_PATH/src/providers/openamqj/org/exolab/jmscts/openamqj/*.java
cd $JMSCTS_PATH/src/providers/openamqj
jar cvf openamqj-provider-0.0a1.jar org/exolab/jmscts/openamqj/*.class
mv openamqj-provider-0.0a1.jar $lib2
