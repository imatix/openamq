package org.openamq.protocol;

import java.awt.*;
import java.applet.*;
import java.net.*;
import java.util.*;
import java.io.*;

import org.apache.log4j.Logger;
import org.apache.mina.common.ByteBuffer;

import org.openamq.*;
import org.openamq.framing.*;

public class AMQChannelState extends AMQChannelStateI implements Runnable
{

//////////////////////////////   G L O B A L S   //////////////////////////////

private static final Logger
    _logger = Logger.getLogger(AMQChannelState.class);

AMQClientConnectionI
    acc;
AMQClientSessionI
    acs;
AMQProtocolHandler
    aph;
HashMap
    methodToEvent;
LinkedList
    frames;
AMQFrame
    frame;
AMQMethodBody
    amb;
AMQMessageI
    message;
boolean
    expectExternalEvent,
    channelOpened,
    channelOpening;

///////////////////////////   C O N T R U C T O R   ///////////////////////////

public AMQChannelState (AMQClientConnectionI acc, AMQClientSessionI acs)
{
    this.acc = acc;
    this.acs = acs;
    this.aph = acc.getProtocolHandler();
    methodToEvent = new HashMap();
    frames = new LinkedList();
    frame = null;
    amb = null;
    message = null;
    expectExternalEvent = false;
    channelOpened = false;
    channelOpening = true;

    methodToEvent.put(ChannelOpenOkBody.class, new Integer(channelOpenOkEvent));
    methodToEvent.put(ChannelCloseBody.class, new Integer(channelCloseEvent));
    methodToEvent.put(ChannelCloseOkBody.class, new Integer(channelCloseOkEvent));
    methodToEvent.put(BasicReturnBody.class, new Integer(basicReturnEvent));
    methodToEvent.put(BasicDeliverBody.class, new Integer(basicDeliverEvent));
}

//////////////////////////////////   M A I N   ////////////////////////////////

public void run ()
{
    execute();
    if (theNextEvent != terminateEvent) {
        cleanup();
        _logger.error("Channel-level FSM bug");
    }
}


//////////////////////////   INITIALISE THE PROGRAM   /////////////////////////

public void initialiseTheProgram ()
{
    theNextEvent = okEvent;
}

////////////////////////////   SET EXTERNAL EVENT   ///////////////////////////

public void setExternalEvent (AMQFrame frame)
{
    synchronized (frames) {
        frames.add(frame);
        frames.notifyAll();
    }
}

////////////////////////////   GET EXTERNAL EVENT   ///////////////////////////

public void getExternalEvent ()
{
    synchronized (frames) {
        while (expectExternalEvent && (channelOpening || channelOpened)) {
            if (frames.isEmpty()) {
                try {
                    frames.wait();
                } catch (InterruptedException e) {}
            } else {
                frame = (AMQFrame)frames.removeFirst();
                expectExternalEvent = false;
            }
        }
    }
    synchronized (this) {
        if (!(channelOpening || channelOpened))
            return;
    }

    try {
        if (frame != null) {
            if (frame.bodyFrame instanceof AMQMethodBody) {
                amb = (AMQMethodBody)frame.bodyFrame;
                // Get connection event
                Integer event = (Integer)methodToEvent.get(amb.getClass());

                if (event != null)
                {
                    theNextEvent = event.intValue();
                }
                else
                {
                    _logger.debug("No special action for frame (at channel level): " + frame);
                    theNextEvent = channelOpenOkEvent;
                }
            } else if (frame.bodyFrame instanceof BasicContentHeaderBody) {
                theNextEvent = contentHeaderEvent;
            } else if (frame.bodyFrame instanceof ContentBody) {
                theNextEvent = contentBodyEvent;
            } else {
                acs.close(AMQConstant.NOT_ALLOWED, "Frame not allowed (at channel level): " + frame, 0, 0);
                cleanup();
                raiseException(0);
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
            cleanup();
            raiseException(0);
        }
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

///////////////////////   WAIT FOR CHANNEL OPENED   ///////////////////////////

public void waitChannelOpened ()
{
    synchronized (this) {
        while (channelOpening && !channelOpened) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }
}

////////////////////////   CHECK FOR CHANNEL OPENED   /////////////////////////

public boolean isChannelOpened ()
{
    synchronized (this) {
        return channelOpened;
    }
}

////////////////////////   CHECK FOR CHANNEL OPENING   ////////////////////////

public boolean isChannelOpening ()
{
    synchronized (this) {
        return channelOpening;
    }
}

//%START MODULE

///////////////////////////////   CHANNEL OPEN   //////////////////////////////

public void channelOpen ()
{
    try {
        aph.writeFrame(acs, ChannelOpenBody.createAMQFrame(acs.getSessionId(), null));
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

/////////////////////////   CHANNEL OPEN OK HANDLER   /////////////////////////

public void channelOpenOkHandler ()
{
    synchronized (this) {
        if (channelOpening) {
            channelOpened = true;
            channelOpening = false;
            notifyAll();
        }
    }
}

///////////////////////////////   EXPECT FRAME   //////////////////////////////

public void expectFrame ()
{
    if (isChannelOpened() || isChannelOpening())
    {
        synchronized (frames) {
            expectExternalEvent = true;
        }
    }
}


/////////////////////////////   CHANNEL CLOSE OK   ////////////////////////////

public void channelCloseOk ()
{
    try {
        aph.writeFrame(acs, ChannelCloseOkBody.createAMQFrame(acs.getSessionId()));
        channelClosed();
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}

/////////////////////////////    CHANNEL CLOSED   /////////////////////////////

public void channelClosed ()
{
    theNextEvent = channelFinishedEvent;
}


/////////////////////////////////   CLEAN UP   ////////////////////////////////

public void cleanup ()
{
    theNextEvent = terminateEvent;
    synchronized (this) {
        acs.disableSession();
        channelOpened = false;
        channelOpening = false;
        notifyAll();
    }
    synchronized (frames) {
        expectExternalEvent = false;
        frames.notifyAll();
    }
}

//////////////////////////   BASIC DELIVER HANDLER   //////////////////////////

public void basicDeliverHandler ()
{
    message = acs.createMessage();
    message.setDelivery(amb);
}

//////////////////////////   BASIC RETURN HANDLER   ///////////////////////////

public void basicReturnHandler ()
{
    basicDeliverHandler();
}

/////////////////////////////   DISPATCH MESSAGE   ////////////////////////////

public void dispatchMessage ()
{
    acs.messageReceived(message);
    message = null;

    theNextEvent = channelOpenOkEvent;
}


//////////////////////////////   CONSUME HEADER   /////////////////////////////

public void consumeHeader ()
{
    try {
        if (message == null) {
            acs.close(AMQConstant.NOT_ALLOWED, "Content header not allowed (received before delivery frame): " + frame, 0, 0);
            cleanup();
            raiseException(0);
        } else {
            BasicContentHeaderBody
                bchb = (BasicContentHeaderBody)frame.bodyFrame;

            message.setHeaders((BasicContentHeaderProperties)bchb.properties);
            if (bchb.bodySize <= Integer.MAX_VALUE) {
                int
                    size = (int)bchb.bodySize;

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

public void consumeBody ()
{
    try {
        if (message == null) {
            acs.close(AMQConstant.NOT_ALLOWED, "Content not allowed (received before delivery frame): " + frame, 0, 0);
            cleanup();
            raiseException(0);
        } else if (message.getHeaders() == null) {
            acs.close(AMQConstant.NOT_ALLOWED, "Content not allowed (received before content header frame): " + frame, 0, 0);
            cleanup();
            raiseException(0);
        } else {
            ByteBuffer
                payload = ((ContentBody)frame.bodyFrame).payload;
            int
                cmp = message.getBody().limit() - (message.getBody().position() + payload.remaining());

            if (cmp < 0) {
                acs.close(AMQConstant.CONTENT_TOO_LARGE, "Message content too large (" + (-cmp) + " bytes) with frame: " + frame, 0, 0);
                cleanup();
                raiseException(0);
            } else {
                message.getBody().put(payload);

                if (cmp == 0) {
                    message.getBody().flip();
                    theNextEvent = messageConsumedEvent;
                }
            }

            payload.release();
        }
    } catch (Exception e) {
        throw new RuntimeException(e);
    }

}


//%END MODULE
}
