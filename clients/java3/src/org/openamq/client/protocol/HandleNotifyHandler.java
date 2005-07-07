package org.openamq.client.protocol;

import org.openamq.client.*;
import org.openamq.client.state.*;
import org.openamq.client.framing.*;
import org.openamq.client.message.UnprocessedMessage;

import org.apache.log4j.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class HandleNotifyHandler implements StateAwareFrameListener
{
    private static final Logger _logger = Logger.getLogger(HandleNotifyHandler.class);

    private static final HandleNotifyHandler _instance = new HandleNotifyHandler();

    public static HandleNotifyHandler getInstance()
    {
        return _instance;
    }

    public void frameReceived(AMQStateManager stateManager, FrameEvent evt) throws AMQException
    {
        final Handle.Notify frame = (Handle.Notify) evt.getFrame();
        final AMQProtocolSession session = evt.getProtocolSession();

        //
        // We deliberately do not decode the message contents here since we want
        // to do as little as possible in the MINA dispatcher thread. By handing the
        // decoding to the session thread we maximise throughput (several sessions may
        // be served by this connection)
        //
        UnprocessedMessage msg = new UnprocessedMessage();
        msg.message = frame.messageFragment;
        msg.messageNbr = frame.messageNbr;
        msg.redelivered = frame.redelivered;
        msg.handleId = frame.handleId;
        if (_logger.isDebugEnabled())
        {
            _logger.debug("Calling protocol session to indicate message has been received");
        }
        session.messageReceived(msg);
    }
}
