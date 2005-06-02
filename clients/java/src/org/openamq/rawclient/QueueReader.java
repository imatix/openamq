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

    private boolean _temporary;

    private int _handleId;

    private AMQHandle.Consume _handleConsume;

    private MessageReceivedCallback _callback;

    QueueReader(Connection con, String name, boolean temporary, int handleId,
                MessageReceivedCallback callback)
            throws AMQClientException
    {
        if (_log.isDebugEnabled()) _log.debug("Creating QueueReader for queue " + name + " with handle id " + handleId);
        _connection = con;
        _name = name;
        _temporary = temporary;
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

    public boolean isTemporary()
    {
        return _temporary;
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
        _handleConsume.handleId = 1;
        _handleConsume.confirmTag = 0;
        _handleConsume.prefetch = 1;
        _handleConsume.noLocal = true;
        _handleConsume.noAck = false;
        _handleConsume.dynamic = true;
        _handleConsume.destName = "";
        _handleConsume.selector = null;
    }
}
