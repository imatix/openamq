package org.openamq.client.protocol;

import org.apache.mina.common.Session;
import org.openamq.client.*;
import org.openamq.client.ConnectionTuneParameters;
import org.openamq.client.framing.Connection;
import org.openamq.client.framing.FieldTable;
import org.openamq.client.state.*;
import org.openamq.client.framing.*;

import java.util.StringTokenizer;
import java.util.Iterator;
import java.util.Map;

import org.apache.log4j.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class HandleNotifyHandler implements StateTransitionHandler
{
	public static final Logger _logger = Logger.getLogger(HandleNotifyHandler.class);
	
    public StateAndFrame messageReceived(AMQState state, Object message, Session session) throws AMQException
    {
        Handle.Notify frame = (Handle.Notify)message;

        Map map = (Map)session.getAttribute(ProtocolSessionAttributes.AMQ_HANDLE_TO_SESSION_MAP);
        
        AMQSession amqSession = (AMQSession)map.get(new Integer(frame.handleId));
        
        if (amqSession == null)
        {
        	_logger.warn("Received message for handle id " + frame.handleId + " - ignoring...");
        }
        else
        {
        	amqSession.notifyHandle(frame.handleId,frame.messageFragment);
        }
        
        return new StateAndFrame(state, null);
    }
}
