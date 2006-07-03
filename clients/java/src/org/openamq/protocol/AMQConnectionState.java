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
HashMap
    MethodToEvent;
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
    MethodToEvent = new HashMap();
    frames = new LinkedList();
    frame = null;
    ExpectExternalEvent = false;
    ConnectionOpened = false;
    ConnectionOpening = true;

    MethodToEvent.put(ConnectionStartBody.class, new Integer(ProtocolInitiationOkEvent));
    MethodToEvent.put(ConnectionTuneBody.class, new Integer(ConnectionTuneEvent));
    MethodToEvent.put(ConnectionOpenOkBody.class, new Integer(ConnectionOpenOkEvent));
    MethodToEvent.put(ConnectionCloseBody.class, new Integer(ConnectionCloseEvent));
    MethodToEvent.put(ConnectionCloseOkBody.class, new Integer(ConnectionFinishedEvent));
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

                if (amb.getClazz() == ConnectionStartBody.CLASS_ID) {
                    // Get connection event
                    Integer event = (Integer)MethodToEvent.get(amb.getClass());

                    if (event != null)
                    {
                        TheNextEvent = event.intValue();
                    }
                    else
                    {
                        _logger.debug("No special action for frame (at connection level): " + frame);
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
                acc.close(AMQConstant.NOT_ALLOWED, "Frame not allowed (at connection level): " + frame, 0, 0);
                Cleanup();
                RaiseException(0);
            }
        } else {
            int
                errorCode;
            String
                errorMessage;

            synchronized (frames) {
                if (frames.isEmpty()) {
                    errorCode = AMQConstant.INTERNAL_ERROR;
                    errorMessage = "FSM error (at connection level)";
                } else {
                    errorCode = AMQConstant.NOT_ALLOWED;
                    errorMessage = "Frame not allowed (at connection level): " + frame;
                }
            }
            acc.close(errorCode, errorMessage, 0, 0);
            Cleanup();
            RaiseException(0);
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

////////////////////////   CONNECTION OPEN OK HANDLER   ///////////////////////

public void ConnectionOpenOkHandler ()
{
    synchronized (this) {
        ConnectionOpened = true;
        ConnectionOpening = false;
        notifyAll();
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

public void Cleanup ()
{
    synchronized (this) {
        ConnectionOpened = false;
        ConnectionOpening = false;
    }
    aph.closeProtocolSession();
    TheNextEvent = TerminateEvent;
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
    if (IsConnectionOpened() || IsConnectionOpening())
    {
        synchronized (frames) {
            ExpectExternalEvent = true;
        }
    }
}

//%END MODULE
}
