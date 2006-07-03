package org.openamq.protocol;

import java.awt.*;
import java.applet.*;
import java.net.*;
import java.util.*;
import java.io.*;

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;

import org.openamq.framing.*;
import org.openamq.*;

public class AMQChannelState extends AMQChannelStateI implements Runnable
{

//////////////////////////////   G L O B A L S   //////////////////////////////

private static final Logger
    _logger = Logger.getLogger(AMQChannelState.class);

AMQClientConnection
    acc;
AMQClientSession
    acs;
AMQProtocolHandler
    aph;
HashMap
    MethodToEvent;
LinkedList
    frames;
AMQFrame
    frame;
AMQMethodBody
    amb;
AMQMessage
    message;
boolean
    ExpectExternalEvent,
    ChannelOpened,
    ChannelOpening;

///////////////////////////   C O N T R U C T O R   ///////////////////////////

public AMQChannelState (AMQClientConnection acc, AMQClientSession acs)
{
    this.acc = acc;
    this.acs = acs;
    this.aph = acc.getProtocolHandler();
    MethodToEvent = new HashMap();
    frames = new LinkedList();
    frame = null;
    amb = null;
    message = null;
    ExpectExternalEvent = false;
    ChannelOpened = false;
    ChannelOpening = true;

    MethodToEvent.put(ChannelOpenBody.class, new Integer(ChannelOpenOkEvent));
    MethodToEvent.put(ChannelCloseBody.class, new Integer(ChannelCloseEvent));
    MethodToEvent.put(ChannelCloseOkBody.class, new Integer(ChannelFinishedEvent));
    MethodToEvent.put(BasicReturnBody.class, new Integer(BasicReturnEvent));
    MethodToEvent.put(BasicDeliverBody.class, new Integer(BasicDeliverEvent));
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
            if (frame.bodyFrame instanceof AMQMethodBody) {
                amb = (AMQMethodBody)frame.bodyFrame;
                // Get connection event
                Integer event = (Integer)MethodToEvent.get(amb.getClass());

                if (event != null)
                {
                    TheNextEvent = event.intValue();
                }
                else
                {
                    _logger.debug("No special action for frame (at channel level): " + frame);
                    TheNextEvent = ChannelOpenOkEvent;
                }
            } else if (frame.bodyFrame instanceof ContentHeaderBody) {
                TheNextEvent = ContentHeaderEvent;
            } else if (frame.bodyFrame instanceof ContentBody) {
                TheNextEvent = ContentBodyEvent;
            } else {
                acs.close(AMQConstant.NOT_ALLOWED, "Frame not allowed (at channel level): " + frame, 0, 0);
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
                    errorMessage = "FSM error (at channel level)";
                } else {
                    errorCode = AMQConstant.NOT_ALLOWED;
                    errorMessage = "Frame not allowed (at channel level): " + frame;
                }
            }
            acs.close(errorCode, errorMessage, 0, 0);
            Cleanup();
            RaiseException(0);
        }
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

///////////////////////   WAIT FOR CHANNEL OPENED   ///////////////////////////

public void WaitChannelOpened ()
{
    synchronized (this) {
        while (!ChannelOpened) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }
}

////////////////////////   CHECK FOR CHANNEL OPENED   /////////////////////////

public boolean IsChannelOpened ()
{
    synchronized (this) {
        return ChannelOpened;
    }
}

////////////////////////   CHECK FOR CHANNEL OPENING   ////////////////////////

public boolean IsChannelOpening ()
{
    synchronized (this) {
        return ChannelOpening;
    }
}

//%START MODULE

///////////////////////////////   CHANNEL OPEN   //////////////////////////////

public void ChannelOpen ()
{
    try {
        aph.writeFrame(acs, ChannelOpenBody.createAMQFrame(acs.getSessionId(), null));
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

/////////////////////////   CHANNEL OPEN OK HANDLER   /////////////////////////

public void ChannelOpenOkHandler ()
{
    synchronized (this) {
        ChannelOpened = true;
        ChannelOpening = false;
        notifyAll();
    }
}

///////////////////////////////   EXPECT FRAME   //////////////////////////////

public void ExpectFrame ()
{
    if (IsChannelOpened() || IsChannelOpening())
    {
        synchronized (frames) {
            ExpectExternalEvent = true;
        }
    }
}


/////////////////////////////   CHANNEL CLOSE OK   ////////////////////////////

public void ChannelCloseOk ()
{
    try {
        aph.writeFrame(acs, ChannelCloseOkBody.createAMQFrame(acs.getSessionId()));
        TheNextEvent = ChannelFinishedEvent;
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}


/////////////////////////////////   CLEAN UP   ////////////////////////////////

public void Cleanup ()
{
    synchronized (this) {
        ChannelOpened = false;
        ChannelOpening = false;
    }
    TheNextEvent = TerminateEvent;
}

//////////////////////////   BASIC DELIVER HANDLER   //////////////////////////

public void BasicDeliverHandler ()
{
    message = acs.createMessage();
    message.setDelivery(amb);
}

//////////////////////////   BASIC RETURN HANDLER   ///////////////////////////

public void BasicReturnHandler ()
{
    BasicDeliverHandler();
}

/////////////////////////////   DISPATCH MESSAGE   ////////////////////////////

public void DispatchMessage ()
{
    acs.messageReceived(message);
    message = null;

    TheNextEvent = ChannelOpenOkEvent;
}


//////////////////////////////   CONSUME HEADER   /////////////////////////////

public void ConsumeHeader ()
{
    try {
        if (message == null) {
            acs.close(AMQConstant.NOT_ALLOWED, "Content header not allowed (received before delivery frame): " + frame, 0, 0);
            Cleanup();
            RaiseException(0);
        } else {
            ContentHeaderBody
                chb = (ContentHeaderBody)frame.bodyFrame;

            message.setHeaders((BasicContentHeaderProperties)chb.properties);
            if (chb.bodySize <= Integer.MAX_VALUE) {
                int
                    size = (int)chb.bodySize;

                message.setBody(ByteBuffer.allocate(size));
                message.getBody().limit(size);
            } else {
                throw new Exception("Message size too big for client ( > " + Integer.MAX_VALUE + " )");
            }
        }
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}


///////////////////////////////   CONSUME BODY   //////////////////////////////

public void ConsumeBody ()
{
    try {
        if (message == null) {
            acs.close(AMQConstant.NOT_ALLOWED, "Content not allowed (received before delivery frame): " + frame, 0, 0);
            Cleanup();
            RaiseException(0);
        } else if (message.getHeaders() == null) {
            acs.close(AMQConstant.NOT_ALLOWED, "Content not allowed (received before content header frame): " + frame, 0, 0);
            Cleanup();
            RaiseException(0);
        } else {
            ByteBuffer
                payload = ((ContentBody)frame.bodyFrame).payload;
            int
                cmp = message.getBody().limit() - (message.getBody().position() + payload.remaining());

            if (cmp < 0) {
                acs.close(AMQConstant.CONTENT_TOO_LARGE, "Message content too large (" + (-cmp) + " bytes) with frame: " + frame, 0, 0);
                Cleanup();
                RaiseException(0);
            } else {
                message.getBody().put(payload);

                if (cmp == 0) {
                    message.getBody().flip();
                    TheNextEvent = MessageConsumedEvent;
                }
            }
        }
    } catch (Exception e) {
        throw new RuntimeException(e);
    }

}


//%END MODULE
}
