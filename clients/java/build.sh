#
#   Build Java clients (provisional build script)
#
gsl amq_frames.xml
lr amqpcli_serial.java.l
javac com/imatix/openamq/framing/AMQFramingFactory.java
javac amqpcli_serial.java
javac EchoClient.java
