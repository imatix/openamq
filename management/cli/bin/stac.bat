@echo off
set COREROOT=..\..\core
set AMQROOT=..\..\..\clients_java

set CP=..\lib\jython\jython.jar
set CP=%CP%;..\intellijclasses
set CP=%CP%;%COREROOT%\intellijclasses
set CP=%CP%;%COREROOT%\classes
set CP=%CP%;%COREROOT%\lib\log4j\log4j-1.2.9.jar
set CP=%CP%;%COREROOT%\lib\xmlbeans\jsr173_api.jar
set CP=%CP%;%COREROOT%\lib\xmlbeans\resolver.jar
set CP=%CP%;%COREROOT%\lib\xmlbeans\xbean.jar
set CP=%CP%;%COREROOT%\lib\xmlbeans\xbean_xpath.jar
set CP=%CP%;%COREROOT%\lib\xmlbeans\xmlpublic.jar
set CP=%CP%;%COREROOT%\lib\xmlbeans\saxon8.jar
set CP=%CP%;%AMQROOT%\dist\openamqj-common.jar
set CP=%CP%;%AMQROOT%\dist\openamqj-jms.jar
set CP=%CP%;%AMQROOT%\lib\mina\mina-0.7.3.jar
set CP=%CP%;%AMQROOT%\lib\jms\jms.jar
set CP=%CP%;%AMQROOT%\lib\util-concurrent\backport-util-concurrent.jar
set CP=%CP%;%AMQROOT%\lib\jakarta-commons\commons-collections-3.1.jar


@rem %JAVA_HOME%\bin\java -Damqj.logging.level="ERROR" -Xdebug -Xnoagent -Djava.compiler=NONE -Xrunjdwp:transport=dt_socket,server=y,suspend=n,address=5005 -cp %CP% org.openamq.stac.Stac
%JAVA_HOME%\bin\java -Damqj.logging.level="ERROR" -cp %CP% org.openamq.stac.Stac