package org.openamq.rawclient;

import org.openamq.frames.*;
import org.openamq.AMQException;
import org.openamq.AMQFramingFactory;
import org.openamq.rawclient.handler.ConnectedHandler;
import org.apache.log4j.Logger;

import java.io.IOException;
import java.util.Map;
import java.util.HashMap;
import java.net.InetAddress;
import java.net.UnknownHostException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class Connection extends amqpcli_serial
{
    private static final Logger _log = Logger.getLogger(Connection.class);

    private String _host;

    private int _port;

    /**
     * Channel open command
     */
    private AMQChannel.Open _channelOpen;

    // AMQ Handle
    /* Handle open command */
    private AMQHandle.Open _handleOpen;

    /* Handle created reply */
    private AMQHandle.Created _handleCreated;

    /* Handle send command */
    private AMQHandle.Send _handleSend;

    /* Handle consume command */
    private AMQHandle.Consume _handleConsume;

    private boolean _ack = true;

    /**
     * The current state. This determines how we process incoming frames.
     */
    private int _currentState;

    /**
     * Store arbitrary data in the session. Passed in to frame handler routines.
     */
    private Map _sessionData = new HashMap();

    // Constants that are keys into the session data. Handles should always use these
    // constants

    public static final String MESSAGE_CONSUMERS = "MessageConsumers";

    /**
     * The channel id associated with this connection
     */
    private int _channelId;

    Connection(String host, int port, String username, String password)
            throws AMQClientException
    {
        arguments.put("opt_server", host);
        try
        {
            this.client_name = InetAddress.getLocalHost().getHostName();
            _log.info("Client name: " + this.client_name);
        }
        catch (UnknownHostException e)
        {
            throw new AMQClientException(_log, "Unable to obtain localhost information - oops: " + e, e);
        }
        _channelId = IdFactory.getInstance().getChannelId();
        super.setup();
        super.execute();
        _currentState = AMQStates.CONNECTED;
        try
        {
            constructFrames();
            openChannel();
        }
        catch (AMQException e)
        {
            throw new AMQClientException(_log, "Error constructing frames: " + e, e);
        }
    }

    /**
     * @return the framing factory used to send and receive frames.
     */
    AMQFramingFactory getFramingFactory()
    {
        return amq_framing;
    }

    /**
     * Open the channel with the server. Uses the channel id that was assigned on
     * construction on the connection.
     *
     * @throws AMQException
     * @throws AMQClientException
     */
    private void openChannel() throws AMQException, AMQClientException
    {
        _channelOpen.channelId = _channelId;
        _channelOpen.confirmTag = (_ack ? 1 : 0);
        _channelOpen.transacted = false;
        _channelOpen.restartable = false;
        _channelOpen.options = null;
        _channelOpen.outOfBand = "";

        try
        {
            getFramingFactory().sendFrame(_channelOpen);
            if (_ack)
            {
                AMQChannel.Reply ack = (AMQChannel.Reply) getFramingFactory().receiveFrame();
                if (_log.isDebugEnabled())
                {
                    _log.debug(String.valueOf(ack.toString()));
                }
            }
        }
        catch (IOException e)
        {
            throw new AMQClientException(_log, "Error sending frame: " + e, e);
        }
    }

    public QueueReader createQueueReader(String name, boolean temporary, int flags, MessageReceivedCallback callback)
            throws AMQClientException
    {
        if (_log.isDebugEnabled())
        {
            _log.debug("Creating queue reader with \nname: " + name + "\ntemporary: " + temporary +
                       "\nflags: " + flags);
        }
        final int handleId = IdFactory.getInstance().getHandleId();
        if (_log.isDebugEnabled())
        {
            _log.debug("Handle id is " + handleId);
        }
        String destName = openHandle(handleId, false, true, temporary, name);

        Map handle2ReaderMap = (Map) _sessionData.get(MESSAGE_CONSUMERS);
        if (handle2ReaderMap == null)
        {
            handle2ReaderMap = new HashMap();
            _sessionData.put(MESSAGE_CONSUMERS, handle2ReaderMap);
        }
        QueueReader qr = new QueueReader(this, destName, temporary, handleId, callback);
        handle2ReaderMap.put(new Integer(_handleOpen.handleId), qr);

        try
        {
            _handleConsume.handleId = handleId;
            getFramingFactory().sendFrame(_handleConsume);
        }
        catch (AMQException e)
        {
            throw new AMQClientException(_log, "Error sending consume frame: " + e, e);
        }
        catch (IOException e)
        {
            throw new AMQClientException(_log, "I/O Error sending consume frame: " + e, e);
        }

        return qr;
    }

    public QueueWriter createQueueWriter(String name, boolean temporary, int flags) throws AMQClientException
    {
        final int handleId = IdFactory.getInstance().getHandleId();
        final String destName = openHandle(handleId, true, false, temporary, name);
        return new QueueWriter(this, destName, temporary, handleId);
    }

    /**
     * Clients drive the reception of messages by calling this method
     */
    public void readMessage() throws AMQClientException
    {
        try
        {
            readFrame();
        }
        catch (AMQException e)
        {
            throw new AMQClientException(_log, "Error reading message: " + e, e);
        }
    }

    private void constructFrames() throws AMQException
    {
        _log.debug("Constructing frame cache");
        _channelOpen = (AMQChannel.Open) amq_framing.constructFrame(AMQChannel.OPEN);
        // Handle
        _handleOpen = (AMQHandle.Open) amq_framing.constructFrame(AMQHandle.OPEN);
        _handleSend = (AMQHandle.Send) amq_framing.constructFrame(AMQHandle.SEND);
        _handleSend.destName = "";
        _handleConsume = (AMQHandle.Consume) amq_framing.constructFrame(AMQHandle.CONSUME);
    }

    /**
     * @param handleId
     * @param producer
     * @param consumer
     * @param temporary
     * @param destName
     * @return the destination name (useful when the server is choosing for you!)
     * @throws AMQClientException
     */
    private String openHandle(int handleId, boolean producer, boolean consumer,
                            boolean temporary, String destName)
            throws AMQClientException
    {
        if (_log.isDebugEnabled())
        {
            _log.debug("Opening handle with id " + handleId);
        }
        _handleOpen.handleId = handleId;
        _handleOpen.producer = producer;
        _handleOpen.consumer = consumer;
        _handleOpen.temporary = temporary;
        _handleOpen.destName = destName;
        if (temporary && !"".equals(destName))
        {
            destName = "";
        }

        _handleOpen.channelId = _channelId;
        _handleOpen.serviceType = (_ack ? 1 : 0);
        _handleOpen.confirmTag = 1;
        _handleOpen.browser = false;
        _handleOpen.mimeType = "";
        _handleOpen.encoding = "";
        _handleOpen.options = null;

        try
        {
            if (_log.isDebugEnabled())
            {
                _log.debug("Sending handle open frame to server for handle id " + handleId);
            }
            getFramingFactory().sendFrame(_handleOpen);

            // There is currently a bug when trying to create a temp queue that happens
            // to already exist. A CREATED is not sent back.
            // So for the time being we will always do sync and read'n'discard until
            // we see the ACK.
//            if (temporary)
//            {
//	            if (_log.isDebugEnabled()) _log.debug("Temp queue, waiting for 'created' from server for handle id " + handleId);
//                // Get handle created
//                // TODO: this should invoke receiveFrame instead to centralise frame
//                // handling
//                AMQFrame frame = getFramingFactory().receiveFrame();
//
//		    if (_log.isDebugEnabled())
//                {
//                    _log.debug(String.valueOf(frame));
//                }
//
//            }
            if (_ack)
            {
                AMQFrame frame = null;

                do
                {
                    if (_log.isDebugEnabled())
                    {
                        _log.debug("Sync, waiting for 'ack' from server for handle id " + handleId);
                    }

                    frame = getFramingFactory().receiveFrame();

                    if (frame instanceof AMQHandle.Created)
                    {
                        AMQHandle.Created createdHandle =(AMQHandle.Created)frame;
                        destName = createdHandle.destName;
                    }
                    if (_log.isDebugEnabled())
                    {
                        _log.debug(String.valueOf(frame));
                    }
                }
                while (!(frame instanceof AMQHandle.Reply));

                AMQHandle.Reply ack = (AMQHandle.Reply) frame;
                if (_log.isDebugEnabled())
                {
                    _log.debug(String.valueOf(ack));
                }
            }

            if (_log.isDebugEnabled())
            {
                _log.debug("Done creating queue with handle id " + handleId);
            }
            return destName;
        }
        catch (AMQException e)
        {
            throw new AMQClientException(_log, "Error receiving frame: " + e, e);
        }
        catch (IOException e)
        {
            throw new AMQClientException(_log, "I/O error receiving frame: " + e, e);
        }
    }

    private void readFrame() throws AMQException, AMQClientException
    {
        if (_currentState == AMQStates.CONNECTION_CLOSED)
        {
            throw new AMQClientException(_log, "Illegal call to readFrame while in connection closed state");
        }
        else
        {
            AMQFrame frame;
            // Get frame
            try
            {
                frame = getFramingFactory().receiveFrame();
                _log.debug("Received a frame from server.");
            }
            catch (IOException e)
            {
                throw new AMQClientException(_log, "Error reading frame: " + e, e);
            }

            switch (_currentState)
            {
                case AMQStates.CONNECTED:
                    _currentState = ConnectedHandler.handle(_currentState, frame,
                                                            _sessionData,
                                                            getFramingFactory());
                    break;
                default:
                    throw new AMQClientException(_log, "Fatal internal error: unknown internal state  " + _currentState);
            }
        }
    }

    /**
     * Internal implementation method. Called by QueueWriter to do the mechanics of
     * sending a message.
     *
     * @param m        the message
     * @param handleId
     * @throws AMQClientException
     */
    void sendMessage(Message m, int handleId) throws AMQClientException
    {
        byte[] messageBody = m.getBytes();

        AMQMessage.Head messageHead;
        try
        {
            messageHead = getFramingFactory().constructMessageHead();
        }
        catch (AMQException e)
        {
            throw new AMQClientException(_log, "Error constructing message head: " + e, e);
        }

        messageHead.bodySize = messageBody.length;
        if (_log.isDebugEnabled())
        {
            _log.debug("Message body length; " + messageBody.length);
        }
        // Set the fragment size
        _handleSend.fragmentSize = messageHead.size() + messageHead.bodySize;
        if (_handleSend.fragmentSize <= amq_framing.getFrameMax())
        {
            try
            {
                // Send message
                _handleSend.handleId = handleId;
                getFramingFactory().sendFrame(_handleSend);
                getFramingFactory().sendMessageHead(messageHead);
                getFramingFactory().sendData(messageBody);
                getFramingFactory().flush();
            }
            catch (IOException e)
            {
                throw new AMQClientException(_log, "I/O error sending frame: " + e, e);
            }
            catch (AMQException e)
            {
                throw new AMQClientException(_log, "Error sending frame: " + e, e);
            }
        }
    }

    public void terminate_the_program()
    {
        // DO NOTHING. We override this since the base class will shut down the client
        // if we don't stop it!
    }

    public String getHost()
    {
        return _host;
    }

    public void setHost(String host)
    {
        _host = host;
    }

    public int getPort()
    {
        return _port;
    }

    public void setPort(int port)
    {
        _port = port;
    }
}
