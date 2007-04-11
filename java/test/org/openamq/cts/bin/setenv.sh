# ---------------------------------------------------------------------------
# Sample environment script for JMS CTS
#
# This is invoked by jmscts.sh to configure:
# . the CLASSPATH, for JMS provider jars
# . JVM options
#
# The following configures the JMS CTS for OpenJMS 0.7.6
# ---------------------------------------------------------------------------

# Configure the CLASSPATH
#
DISTDIR="$IBASE/openamqj/dist"
LIBDIR="$IBASE/openamqj/lib"

CLASSPATH="$LIBDIR/jakarta-commons/commons-collections-3.1.jar:$LIBDIR/util-concurrent/backport-util-concurrent.jar:$LIBDIR/mina/mina-0.7.3.jar:$LIBDIR/jms/jms.jar:$LIBDIR/logging-log4j/log4j-1.2.9.jar:$DISTDIR/openamqj-common.jar:$DISTDIR/openamqj-jms.jar"

# Configure JVM options
#
JAVA_OPTS=-Xmx512m -Xms512m
JAVA_OPTS="$JAVA_OPTS \
           -Damqj.logging.level=WARN"
