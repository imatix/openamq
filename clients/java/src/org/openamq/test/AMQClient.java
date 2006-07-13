package org.openamq.test;

import java.io.*;
import java.net.*;
import java.util.*;

import org.apache.log4j.Logger;

import org.openamq.framing.*;
import org.openamq.transport.*;
import org.openamq.protocol.*;
import org.openamq.*;

public class AMQClient extends AMQClientConnection {
    private static final Logger
        _logger = Logger.getLogger(AMQClient.class);

    static final String
        USAGE = 
            "Syntax: java AMQClient [options...]\n"                                   +
            "Options:\n"                                                              +
            "  -s server[:port] Name or address of server (localhost)\n"              +
            "  -n number        Number of messages to send/receive (1)\n"             +
            "  -x size          Size of each message (default = 1024)\n"              +
            "  -a connections   Open N active connections (1)\n"                      +
            "  -p connections   Open N passive connections (0)\n"                     +
            "  -r repeat        Repeat test N times (1)\n"                            +
            "  -t level         Set trace level (default = 0)\n"                      +
            "                   0=none, 1=info, 2=warn, 3=debug\n"                    +
            "  -v               Show version information\n"                           +
            "  -h               Show summary of command-line options\n"               +
            "\nThe order of parameters is not important. Switches and filenames\n"     +
            "are case sensitive. See documentation for detailed information.\n",

        COPYRIGHT =
            "Copyright (c) 2005-2006 iMatix Corporation",

        NO_WARRANTY =
            "This is free software; see the source for copying conditions.  There is NO\n" +
            "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",

        CLIENT_NAME = "AMQClient Java";

    // Test parameters
    int
        messages,
        active,
        passive,
        msgSize,
        repeats;

    // Test objects
    AMQClientConnection[]
        connections;
    AMQClientSession[]
        sessions;
    AMQMessage[]
        msgs;

    public AMQClient(String host, String virtualHost, AuthData authData, String instance,
        int trace, int timeout) throws IOException {

        setHost(host);
        setVirtualHost(virtualHost);
        setAuthData(authData);
        setClientInstance(instance);
        setTrace(trace);
        setTimeout(timeout);

        messages = 1;
        active = 1;
        passive = 0;
        msgSize = 1024;
        repeats = 1;
    }

    public void setMessages(int messages) {
        this.messages = messages;
    }

    public int getMessages() {
        return messages;
    }

    public void setActive(int active) {
        this.active = active;
    }

    public int getActive() {
        return active;
    }

    public void setPassive(int passive) {
        this.passive = passive;
    }

    public int getPassive() {
        return passive;
    }

    public void setMsgSize(int msgSize) {
        this.msgSize = msgSize;
    }

    public int getMsgSize() {
        return msgSize;
    }

    public void setRepeats(int repeats) {
        this.repeats = repeats;
    }

    public int getRepeats() {
        return repeats;
    }

