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

1. Simple pattern checking serial client

    java -cp OpenAMQJava.jar:. amqpcli_serial
    java -Xmx256M -cp OpenAMQJava.jar:. amqpcli_serial -x 1G -m 1

    The second call sends a 1GB message through the server (SUN's JRE).

2. Echo (chat) client

    java -cp OpenAMQJava.jar:. EchoClient

    In separate windows, run the openamq/clients/stdc/chat_in and chat_out
    clients.



