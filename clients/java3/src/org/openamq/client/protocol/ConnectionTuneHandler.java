package org.openamq.client.protocol;

import org.openamq.client.AMQException;
import org.openamq.client.ConnectionTuneParameters;
import org.openamq.client.framing.Connection;
import org.openamq.client.state.AMQState;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareFrameListener;
import org.apache.log4j.Logger;

import java.util.Iterator;
import java.util.Map;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionTuneHandler implements StateAwareFrameListener
{
    private static final Logger _logger = Logger.getLogger(ConnectionTuneHandler.class);

    private static final ConnectionTuneHandler _instance = new ConnectionTuneHandler();

    public static ConnectionTuneHandler getInstance()
    {
        return _instance;
    }

    private ConnectionTuneHandler()
    {
    }

    public void frameReceived(AMQStateManager stateManager, FrameEvent evt) throws AMQException
    {
        _logger.debug("ConnectionTune frame received");
        Connection.Tune frame = (Connection.Tune) evt.getFrame();
        AMQProtocolSession session = evt.getProtocolSession();
        ConnectionTuneParameters params = session.getConnectionTuneParameters();
        if (params == null)
        {
            params = new ConnectionTuneParameters();
            session.setConnectionTuneParameters(params);
        }

        if (frame.options != null)
        {
            final Iterator it = frame.options.entrySet().iterator();
            while (it.hasNext())
            {
                final Map.Entry entry = (Map.Entry) it.next();

                if ("FRAME_MAX".equals(entry.getKey()))
                {
                    params.setFrameMax(((Long)entry.getValue()).longValue());
                }
                else if ("CHANNEL_MAX".equals(entry.getKey()))
                {
                    params.setChannelMax(((Long)entry.getValue()).longValue());
                }
                else if ("HANDLE_MAX".equals(entry.getKey()))
                {
                    params.setHandleMax(((Long)entry.getValue()).longValue());
                }
                else if ("HEARTBEAT".equals(entry.getKey()))
                {
                    params.setHearbeat(((Long)entry.getValue()).longValue());
                }
                else if ("TXN_LIMIT".equals(entry.getKey()))
                {
                    params.setTxnLimit(((Long)entry.getValue()).longValue());
                }
                else
                {
                    throw new AMQException("Unknown tune option: " + entry.getKey());
                }
            }
        }
        final Connection.Open openFrame = new Connection.Open();
        openFrame.clientName = session.getClientID();
        openFrame.virtualPath = session.getVirtualPath();
        // we must write the frame before changing state otherwise another thread
        // could conceivably send a frame before the Connection.Open frame has been
        // sent.
        session.writeFrame(openFrame);
        stateManager.changeState(AMQState.CONNECTION_OPEN);
    }
}
