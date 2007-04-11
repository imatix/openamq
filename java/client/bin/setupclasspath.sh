COMMONLIBDIR=../../common/lib
LIBDIR=../lib
DISTDIR=../dist

CP=$LIBDIR/jms/jms.jar
CP=$CP:$COMMONLIBDIR/commons-collections/commons-collections-3.1.jar
CP=$CP:$COMMONLIBDIR/slf4j/slf4j-simple.jar
CP=$CP:$COMMONLIBDIR/logging-log4j/log4j-1.2.13.jar
CP=$CP:$COMMONLIBDIR/mina/mina-core-0.9.5-SNAPSHOT.jar
CP=$CP:$COMMONLIBDIR/mina/mina-filter-ssl-0.9.5-SNAPSHOT.jar
CP=$CP:$COMMONLIBDIR/commons-configuration/commons-configuration-1.2.jar
CP=$CP:$COMMONLIBDIR/commons-cli/commons-cli-1.0.jar
CP=$CP:$COMMONLIBDIR/commons-lang/commons-lang-2.1.jar
CP=$CP:$COMMONLIBDIR/commons-logging/commons-logging-api.jar
CP=$CP:$COMMONLIBDIR/commons-logging/commons-logging.jar
CP=$CP:$COMMONLIBDIR/junit/junit-4.0.jar
CP=$CP:$LIBDIR/util-concurrent/backport-util-concurrent.jar
CP=$CP:$DISTDIR/openamqj-common.jar
CP=$CP:$DISTDIR/openamqj-jms.jar
CP=$CP:$DISTDIR/openamqj-tests.jar

PROFILER_ARGS=
if [[ -n $PROFILE && $PROFILE == "profile" ]]; then
    PROFILER_SERVER=$HOME/blaze/profiler-server
    PROFILER_ARGS=-agentpath:$PROFILER_SERVER/lib/deployed/jdk15/linux/libprofilerinterface.so=$PROFILER_SERVER/lib,5140
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/pyramid/u675605/blaze/jprofiler4/bin/linux-x86
#    PROFILER_ARGS="-Xjvmpi:entryexit=off -Xrunjprofiler:port=8849 -Xbootclasspath/a:/home/pyramid/u675605/blaze/jprofiler4/bin/agent.jar"
fi
