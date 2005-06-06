package org.openamq.client.protocol;

import org.apache.mina.common.Session;
import org.openamq.client.AMQException;
import org.openamq.client.ConnectionTuneParameters;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.FieldTable;
import org.openamq.client.state.*;

import java.util.StringTokenizer;
import java.util.Iterator;
import java.util.Map;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ConnectionTuneHandler implements StateTransitionHandler
{
    public StateAndFrame messageReceived(AMQState state, Object message, Session session) throws AMQException
    {
        Connection.Tune frame = (Connection.Tune) message;

        ConnectionTuneParameters params = (ConnectionTuneParameters) session.getAttribute(ProtocolSessionAttributes.
                                                                                          CONNECTION_TUNE_PARAMETERS);
        if (params == null)
        {
            params = new ConnectionTuneParameters();
            session.setAttribute(ProtocolSessionAttributes.CONNECTION_TUNE_PARAMETERS, params);
        }

        if (frame.options != null)
        {
            Iterator it = frame.options.entrySet().iterator();
            while (it.hasNext())
            {
                Map.Entry entry = (Map.Entry) it.next();

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
        openFrame.clientName = (String) session.getAttribute(ProtocolSessionAttributes.CLIENT_NAME);
        openFrame.virtualPath = (String) session.getAttribute(ProtocolSessionAttributes.VIRTUAL_PATH);
        return new StateAndFrame(AMQState.CONNECTION_TUNED, openFrame);
    }
}
