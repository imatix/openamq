package org.openamq.protocol;

import java.awt.*;
import java.applet.*;
import java.net.*;
import java.util.*;
import java.io.*;

import org.apache.log4j.Logger;

import org.openamq.framing.*;
import org.openamq.*;

public class amqconnectionstate extends amqconnectionstatei implements Runnable
{

//////////////////////////////   G L O B A L S   //////////////////////////////

private static final Logger
    _logger = Logger.getLogger(amqconnectionstate.class);

AMQClientConnection
    acc;
AMQProtocolHandler
    aph;
LinkedList
    frames;
AMQFrame
    frame;
AMQMethodBody
    amb = null;
boolean
    expect_external_event,
    connection_opened,
    connection_opening;

///////////////////////////   C O N T R U C T O R   ///////////////////////////

public amqconnectionstate (AMQClientConnection acc)
{
    this.acc = acc;
    this.aph = acc.getProtocolHandler();
    frames = new LinkedList();
    frame = null;
    expect_external_event = false;
    connection_opened = false;
    connection_opening = true;
}

//////////////////////////////////   M A I N   ////////////////////////////////

public void run ()
{
    execute();
}

//////////////////////////   INITIALISE THE PROGRAM   /////////////////////////

public void initialise_the_program ()
{
    the_next_event = ok_event;
}

////////////////////////////   SET EXTERNAL EVENT   ///////////////////////////

public void set_external_event (AMQFrame frame)
{
    synchronized (frames) {
        frames.add(frame);
        frames.notifyAll();
    }
}

////////////////////////////   GET EXTERNAL EVENT   ///////////////////////////

public void get_external_event ()
{
    synchronized (frames) {
        while (expect_external_event) {
            if (frames.isEmpty()) {
                try {
                    frames.wait();
                } catch (InterruptedException e) {}
            } else {
                frame = (AMQFrame)frames.removeFirst();
                expect_external_event = false;
            }
        }
    }

    try {
        if (frame != null) {
            if (frame.bodyFrame instanceof HeartbeatBody) {
                the_next_event = connection_open_ok_event;
            } else if (frame.bodyFrame instanceof AMQMethodBody) {
                amb = (AMQMethodBody)frame.bodyFrame;
    
                if (amb.getId() < 2000) {
                    // Connection event
                    switch (amb.getId())
                    {
                        case 1001:
                            the_next_event = protocol_initiation_ok_event;
                            break;
                        case 1005:
                            the_next_event = connection_tune_event;
                            break;
                        case 1008:
                            synchronized (this) {
                                connection_opened = true;
                                connection_opening = false;
                                notifyAll();
                            }
                            the_next_event = connection_open_ok_event;
                            break;
                        case 1010:
                            the_next_event = connection_close_event;
                            break;
                        case 1011:
                            the_next_event = connection_finished_event;
                            break;
                        default:
                            _logger.debug("No special action for frame at connection level: " + frame);
                            the_next_event = connection_open_ok_event;
                    }
                } else {
                    // Dispatch to sessions
                    acc.dispatchFrame(frame);
                    the_next_event = connection_open_ok_event;
                }
            } else if (frame.bodyFrame instanceof ContentBody || frame.bodyFrame instanceof ContentHeaderBody) {
                // Dispatch to sessions
                acc.dispatchFrame(frame);
                the_next_event = connection_open_ok_event;
            } else {
                acc.close(AMQConstant.NOT_ALLOWED, "Frame not allowed at connection level: " + frame, 0, 0);
                clean_up();
            }
        } else {
            int
                errorCode;
            String
                errorMessage;
    
            synchronized (frames) {
                if (frames.isEmpty()) {
                    errorCode = AMQConstant.INTERNAL_ERROR;
                    errorMessage = "FSM error at connection level";
                } else {
                    errorCode = AMQConstant.NOT_ALLOWED;
                    errorMessage = "Frame not allowed at connection level: " + frame;
                }
            }
            acc.close(errorCode, errorMessage, 0, 0);
            clean_up();
        }
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

///////////////////////   WAIT FOR CONNECTION OPENED   ////////////////////////

public void wait_connection_opened ()
{
    synchronized (this) {
        while (!connection_opened) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }
}

///////////////////////   CHECK FOR CONNECTION OPENED   ///////////////////////

public boolean is_connection_opened ()
{
    synchronized (this) {
        return connection_opened;
    }
}

///////////////////////   CHECK FOR CONNECTION OPENING   //////////////////////

public boolean is_connection_opening ()
{
    synchronized (this) {
        return connection_opening;
    }
}

//%START MODULE

/////////////////////////   SEND PROTOCOL INITIATION   ////////////////////////

public void send_protocol_initiation ()
{
    // Send protocol initialization
    try {
        aph.writeFrame(null, new ProtocolInitiation());
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

/////////////////////////////   CONNECTION START   ////////////////////////////

public void connection_start_ok ()
{
    ConnectionStartBody
        csb = (ConnectionStartBody)amb;
    FieldTable
        client = new FieldTable();
    AuthData
        ad = acc.getAuthData();

    client.put("instance", acc.getClientInstance());
    client.put("product", "OpenAMQ/J");
    client.put("version", "0.1a");
    client.put("platform", "java");
    client.put("copyright", "(C) iMatix 2006");
    try {
        aph.writeFrame(null, ConnectionStartOkBody.createAMQFrame(0, client, ad.getAuthMechanism(), ad.getAuthResponse(), "en_US"));
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

/////////////////////////////   CONNECTION OPEN   /////////////////////////////

public void connection_open ()
{
    try {
        aph.writeFrame(null, ConnectionOpenBody.createAMQFrame(0, acc.getVirtualHost(), null, true));
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

///////////////////////////   CONNECTION CLOSE OK   ///////////////////////////

public void connection_close_ok ()
{
    try {
        aph.writeFrame(null, ConnectionCloseOkBody.createAMQFrame(0));
        the_next_event = connection_finished_event;
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

/////////////////////////////////   CLEAN UP   ////////////////////////////////

public void clean_up ()
{
    synchronized (this) {
        connection_opened = false;
    }
    aph.closeProtocolSession();
}

////////////////////////////   CONNECTION TUNE OK   ///////////////////////////

public void connection_tune_ok ()
{
    ConnectionTuneBody
        ctb = (ConnectionTuneBody)amb;

    acc.setConnectionTuneData(ctb);
    try {
        aph.writeFrame(null, ConnectionTuneOkBody.createAMQFrame(0, ctb.channelMax, ctb.frameMax, ctb.heartbeat));
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

///////////////////////////////   EXPECT FRAME   //////////////////////////////

public void expect_frame ()
{
    synchronized (frames) {
        expect_external_event = true;
    }
}

//%END MODULE
}
