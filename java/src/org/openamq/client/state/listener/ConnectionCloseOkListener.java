package org.openamq.client.state.listener;

import org.openamq.AMQException;
import org.openamq.client.protocol.BlockingMethodFrameListener;
import org.openamq.framing.AMQMethodBody;
import org.openamq.framing.ConnectionCloseOkBody;

public class ConnectionCloseOkListener extends BlockingMethodFrameListener
{

    public ConnectionCloseOkListener()
    {
        super(0);
    }
    
    public boolean processMethod(int channelId, AMQMethodBody frame) throws AMQException
    {
        return (frame instanceof ConnectionCloseOkBody);
    }
}
