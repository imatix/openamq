Readme for openamq/clients/java
*******************************

Package summary
=================

The com.imatix.openamq package provides an implementation of the AMQP/Fast API
for Java applications. The package provides classes for serializing and 
deserializing connection, channel and handle protocol commands. A factory class
provides object instantiation from the network. This utility class enforces 
negotiated protocol parameters. Message input and output is possible via 
filtered streams. The provided streams can read from and write to memory 
buffers available to the application, while simple message defragmentation and 
fragmentation is handled transparently. Out-of-band messages are possible by 
passing of additional dedicated input/output streams. The package supports 
field table encoding and decoding through a class which is similar in spirit
to the Java Properties class. Methods for reading and writing unsigned values
up to 4 bytes long are available. Since Java does not provide primitive 
unsigned types, then AMQP integer values must be stored in memory into types of
greater capacity than the protocol specifies; in Java spirit, moving those
values from memory to network is runtime-checked for overflows/underflows. Long
strings are stored as byte arrays.

The Java API is currently a low-level communications layer, it does not implement
client or server protocol dialogues. Specifically, the provided API can be used
to:
- Send messages as data streams
- Send and receive command frames  
- Read and write field tables
- Read and write primitive protocol types
- Dump to console the data traffic
- Build higher level (client/server) APIs

Running the test clients
========================

1. JMS Hello World client

    Produce through JMS interface:
        java -cp OpenAMQJava.jar:jms.jar:. JMSHelloWorld -H /test -D test -s <server>
    Consume with plain AMQP interface:    
        java -cp OpenAMQJava.jar:. EchoClient -s localhost -r test -s <server>  

1. Simple pattern checking serial client

    This call sends a series of small messages through the server
        java -cp OpenAMQJava.jar:. amqpcli_serial -s <server>
    This call sends a 1GB message through the server (SUN's JRE).
        java -Xmx256M -cp OpenAMQJava.jar:. amqpcli_serial -x 1G -m 1 -s <server>

2. Echo (chat) client

    Create peer 1:
        java -cp OpenAMQJava.jar:. EchoClient -s <server>
    Create peer 2:
        java -cp OpenAMQJava.jar:. EchoClient -c client2 -r "c-in" -w "java-in" -s <server>
    
