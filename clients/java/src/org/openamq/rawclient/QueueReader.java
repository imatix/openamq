package org.openamq.rawclient;

import org.openamq.frames.AMQHandle;
import org.openamq.AMQFramingFactory;
import org.openamq.AMQException;
import org.apache.log4j.Logger;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class QueueReader
{
    private static final Logger _log = Logger.getLogger(QueueReader.class);

    private Connection _connection;

    private String _name;

    private boolean _exclusive;

    private boolean _dynamic;

    private int _handleId;

    private AMQHandle.Consume _handleConsume;

    private MessageReceivedCallback _callback;

    /**
     *
     * @param con
     * @param name
     * @param exclusive if true, only a single reader is allowed to consume
     * messages from the queue
     * @param dynamic if true, the queue will be created if it does not already exist and also
     * will be destroyed when the last consumer detaches
     * @param handleId
     * @param callback
     * @throws AMQClientException
     */
    QueueReader(Connection con, String name, boolean exclusive, boolean dynamic, int handleId,
                MessageReceivedCallback callback)
            throws AMQClientException
    {
        if (_log.isDebugEnabled()) _log.debug("Creating QueueReader for queue " + name + " with handle id " + handleId);
        _connection = con;
        _name = name;
        _exclusive = exclusive;
        _dynamic = dynamic;
        _handleId = handleId;
        _callback = callback;
        try
        {
            // we cache a frame to avoid the overhead of creating it every time
            populateConsumeFrame(con.getFramingFactory());
        }
        catch (AMQException e)
        {
            throw new AMQClientException(_log, "Error creating frame: " + e, e);
        }

    }

    public Connection getConnection()
    {
        return _connection;
    }

    public String getName()
    {
        return _name;
    }

    public boolean isExclusive()
    {
        return _exclusive;
    }

    public boolean isDynamic()
    {
        return _dynamic;
    }

    public void fireCallback(Message msg) throws AMQClientException
    {
        if (_callback != null)
        {
            if (_log.isDebugEnabled()) _log.debug("Calling callback routine for message on queue with handle id " +
                                                  _handleId);
            _callback.onMessage(msg);
        }
    }

    private void populateConsumeFrame(AMQFramingFactory framingFactory) throws AMQException
    {
        _handleConsume = (AMQHandle.Consume)framingFactory.constructFrame(AMQHandle.CONSUME);
        _handleConsume.handleId = _handleId;
        _handleConsume.confirmTag = 0;
        _handleConsume.prefetch = 1;
        _handleConsume.noLocal = true;
        _handleConsume.noAck = false;
        _handleConsume.dynamic = _dynamic;
        _handleConsume.destName = "";
        _handleConsume.selector = null;
    }
}
