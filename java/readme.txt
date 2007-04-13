OpenAMQ/JMS
Copyright 2007 iMatix Corporation
For copying conditions, please see the file license.txt included 
with this distribution.

Building OpenAMQ/JMS
--------------------

In order to build OpenAMQ/JMS you need:

  * Sun J2SE 5.0 JDK, which can be downloaded from http://java.sun.com/
  * Apache Ant 1.7.0 or newer, which can be downloaded from
    http://ant.apache.org/

To build, please ensure that Ant and your JDK are installed correctly,
and that the JAVA_HOME environment variable, if set, is pointing to the
root directory of the JDK that you wish to build OpenAMQ/JMS with.

Then, run the following:

> ant

This will build OpenAMQ/JMS and test classes, and leave the JAR files in
the dist directory of the distribution.

To install the JAR files into an OpenAMQ IBASE, you can run

> ant install

which will install them into $IBASE/java/lib, if the IBASE environment
variable is set.

For other useful targets, try:

> ant -projecthelp

Running Example Programs
------------------------

Scripts to run the examples are in the bin directory.

The simplest test to ensure everything is working is the "service
request reply" test. This involves one client that is known as a
"service provider" and it listens on a well-known queue for requests.

Another client, known as the "service requester" creates a private
(temporary) response queue, creates a message with the private response
queue set as the "reply to" field and then publishes the message to the
well known service queue. The test allows you to time how long it takes
to send messages and receive the response back. It also allows varying
of the message size.

You must start the service provider first:

On UNIX:

> bin/serviceProvidingClient.sh host:port

On Windows:

> bin\serviceProvidingClient host:port

where host:port is the host and port you are running the broker on.

To run the service requester:

On UNIX:

> bin/serviceRequestingClient.sh host:post <message count> <message size in bytes>

On Windows:

> bin\serviceRequestingClient host:post <message count> <message size in bytes>

After receiving all the messages the client outputs the rate it achieved.

A more realistic example is the "headers test", which tests the
performance of routing messages based on message headers to a
configurable number of clients (e.g. 50). A publisher sends e.g. 10000
messages to each client and waits to receive a message from each client
when it has received all the messages.

You run the listener processes first (repeat for as many listeners as
you like):

> bin/headersListener.sh -host 127.0.0.1 -port 5672

Then run the publisher process:

> bin/headersPublisher.sh -host 10.0.0.1 -port 5672 <message count> <number of listeners>

Note that before starting the publisher you should wait a few seconds to
ensure all the clients are registered with the broker.

