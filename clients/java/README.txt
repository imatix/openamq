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

    javac com/imatix/openamq/framing/AMQFramingFactory.java
    javac amqpcli_serial.java
    javac EchoClient.java

Running the test client
=======================

1. Simple pattern checking serial client

    java amqpcli_serial -h

2. Echo (chat) client

    java EchoClient -h

    in separate windows, run the openamq/clients/stdc/chat_in and chat_out
    clients.



