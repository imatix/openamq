/**
 *  JMSHelloWorld.java - JMS Hello World using AMQP/Fast.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2005 JPMorgan
 *  Copyright (c) 2005 iMatix Corporation
 *
 **/

import java.util.Properties;
import javax.jms.*;
import org.openamq.*;
import org.openamq.jmsclient.*;

public class JMSHelloWorld {
    static boolean
        verbose = false;                /* Verbose mode                      */

    public static void main(String[] args) {
        final String
            CLIENT_NAME =               /* Client name                       */
                "JMS Hello World",
            USAGE =                     /* Usage                             */
                "Syntax: clientname [options...]\n"                                       +
                "Options:\n"                                                              +
                "  -c clientname    Client identifier (default: '" + CLIENT_NAME + "')\n" +
                "  -s server        Name or address of server (localhost)\n"              +
                "  -w destination   Destination on default host\n"                        +
                "  -m number        Number of messages to send/receive (1000)\n"          +
                "  -q               Quiet mode: no messages\n"                            +
                "  -v               Show version information\n"                           +
                "  -h               Show summary of command-line options\n"               +
                "\nThe order of arguments is not important. Switches and filenames\n"     +
                "are case sensitive.\n",
            CLIENT_NAME_PRINT =         /* Full name for console             */
                CLIENT_NAME + " - " + AMQFramingFactory.VERSION + "\n",
            COPYRIGHT =                 /* Copyright                         */
                "Copyright (c) 2004-2005 JPMorgan",
            NOWARRANTY =                /* Warranty                          */
                "This is free software; see the source for copying conditions.  There is NO\n"  +
                "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n";

        boolean
            args_ok = true;             /* Arguments parsing status          */
        int
            messages = 1000;            /* Messages to send                  */
        String
            destName = "test-small";    /* Server destination                */
        Properties                      /* Command-line arguments            */
            arguments = new Properties();   
        String
            argparm = null;             /* Command line argument             */
    
        for (int argn = 0; argn < args.length; argn++) {
            /*  If argparm is set, we have to collect an argument parameter  */
            if (argparm != null) {
                if (!args[argn].startsWith("-")) {  /*  Parameter can't start with '-'   */
                    arguments.setProperty(argparm, args[argn]);
                    argparm = null;
                } else {
                    args_ok = false;
                    break;
                }
            } else if (args[argn].startsWith("-")) {
                switch (args[argn].charAt(1)) {
                    /*  These switches take a parameter                      */
                    case 'c':
                        argparm = "opt_client";
                        break;
                    case 's':
                        argparm = "opt_server";
                        break;
                    case 'm':
                        argparm = "opt_messages";
                        break;
                    case 'w':
                        argparm = "opt_destination";
                        break;
    
                    /*  These switches have an immediate effect              */
                    case 'q':
                        verbose = true;
                        break;
                    case 'v':
                        System.out.println(CLIENT_NAME_PRINT);
                        System.out.println(COPYRIGHT);
                        System.out.println(NOWARRANTY);
                        System.exit(0);
                    case 'h':
                        System.out.println(CLIENT_NAME_PRINT);
                        System.out.println(COPYRIGHT);
                        System.out.println(NOWARRANTY);
                        System.out.println(USAGE);
                        System.exit(0);
    
                    /*  Anything else is an error                            */
                    default:
                        args_ok = false;
                }
            } else {
                args_ok = false;
                break;
            }
        }
        /*  If there was a missing parameter or an argument error, quit      */
        if (argparm != null) {
            System.out.println("Argument missing - use -h for help");
            System.exit(1);
        } else if (!args_ok) {
            System.out.println("Invalid arguments - use -h for help");
            System.exit(1);
        }
    
        messages = Integer.parseInt(arguments.getProperty("opt_messages", "1000"));
        verbose = Integer.parseInt(arguments.getProperty("opt_trace", "0")) > 0;
    
        test(destName, messages);
    }

    static void test(String destName, int messages) {
        Connection
            connection = null;
        Session
            session;
        Queue
            que;
        MessageProducer
            producer;
        TextMessage
            message;

        try {
            connection = new JMSConnection("localhost", 7654);
            session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            que = new JMSQueue(destName);
            producer = session.createProducer(que);
            message = session.createTextMessage();

            for (int i = 0; i < messages; i++) {
                String
                    text =
                        "The Message interface is the root interface of all JMS messages." +
                        "It defines the message header and the acknowledge method used for all messages." +
                        "Most message-oriented middleware (MOM) products treat messages as lightweight " +
                        "entities that consist of a header and a payload. The header contains fields used for " +
                        "message routing and identification; the payload contains the application data being sent." +
                        "Within this general form, the definition of a message varies significantly across products. " +
                        "It would be quite difficult for the JMS API to support all of these message models.";

                message.setText(text);
                if (verbose)
                    System.out.println("I: Sending text message #" + (i + 1) + " to server.");
                producer.send(message);
            }

            /* Send an end-of-queue message                                  */
            producer.send(session.createMessage());
        } catch (JMSException e) {
            System.out.println("JMSHelloWorld: " + "test: " + e.toString());
            System.exit(1);
        } finally {
            if (connection != null) {
                try {
                    connection.close();
                } catch (JMSException e) {}
            }
        }
    }
}
