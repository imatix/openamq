package org.openamq.rawclient.handler;

import org.openamq.frames.*;
import org.openamq.rawclient.*;
import org.openamq.AMQFramingFactory;
import org.openamq.AMQException;
import org.apache.log4j.Logger;

import java.util.Map;
import java.io.IOException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectedHandler
{
    private static final Logger _log = Logger.getLogger(Logger.class);

    /** ack frame is cached to avoid repeated construction */
    private static AMQChannel.Ack _channelAck;

    /**
     * Handle a frame and return a new state.
     * @param frame
     * @param sessionData
     * @return the new state
     * @throws AMQClientException
     */
    public static int handle(int state, AMQFrame frame, Map sessionData, AMQFramingFactory framingFactory)
            throws AMQClientException
    {
        if (_log.isDebugEnabled())
        {
            _log.debug("Connect handler called while in state " +  state + " with frame " +
                       String.valueOf(frame));
        }
        try
        {
            if (frame instanceof AMQConnection.Close)
            {
                return handleAMQConnectionClose(state, (AMQConnection.Close)frame, sessionData, framingFactory);
            }
            else if (frame instanceof AMQChannel.Close)
            {
                return handleAMQChannelClose(state, (AMQChannel.Close)frame, sessionData, framingFactory);
            }
            else if (frame instanceof AMQHandle.Notify)
            {
                return handleAMQHandleNotify(state, (AMQHandle.Notify)frame, sessionData, framingFactory);
            }
            else
            {
                return state;
            }
        }
        catch (AMQException e)
        {
            throw new AMQClientException(_log, "Error handling frame: " + e, e);
        }
        catch (IOException e)
        {
            throw new AMQClientException(_log, "IO Error handling frame: " + e, e);
        }
    }

    private static int handleAMQChannelClose(int state, AMQChannel.Close frame,
                                             Map sessionData, AMQFramingFactory framingFactory)
            throws AMQException, IOException
    {
        _log.info("Server closed channel with code " + frame.replyCode +
                  " and message:\n " + frame.replyText);
        AMQConnection.Close closeFrame = (AMQConnection.Close)framingFactory.constructFrame(AMQConnection.CLOSE);
        closeFrame.replyCode = 200;
        closeFrame.replyText = "amqpcli_serial.java: bye";
        framingFactory.sendFrame(closeFrame);
        closeFrame = (AMQConnection.Close)framingFactory.receiveFrame();
        _log.info("Server closed connection with code " + frame.replyCode +
                  " and message:\n " + frame.replyText);
        return AMQStates.CONNECTION_CLOSED;
    }

    private static int handleAMQConnectionClose(int state, AMQConnection.Close frame,
                                                Map sessionData, AMQFramingFactory framingFactory)
            throws AMQException, IOException
    {
        _log.info("Server closed connection with code " + frame.replyCode +
                  " and message:\n " + frame.replyText);
        return AMQStates.CONNECTION_CLOSED;
    }
    private static int handleAMQHandleNotify(int state, AMQHandle.Notify frame,
                                             Map sessionData, AMQFramingFactory framingFactory)
            throws AMQException, IOException, AMQClientException
    {
        AMQMessage.Head messageHead = framingFactory.receiveMessageHead();
        byte[] payload = new byte[(int)messageHead.bodySize];
        framingFactory.receiveData(payload);
        Message msg = new Message(payload);

        if (_channelAck == null)
        {
            _channelAck = (AMQChannel.Ack)framingFactory.constructFrame(AMQChannel.ACK);
        }
        // Acknowledge
        _channelAck.messageNbr = frame.messageNbr;
        framingFactory.sendFrame(_channelAck);

        notifyClientMessageHasArrived(msg, frame.handleId, (Map) sessionData.get(Connection.MESSAGE_CONSUMERS));
        return state;
    }

    private static void notifyClientMessageHasArrived(Message msg, int handleId, Map handleId2ConsumersMap)
            throws AMQClientException
    {
        final QueueReader qr = (QueueReader) handleId2ConsumersMap.get(new Integer(handleId));
        if (qr != null)
        {
            qr.fireCallback(msg);
        }
        else
        {
            throw new AMQClientException(_log, "No callback registered for message");
        }
    }
}