    public static void main(String args[]) throws Exception {
        //  These are the parameters we may get on the command line
        final String
            OPT_SERVER = "server",
            OPT_MESSAGES = "messages",
            OPT_ACTIVE = "active",
            OPT_PASSIVE = "passive",
            OPT_TRACE = "trace",
            OPT_MSG_SIZE = "msgSize",
            OPT_REPEATS = "repeats";

        TestProperties
            parameters = new TestProperties();
        String
            argParam = null;
        boolean
            argsOk = true;

        AuthData
            auth;
        AMQClient
            client;

        for (int argn = 0; argn < args.length; argn++) {
            // If argParam is set, we have to collect an argument parameter 
            if (argParam != null) {
                if (!args[argn].startsWith("-")) {
                    parameters.setProperty(argParam, args[argn]);
                    argParam = null;
                } else {
                    // Parameter can't start with '-'
                    argsOk = false;
                    break;
                }
            } else if (args[argn].startsWith("-")) {
                switch (args[argn].charAt(1)) {
                    // These switches take a parameter
                    case 's':
                        argParam = OPT_SERVER;
                        break;
                    case 'n':
                        argParam = OPT_MESSAGES;
                        break;
                    case 'a':
                        argParam = OPT_ACTIVE;
                        break;
                    case 'p':
                        argParam = OPT_PASSIVE;
                        break;
                    case 't':
                        argParam = OPT_TRACE;
                        break;
                    case 'x':
                        argParam = OPT_MSG_SIZE;
                        break;
                    case 'r':
                        argParam = OPT_REPEATS;
                        break;
    
                    // These switches have an immediate effect
                    case 'v':
                        System.out.println(CLIENT_NAME + "\n");
                        System.out.println(COPYRIGHT);
                        System.out.println(NO_WARRANTY);
                        System.exit(0);
                    case 'h':
                        System.out.println(CLIENT_NAME + "\n");
                        System.out.println(COPYRIGHT);
                        System.out.println(NO_WARRANTY);
                        System.out.println(USAGE);
                        System.exit(0);
    
                    // Anything else is an error
                    default:
                        argsOk = false;
                }
            } else {
                argsOk = false;
                break;
            }
        }
        // If there was a missing parameter or an argument error, quit
        if (argParam != null) {
            _logger.error("argument missing - use -h for help");
            System.exit(1);
        } else if (!argsOk) {
            _logger.error("invalid parameters - use -h for help");
            System.exit(1);
        }

        // Test
        auth = AMQClientConnection.authPlain("guest", "guest");
        client = new AMQClient("localhost", "/", auth, CLIENT_NAME, 0, 30000);

        client.setHost(parameters.getProperty(OPT_SERVER, client.getHost()));
        client.setMessages(parameters.getIntProperty(OPT_MESSAGES, client.getMessages()));
        client.setActive(parameters.getIntProperty(OPT_ACTIVE, client.getActive()));
        client.setPassive(parameters.getIntProperty(OPT_PASSIVE, client.getPassive()));
        client.setTrace(parameters.getIntProperty(OPT_TRACE, client.getTrace()));
        client.setMsgSize(parameters.getIntProperty(OPT_MSG_SIZE, client.getMsgSize()));
        client.setRepeats(parameters.getIntProperty(OPT_REPEATS, client.getRepeats()));

        client.test();

        System.exit(0);
    }

    public void test() {
        try {
            byte[]
                body;
            int
                i, j, k;

            connections = new AMQClientConnection[active + passive];
            sessions = new AMQClientSession[connections.length];
            msgs = new AMQMessage[active];

            // Setup message body
            for (i = 0, body = new byte[msgSize]; i < msgSize; i++)
                body[i] = (byte)0xAB;

            // Setup connections
            for (i = 0; i < connections.length; i++) {
                connections[i] = new AMQClientConnection(getHost(), getVirtualHost(), getAuthData(), getClientInstance(), getTrace(), getTimeout());
                sessions[i] = connections[i].createSession();
                if (i < active) {
                    String
                        queue = "QUEUE" + i;
    
                    sessions[i].queueDeclare(queue);
                    sessions[i].basicConsume(queue);

                    msgs[i] = sessions[i].createMessage();
                    msgs[i].setBody(body);
                }
            }
            _logger.info("opened " + connections.length + " connection(s) to " + getHost());

            // Repeat test
            for (i = 0; i < repeats; i++) {
                _logger.info("(" + (i + 1) + ") sending " + (messages * active) + " message(s) to "  + getHost() );

                // Send messages
                for (j = 0; j < messages; j++) {
                    for (k = 0; k < active; k++) {
                        String
                            queue = "QUEUE" + k;

                        msgs[k].getHeaders().setMessageId("ID" + k + "-" + j);
                        sessions[k].basicPublish(msgs[k], "", queue);
                    }
                }

                // Receive messages and discard them
                for (j = 0; j < messages; j++) {
                    for (k = 0; k < active; k++) {
                        msgs[k] = sessions[k].getMessage();
                        if (_logger.isDebugEnabled()) {
                            _logger.debug("Receiving message with ID: " + msgs[k].getHeaders().getMessageId());
                            _logger.debug(msgs[k].getBody().getHexDump());
                        }
                    }
                }
            }
        } catch (Exception e) {
            _logger.error("error communicating with " + getHost(), e);
        }
    }
}

class TestProperties extends Properties {
    TestProperties() {}

    public int getIntProperty(String key, int defaultValue) {
        return Integer.parseInt(getProperty(key, String.valueOf(defaultValue)));
    } 
}
