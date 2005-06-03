package org.openamq.rawclient;

import org.openamq.frames.AMQHandle;
import org.openamq.AMQFramingFactory;
import org.openamq.AMQException;
import org.apache.log4j.Logger;

/**
 * Used for writing messages to a specific queue.
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class QueueWriter
{
    private static final Logger _log = Logger.getLogger(QueueWriter.class);

    private Connection _connection;

    private String _name;

    private boolean _temporary;

    private int _handleId;

    /* Frame for handle send */
    private AMQHandle.Send _handleSend;

    QueueWriter(Connection con, String name, boolean temporary, int handleId)
            throws AMQClientException
    {
        if (_log.isDebugEnabled()) _log.debug("Create queue writer for queue " + name +
                                              " with handle id " + handleId);
        _connection = con;
        _name = name;
        _temporary = temporary;
        _handleId = handleId;

        try
        {
            _log.debug("Populating handle send frame");
            populateSendFrame(con.getFramingFactory());
        }
        catch (AMQException e)
        {
            throw new AMQClientException(_log, "Error create frame: " + e, e);
        }
    }

    public void write(byte[] message) throws AMQClientException
    {
        _log.debug("Sending byte message on queue " + _name + "(handle id " + _handleId + ")");
        _connection.sendMessage(new Message(message), _handleId, _name);
    }

    public void write(String message) throws AMQClientException
    {
        _log.debug("Sending string message on queue " + _name + "(handle id " + _handleId + ")");
        _connection.sendMessage(new Message(message), _handleId, _name);
    }

    public void write(Message m) throws AMQClientException
    {
        _log.debug("Sending message on queue " + _name + "(handle id " + _handleId + ")");
        _connection.sendMessage(m, _handleId, _name);
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

    private void populateSendFrame(AMQFramingFactory framingFactory) throws AMQException
    {
        _handleSend = (AMQHandle.Send)framingFactory.constructFrame(AMQHandle.SEND);
        _handleSend.handleId = 2;
        _handleSend.confirmTag = 0;
        _handleSend.fragmentSize = 0;
        _handleSend.partial = false;
        _handleSend.outOfBand = false;
        _handleSend.recovery = false;
        _handleSend.immediate = true;
        _handleSend.destName = "";
    }
}