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
    _logger = Logger.getLogger(amqchannelstate.class);

AMQClientConnection
    acc;
AMQClientSession
    acs;
AMQProtocolHandler
    aph;
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
    frames = new LinkedList();
    frame = null;
    amb = null;
    message = null;
    ExpectExternalEvent = false;
    ChannelOpened = false;
    ChannelOpening = true;
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
    
                switch (amb.getId())
                {
                    case 2002:
                        TheNextEvent = ChannelOpenOkEvent;
                        synchronized (this) {
                            ChannelOpened = true;
                            ChannelOpening = false;
                            notifyAll();
                        }
                        break;
                    case 2006:
                        TheNextEvent = ChannelCloseEvent;
                        break;
                    case 2007:
                        TheNextEvent = ChannelFinishedEvent;
                        break;
                    case 6006:
                        message = acs.createMessage();
                        message.setDelivery(amb);
                        TheNextEvent = BasicReturnEvent;
                        break;
                    case 6007:
                        message = acs.createMessage();
                        message.setDelivery(amb);
                        TheNextEvent = BasicDeliverEvent;
                        break;
                    default:
                        _logger.debug("No special action for frame (at channel level): " + frame);
                        TheNextEvent = ChannelOpenOkEvent;
                }
            } else if (frame.bodyFrame instanceof ContentHeaderBody) {
                TheNextEvent = ContentHeaderEvent;
            } else if (frame.bodyFrame instanceof ContentBody) {
                TheNextEvent = ContentBodyEvent;
            } else {
                acs.close(AMQConstant.NOT_ALLOWED, "Frame not allowed at session level: " + frame, 0, 0);
                clean_up();;
            }
        } else {
            int
                errorCode;
            String
                errorMessage;

            synchronized (frames) {
                if (frames.isEmpty()) {
                    errorCode = AMQConstant.INTERNAL_ERROR;
                    errorMessage = "FSM error at session level";
                } else {
                    errorCode = AMQConstant.NOT_ALLOWED;
                    errorMessage = "Frame not allowed at session level: " + frame;
                }
            }
            acs.close(errorCode, errorMessage, 0, 0);
            clean_up();
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


///////////////////////////////   EXPECT FRAME   //////////////////////////////

public void ExpectFrame ()
{
    synchronized (frames) {
        ExpectExternalEvent = true;
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

public void CleanUp ()
{
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
            clean_up();
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
            expect_frame();
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
            clean_up();
        } else if (message.getHeaders() == null) {
            acs.close(AMQConstant.NOT_ALLOWED, "Content not allowed (received before content header frame): " + frame, 0, 0);
            clean_up();
        } else {
            ByteBuffer
                payload = ((ContentBody)frame.bodyFrame).payload;
            int
                cmp = message.getBody().limit() - (message.getBody().position() + payload.remaining());

            if (cmp < 0) {
                acs.close(AMQConstant.CONTENT_TOO_LARGE, "Message content too large (" + (-cmp) + " bytes) with frame: " + frame, 0, 0);
                clean_up();
            } else {
                message.getBody().put(payload);
    
                if (cmp > 0) {
                    expect_frame();
                } else if (cmp == 0) {
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
