Readme for openamq/clients/java
*******************************

Building
========

1. Generate the framing code 
ln -s ../../core/amq_frames.xml
gsl amq_frames.xml

2. Generate the serial Java client from Libero specifications 
lr amqpcli_serial.java.l

3. Compile
javac com/imatix/openamq/framing/AMQFactory.java
javac amqpcli_serial.java

Running the test client
=======================

java amqpcli_serial
