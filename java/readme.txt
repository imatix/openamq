In order to build Blaze you need Ant 1.6.5.

Build in order:

* common
* broker
* client

Running the Broker
------------------

To run the broker, run the broker/bin/amq_server.sh script. The only argument you need to provide is a path to a config file,
and the one supplied in the etc directory probably doesn't need modification.

So for example:

./amq_server.sh -c ../etc/config.xml

You can get a list of all command line arguments by using the -h argument.

In particular, -p overrides the port specified in the config file and -b overrides the bind interface (default is wildcard).

Running Tests
-------------

Shell scripts to run tests are in the client/bin directory.

The simplest test to ensure everything is working is the "service request reply" test. This involves one client that is known
as a "service provider" and it listens on a well-known queue for requests. Another client, known as the "service requester"
creates a private (temporary) response queue, creates a message with the private response queue set as the "reply to" field
and then publishes the message to the well known service queue. The test allows you to time how long it takes to send
messages and receive the response back. It also allows varying of the message size.

You must start the service provider first:

./serviceProvidingClient.sh nop host:port

where host:port is the host and port you are running the broker on. The first argument, if set to "profile" will enable profiling
with JFluid (Netbeans profiler).

To run the service requester:

./serviceRequestingClient.sh nop host:post <messagecount> <message size in bytes>

After receiving all the messages the client outputs the rate it achieved.

A more realistic test is the "headers test", which tests the performance of routing messages based on message headers to
a configurable number of clients (e.g. 50). A publisher sends 10000 messages to each client and waits to receive a message
from each client when it has received all the messages.

You run the listener processes first:

./run_many.sh 10 headersListener "headersListener.sh -host 10.0.0.1 -port 5672"

In this command, the first argument means start 10 processes, the second is just a name use in the log files generated and the
third argument is the command to run. In this case it runs another shell script but it could be anything. The arguments
to the shell script are hopefully self explanatory.

Then run the publisher process:

./headersPublisher.sh -host 10.0.0.1 -port 5672 10000 10

The last two arguments are: the number of messages to send to each client, and the number of clients.

Note that before starting the publisher you should wait about 30 seconds to ensure all the clients are registered with the
broker (you can see this from the broker output). Otherwise the numbers will be slightly skewed.

A third useful test, which can easily be ported to other JMS implementations is the "topic test". It does the same as the
headers test but using a standard topic (i.e. pub sub).

To run the listeners:

./run_many.sh 10 topictester "./topicListener.sh -host 10.0.0.1 -port 5672"

(run_many.sh is just a simple script that runs multiple instances of the specified command)

and to run the publisher:

./topicPublisher.sh -host 10.0.0.1 -port 5672 -clients 10 -messages 10000
