/**
 *  JMSConnection.java - JMS Connection implementation using AMQP/Fast.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2005 JPMorgan
 *  Copyright (c) 2005 iMatix Corporation
 *
 **/

package org.openamq.jmsclient;

import javax.jms.Connection;
import javax.jms.ExceptionListener;
import javax.jms.Destination;
import javax.jms.Topic;
import javax.jms.Session;
import javax.jms.ServerSessionPool;
import javax.jms.ConnectionConsumer;
import javax.jms.ConnectionMetaData;
import javax.jms.JMSException;

import java.net.*;
import java.util.*;
import java.io.*;
import org.openamq.*;
import org.openamq.frames.*;

public class JMSConnection implements Connection {
    static int
        connID = 1;                         /* Client id                         */

    short
        protocolID = 128,                   /* Protocol id                       */
        protocolVer = 1;                    /* Protocol version                  */
    int
        frameMax = 65536;                   /* Max frame size we can handle      */
    Socket
        amqp = null;                        /* Network socket                    */
    LinkedList
        sessions;                           /* Sessions list                     */
    AMQFieldTable
        table;                              /* Field table                       */
    AMQFramingFactory
        amqFraming = null;                  /* Framing utility                   */
    AMQConnection.Open
        clientOpen;                         /* Connection parameters             */
    AMQConnection.Tune
        clientTune;                         /* Tune parameters                   */
    AMQConnection.Close
        clientClose;                        /* Default close parameters          */

    public JMSConnection(String server, int port) throws JMSException {

        try
        {
            sessions = new LinkedList();
            // Network setup
            System.out.println("Connecting to " + server + "...");
            amqp = new Socket(server, port);
            amqFraming = new AMQFramingFactory(amqp);
            amqFraming.verbose = false;
            System.out.println("I: connected to AMQP server on " + server + ":" + port);
            // Client tune capabilities
            clientTune = (AMQConnection.Tune)amqFraming.constructFrame(AMQConnection.TUNE);
            clientTune.options = null;
            clientTune.dictionary = null;
            // Client name and connection open defaults
            clientOpen = (AMQConnection.Open)amqFraming.constructFrame(AMQConnection.OPEN);
            clientOpen.confirmTag = 0;
            clientOpen.virtualPath = null;
            clientOpen.clientName = "OpenAMQ/Java Client " + connID++;
            clientOpen.options = null;
            // Connection close defaults
            clientClose = (AMQConnection.Close)amqFraming.constructFrame(AMQConnection.CLOSE);
            clientClose.replyCode = 200;
            clientClose.replyText = "bye";

            // Send initiation
            amqFraming.sendConnectionInitiation(protocolID, protocolVer);

            // Authenticate
            authenticate();

            // Connection tune
            tune();

            // Open connection
            clientOpen.virtualPath = "/";
            amqFraming.sendFrame(clientOpen);
        } catch (UnknownHostException e) {
            throw new JMSException("JMSConnection: " + "JMSConnection: " + "unknown host");
        } catch (SocketTimeoutException e) {
            throw new JMSException("JMSConnection: " + "JMSConnection: " + "SocketTimeoutException");
        } catch (IOException e) {
            throw new JMSException("JMSConnection: " + "JMSConnection: " + e.getMessage());
        } catch (AMQException e) {
            throw new JMSException("JMSConnection: " + "JMSConnection: " + e.getMessage());
        }
    }

    void authenticate() throws JMSException {
        try {
            AMQConnection.Challenge         /* Challenge from server             */
                challenge = (AMQConnection.Challenge)amqFraming.receiveFrame();
            AMQConnection.Response          /* our response                      */
                response = (AMQConnection.Response)amqFraming.constructFrame(AMQConnection.RESPONSE);
            // Send the response
            response.mechanism = "PLAIN";
            table = new AMQFieldTable();
            table.putString("LOGIN", amqFraming.string2Bytes("guest"));
            table.putString("PASSWORD", amqFraming.string2Bytes("guest"));
            response.responses = table.storeToBucket();
            amqFraming.sendFrame(response);
        } catch (ClassCastException e) {
            throw new JMSException("JMSConnection: " + "authenticate: " + "unexpected frame from server");
        } catch (SocketTimeoutException e) {
            throw new JMSException("JMSConnection: " + "authenticate: " + "SocketTimeoutException");
        } catch (IOException e) {
            throw new JMSException("JMSConnection: " + "authenticate: " + e.getMessage());
        } catch (AMQException e) {
            throw new JMSException("JMSConnection: " + "authenticate: " + e.getMessage());
        }
    }

