Readme for openamq/clients/java
*******************************

Package summary
=================

The com.imatix.openamq.framing package provides a base level communications
layer for AMQP/Fast applications coded in Java, client or server. The package
provides classes for network serialization/deserialization of connection, 
channel and handle protocol commands. A factory class glues all this command
classes together, providing object instantiation from the network; this utility 
class enforces negotiated protocol parameters. Message body input and output is 
possible via filtered streams. The provided streams can read from/write to
memory available to the application, while simple fragmentation/defragmentation
is handled transparently. Out-of-band messages will be possible in future 
versions, by passing of additional dedicated input/output streams. The package
supports loading/storing of field tables through a provided class which is 
similar in spirit to the Java Properties class. Methods for reading and writing
unsigned values up to 4 bytes long are available. Since Java does not provide 
primitive unsigned types, then AMQP integer values must be stored in memory 
into types of greater capacity than the protocol specifies; in Java spirit, 
moving those values from memory to network is runtime-checked for 
overflows/underflows. 

The framing API is just a low-level communications layer, it does not implement
client or server protocol dialogues. Specifically, the provided framing API can
be used to:
- Write unsigned values during connection initiation
- Send and receive command frames  
- Send message bodies as data streams
- Read and write field tables
- Dump to console the data traffic
- Build higher level APIs

Running the test clients
========================

1. Simple pattern checking serial client

    java -cp OpenAMQJava.jar:. amqpcli_serial
    java -Xmx256M -cp OpenAMQJava.jar:. amqpcli_serial -x 1073741824 -m 1

    The second call sends a 1GB message through the server (SUN's JRE).

2. Echo (chat) client

    java -cp OpenAMQJava.jar:. EchoClient

    in separate windows, run the openamq/clients/stdc/chat_in and chat_out
    clients.



