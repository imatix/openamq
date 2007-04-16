+ FileMQ - File transfer over AMQP

++ General architecture

All routing is done outside AMQP.

* N publishers who send files to a cache server
* 1 cache server
* N consumers who get files from the cache server

API:

content->type is command

    -- Staging a file ------------------------------------------------

    P:STAGE
    S:STAGE-OK | EXCEPTION

    STAGE - send fragment of arbitrary size
        FILENAME=x                      //  File name
        FILESIZE=n                      //  Total size of file
        REVISED=n                       //  Date/time revised
        DIGEST=x                        //  SHA1 file digest
        OFFSET=n                        //  Offset of fragment
        + content body
        + reply-to

    STAGE-OK - fragment received, thanks
        FILENAME=x                      //  File name
        FILESIZE=n                      //  Total size of file
        STAGED=n                        //  Amount staged so far
        -- when STAGED=FILESIZE, file fully received

    EXCEPTION
        FILENAME=x                      //  File name
        TEXT=x                          //  Exception text

    -- Publishing a file ---------------------------------------------

    P:PUBLISH
    S:PUBLISH-OK | EXCEPTION

    PUBLISH - distribute a previously-staged file
        FILENAME=x                      //  File name
        FILESIZE=n                      //  Total size of file
        REVISED=n                       //  Date/time revised
        DIGEST=x                        //  SHA1 file digest
        CHANNEL=channel                 //  Name of channel
        + reply-to

    PUBLISH-OK - file successfully published
        FILENAME=x                      //  File name
        CHANNEL=channel                 //  Name of channel
        SUBSCRIBES=n                    //  Number of subscribers

    -- Subscribing to a channel --------------------------------------

    C:SUBSCRIBE
    S:SUBSCRIBE-OK | EXCEPTION

    SUBSCRIBE - subscribe to a channel
        CHANNEL=channel                 //  Name of channel
        + reply-to

    SUBSCRIBE-OK - confirm subscription
        CHANNEL=channel

    -- Delivering a file ---------------------------------------------

    S:STAGE
    C:STAGE-OK | EXCEPTION

    S:DELIVER
    C:DELIVER-OK | EXCEPTION

    DELIVER - deliver a previously-staged file
        FILENAME=x                      //  File name
        FILESIZE=n                      //  Total size of file
        REVISED=n                       //  Date/time revised
        DIGEST=x                        //  SHA1 file digest
        CHANNEL=channel                 //  Name of channel

    DELIVER-OK - file successfully delivered
        FILENAME=x                      //  File name
        CHANNEL=channel                 //  Name of channel
        SUBSCRIBES=n                    //  Number of subscribers

    -- Requesting a syncpoint ----------------------------------------

    C:SYNC
    S:SYNC-OK | EXCEPTION

    SYNC - set synchronisation point
        REVISED=n                       //  Date/time syncpoint
        CHANNEL=channel                 //  Name of channel
        + reply-to

    SYNC-OK - confirm sync point
        REVISED=n                       //  Date/time syncpoint
        CHANNEL=channel


File stream
    - many publishers, one cache, many subscribers
    - subscribers get files one by one
    - fmq_sub launches external commands to process files
    - subscribers can replay stream from by time
        - cache-to-subscriber session
    - all files are versioned on cache

shared folder
    - one publisher, one cache, many subscribers
    - subscribers get snapshot of folder
    - fmq_sub can launch external commands to process events
        - new files, deleted files
    - all files are versioned on cache


subscriber
    - create queue, bind to channel patterns, receive messages
    - deliver messages by running some process
    - or by placing messages into directory

cache
    - daemon, runs permanently
    - create queue, bind to #, receive messages
    - store messages in folder named by channel


++ fmq_cache


-> type = message name
-> reply_to for replies
-> expiration for files
-> message_id = filename if needed
-> timestamp = file timestamp


++ General approach

We will use the existing Basic class to transfer messages across an unreliable broker infrastructure, consisting of one or more brokers in a network.  We do not assume any reliabilty in the broker.

All file-transfer specific functionality will be handled by external applications, rather than the broker.  However this functionalty would eventually be moved into AMQP as a File Transfer service.

The general architecture of our design consists of:

# A file server application which stores and forwards files.
# One or more file publishers, who send files to the file server.
# One or more file consumers, who receive files from the file server.
# An AMQP broker that connects these in a network.

An application can be both publisher and consumer.

++ Addressing and routing

We use a simple addressing and routing model:

* A broker can handle multiple file servers, each with a different name.
* Publishers and consumers participate in "channel".  A channel is a named area, like an IRC channel.
* Publishers send their files to a particular channel.
* Consumers subscribe to channels, and receive all files on a channel, or files who's names match particular patterns (e.g. file extensions).

Addressing, routing, and quality-of-service are handled **outside** AMQP, in communications between the publishers, consumers, and file server.  Eventually the file transfer semantics will be incorporated into AMQP so that brokers can act as file servers (to improve interoperability and performance).

Channels are created by publishers and consumers on demand.

The routing implementation is as follows:

* Each file server uses an exclusive (auto-delete) named queue.  It is not possible for two file servers to share a queue.
* The named queue is registered with the "filemq" exchange (a direct exchange) using the queue name as the routing key.
* Client applications send their messages to the "filemq" exchange using the file server name as the routing key.

++ Quality of service

Consumers can specify various quality of service criteria:

* The maximum rate at which they which to receive incoming traffic from the file server.
* The maximum sizes of individual files that they will accept.
* The hours of the day during which they will accept incoming traffic.

++ File transfer protocol

The FT protocol (which is above AMQP) is an XML-formatted RPC protocol that implements this functionality:

* Authenticate publisher or consumer.
* Subscribe to channel, with specified criteria.
* Set quality of service for consumers.
* Request channel history.
* Request specific file.
* Send file fragment.

basic.publish
    routing-key = server-name
    message-id = filename
    header:channel = channel
    header:authdata = authentication

- channel = folder under server
- files are written to folder
- fragments are echoed back to broker
...

exchange = server name
channel = routing-key
amq.topic exchange
    - file server listens on #
publish to exchange
    - routing-key = channel name
subscribe to exchange
    - routing-key = channel name
request files from server
    - publish to server, channel name, message-id = what files





++ File server implementation

The file server is an AMQP application that has the following rough design:

* The application creates a private queue and a direct exchange called "file-server".
* It binds the private queue to the file-server exchange and consumes messages off this queue.
* It processes incoming requests.

The file server implements a simple failover mechanism; it will look for one of a list of brokers and connect to the first available one. If the broker dies, the file server pauses for a few seconds and then re-tries the list.  If it cannot connect to any broker on the list it exits, or continues to retry, depending on a runtime switch (the -r robust option).