    void tune() throws JMSException {
        try {
            AMQConnection.Tune              /* Tune parameters from server       */
                tune_server = null;
            tune_server = (AMQConnection.Tune)amqFraming.receiveFrame();
            amqFraming.setTuneParameters(tune_server);
            // Send the reply
            table = new AMQFieldTable();
            table.putInteger("FRAME_MAX", Math.min(frameMax, amqFraming.getFrameMax()));
            table.putInteger("HEARTBEAT", 0);
            clientTune.options = table.storeToBucket();
            amqFraming.sendFrame(clientTune);
            amqFraming.setTuneParameters(clientTune);
        } catch (ClassCastException e) {
            throw new JMSException("JMSConnection: " + "tune: " + "unexpected frame from server");
        } catch (SocketTimeoutException e) {
            throw new JMSException("JMSConnection: " + "tune: " + "SocketTimeoutException");
        } catch (IOException e) {
            throw new JMSException("JMSConnection: " + "tune: " + e.getMessage());
        } catch (AMQException e) {
            throw new JMSException("JMSConnection: " + "tune: " + e.getMessage());
        }
    }

    public Session createSession(boolean transacted, int acknowledgeMode) 
        throws JMSException {
        JMSSession
            session = new JMSSession(this, transacted, acknowledgeMode);

        sessions.add(session);

        return session;
    }    
    
    public String getClientID() throws JMSException {
        return null;    
    }

    public void setClientID(String clientID) throws JMSException {
    }
 
    public ConnectionMetaData getMetaData() throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public ExceptionListener getExceptionListener() throws JMSException {
        throwNotImplementedException();
        return null;
    }

    public void setExceptionListener(ExceptionListener listener) throws JMSException {
        throwNotImplementedException();
    }

    public void start() throws JMSException {
        /*
            Call the start method (provider-specific) on every consumer
            The consumer start method uses HANDLE FLOW to start incoming messages
        */
    }
 
    public void stop() throws JMSException {
        /*
            Call the stop method (provider-specific) on every consumer
            The consumer stop method uses HANDLE FLOW to stop incoming messages
        */
    }
 
    public void close() throws JMSException {
        while (sessions.size() > 0) {
            JMSSession
                session = (JMSSession)sessions.removeFirst();

            session.close();
            synchronized (session) {
                session.session = null;
                session.notify();
            }
        }

        try {
            if (amqFraming != null) {
                amqFraming.sendFrame(clientClose);
                clientClose = (AMQConnection.Close)amqFraming.receiveFrame();
    
                amqFraming.stopWriter();
            }
        } catch (ClassCastException e) {
            throw new JMSException("JMSConnection: " + "close: " + "unexpected frame from server");
        } catch (IOException e) {
            throw new JMSException("JMSConnection: " + "close: " + e.getMessage());
        } catch (AMQException e) {
            throw new JMSException("JMSConnection: " + "close: " + e.getMessage());
        }
    }
    
    public ConnectionConsumer createConnectionConsumer(Destination destination, 
        String messageSelector, ServerSessionPool sessionPool, int maxMessages)
        throws JMSException {
        throwNotImplementedException();
        return null;
    }    

    public ConnectionConsumer createDurableConnectionConsumer(Topic topic, 
        String subscriptionName, String messageSelector, 
        ServerSessionPool sessionPool, int maxMessages) throws JMSException {
        throwNotImplementedException();
        return null;
    }    
    
    private void throwNotImplementedException() throws JMSException {
        throw new JMSException("OpenAMQ/JMS: method not yet implemented.");
    }
}

