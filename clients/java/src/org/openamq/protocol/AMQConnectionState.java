package org.openamq.protocol;

import java.awt.*;
import java.applet.*;
import java.net.*;
import java.util.*;
import java.io.*;

import org.apache.log4j.Logger;

import org.openamq.framing.*;
import org.openamq.*;

public class AMQConnectionState extends AMQConnectionStateI implements Runnable
{

//////////////////////////////   G L O B A L S   //////////////////////////////

private static final Logger
    _logger = Logger.getLogger(AMQConnectionState.class);

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
    ExpectExternalEvent,
    ConnectionOpened,
    ConnectionOpening;

///////////////////////////   C O N T R U C T O R   ///////////////////////////

public AMQConnectionState (AMQClientConnection acc)
{
    this.acc = acc;
    this.aph = acc.getProtocolHandler();
    frames = new LinkedList();
    frame = null;
    ExpectExternalEvent = false;
    ConnectionOpened = false;
    ConnectionOpening = true;
}

//////////////////////////////////   M A I N   ////////////////////////////////

public void run ()
{
    execute();
}

//////////////////////////   INITIALISE THE PROGRAM   /////////////////////////

public void InitialiseTheProgram ()
{
    TheNextEvent = OkEvent;
}

////////////////////////////   SET EXTERNAL EVENT   ///////////////////////////

public void SetExternalEvent (AMQFrame frame)
{
    synchronized (frames) {
        frames.add(frame);
        frames.notifyAll();
    }
}

////////////////////////////   GET EXTERNAL EVENT   ///////////////////////////

public void GetExternalEvent ()
{
    synchronized (frames) {
        while (ExpectExternalEvent) {
            if (frames.isEmpty()) {
                try {
                    frames.wait();
                } catch (InterruptedException e) {}
            } else {
                frame = (AMQFrame)frames.removeFirst();
                ExpectExternalEvent = false;
            }
        }
    }

    try {
        if (frame != null) {
            if (frame.bodyFrame instanceof HeartbeatBody) {
                TheNextEvent = ConnectionOpenOkEvent;
            } else if (frame.bodyFrame instanceof AMQMethodBody) {
                amb = (AMQMethodBody)frame.bodyFrame;

                if (amb.getId() < 2000) {
                    // Connection event
                    switch (amb.getId())
                    {
                        case 1001:
                            TheNextEvent = ProtocolInitiationOkEvent;
                            break;
                        case 1005:
                            TheNextEvent = ConnectionTuneEvent;
                            break;
                        case 1008:
                            synchronized (this) {
                                ConnectionOpened = true;
                                ConnectionOpening = false;
                                notifyAll();
                            }
                            TheNextEvent = ConnectionOpenOkEvent;
                            break;
                        case 1010:
                            TheNextEvent = ConnectionCloseEvent;
                            break;
                        case 1011:
                            TheNextEvent = ConnectionFinishedEvent;
                            break;
                        default:
                            _logger.debug("No special action for frame at connection level: " + frame);
                            TheNextEvent = ConnectionOpenOkEvent;
                    }
                } else {
                    // Dispatch to sessions
                    acc.dispatchFrame(frame);
                    TheNextEvent = ConnectionOpenOkEvent;
                }
            } else if (frame.bodyFrame instanceof ContentBody || frame.bodyFrame instanceof ContentHeaderBody) {
                // Dispatch to sessions
                acc.dispatchFrame(frame);
                TheNextEvent = ConnectionOpenOkEvent;
            } else {
                acc.close(AMQConstant.NOT_ALLOWED, "Frame not allowed at connection level: " + frame, 0, 0);
                CleanUp();
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
            CleanUp();
        }
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

///////////////////////   WAIT FOR CONNECTION OPENED   ////////////////////////

public void WaitConnectionOpened ()
{
    synchronized (this) {
        while (!ConnectionOpened) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }
}

///////////////////////   CHECK FOR CONNECTION OPENED   ///////////////////////

public boolean IsConnectionOpened ()
{
    synchronized (this) {
        return ConnectionOpened;
    }
}

///////////////////////   CHECK FOR CONNECTION OPENING   //////////////////////

public boolean IsConnectionOpening ()
{
    synchronized (this) {
        return ConnectionOpening;
    }
}

//%START MODULE

/////////////////////////   SEND PROTOCOL INITIATION   ////////////////////////

public void SendProtocolInitiation ()
{
    // Send protocol initialization
    try {
        aph.writeFrame(null, new ProtocolInitiation());
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

/////////////////////////////   CONNECTION START   ////////////////////////////

public void ConnectionStartOk ()
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

public void ConnectionOpen ()
{
    try {
        aph.writeFrame(null, ConnectionOpenBody.createAMQFrame(0, acc.getVirtualHost(), null, true));
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

///////////////////////////   CONNECTION CLOSE OK   ///////////////////////////

public void ConnectionCloseOk ()
{
    try {
        aph.writeFrame(null, ConnectionCloseOkBody.createAMQFrame(0));
        TheNextEvent = ConnectionFinishedEvent;
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

/////////////////////////////////   CLEAN UP   ////////////////////////////////

public void CleanUp ()
{
    synchronized (this) {
        ConnectionOpened = false;
    }
    aph.closeProtocolSession();
}

////////////////////////////   CONNECTION TUNE OK   ///////////////////////////

public void ConnectionTuneOk ()
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

public void ExpectFrame ()
{
    synchronized (frames) {
        ExpectExternalEvent = true;
    }
}

//%END MODULE
}
