package org.openamq.client.handler;

import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.client.protocol.AMQMethodEvent;
import org.openamq.client.state.AMQStateManager;
import org.openamq.client.state.StateAwareMethodListener;
import org.openamq.framing.ChannelFlowOkBody;

public class ChannelFlowOkMethodHandler implements StateAwareMethodListener
{
     private static final Logger _logger = Logger.getLogger(ChannelFlowOkMethodHandler.class);
     private static final ChannelFlowOkMethodHandler _instance = new ChannelFlowOkMethodHandler();

     public static ChannelFlowOkMethodHandler getInstance()
     {
         return _instance;
     }

     private ChannelFlowOkMethodHandler()
     {
     }

     public void methodReceived(AMQStateManager stateManager, AMQMethodEvent evt) throws AMQException
     {
         ChannelFlowOkBody method = (ChannelFlowOkBody) evt.getMethod();
         _logger.debug("Received Channel.Flow-Ok message, active = " + method.active);
     }
}
