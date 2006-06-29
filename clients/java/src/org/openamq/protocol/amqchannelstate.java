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

public class amqchannelstate extends amqchannelstatei implements Runnable
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
    expect_external_event,
    channel_opened,
    channel_opening;

///////////////////////////   C O N T R U C T O R   ///////////////////////////

public amqchannelstate (AMQClientConnection acc, AMQClientSession acs)
{
    this.acc = acc;
    this.acs = acs;
    this.aph = acc.getProtocolHandler();
    frames = new LinkedList();
    frame = null;
    amb = null;
    message = null;
    expect_external_event = false;
    channel_opened = false;
    channel_opening = true;
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
            if (frame.bodyFrame instanceof AMQMethodBody) {
                amb = (AMQMethodBody)frame.bodyFrame;
    
                switch (amb.getId())
                {
                    case 2002:
                        the_next_event = channel_open_ok_event;
                        synchronized (this) {
                            channel_opened = true;
                            channel_opening = false;
                            notifyAll();
                        }
                        break;
                    case 2006:
                        the_next_event = channel_close_event;
                        break;
                    case 2007:
                        the_next_event = channel_finished_event;
                        break;
                    case 6006:
                        message = acs.createMessage();
                        message.setDelivery(amb);
                        the_next_event = basic_return_event;
                        break;
                    case 6007:
                        message = acs.createMessage();
                        message.setDelivery(amb);
                        the_next_event = basic_deliver_event;
                        break;
                    default:
                        _logger.debug("No special action for frame (at channel level): " + frame);
                        the_next_event = channel_open_ok_event;
                }
            } else if (frame.bodyFrame instanceof ContentHeaderBody) {
                the_next_event = content_header_event;
            } else if (frame.bodyFrame instanceof ContentBody) {
                the_next_event = content_body_event;
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

public void wait_channel_opened ()
{
    synchronized (this) {
        while (!channel_opened) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
    }
}

////////////////////////   CHECK FOR CHANNEL OPENED   /////////////////////////

public boolean is_channel_opened ()
{
    synchronized (this) {
        return channel_opened;
    }
}

////////////////////////   CHECK FOR CHANNEL OPENING   ////////////////////////

public boolean is_channel_opening ()
{
    synchronized (this) {
        return channel_opening;
    }
}

//%START MODULE

///////////////////////////////   CHANNEL OPEN   //////////////////////////////

public void channel_open ()
{
    try {
        aph.writeFrame(acs, ChannelOpenBody.createAMQFrame(acs.getSessionId(), null));
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


/////////////////////////////   CHANNEL CLOSE OK   ////////////////////////////

public void channel_close_ok ()
{
    try {
        aph.writeFrame(acs, ChannelCloseOkBody.createAMQFrame(acs.getSessionId()));
        the_next_event = channel_finished_event;
    } catch (Exception e) {
        throw new RuntimeException(e);
    }
}


/////////////////////////////////   CLEAN UP   ////////////////////////////////

public void clean_up ()
{
}


/////////////////////////////   DISPATCH MESSAGE   ////////////////////////////

public void dispatch_message ()
{
    acs.messageReceived(message);
    message = null;

    the_next_event = channel_open_ok_event;
}


//////////////////////////////   CONSUME HEADER   /////////////////////////////

public void consume_header ()
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

public void consume_body ()
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
                    the_next_event = message_consumed_event;
                }
            }
        }
    } catch (Exception e) {
        throw new RuntimeException(e);
    }

}


//%END MODULE
}
