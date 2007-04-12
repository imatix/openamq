package org.openamq.nio;

import org.apache.mina.common.support.DelegatedIoConnector;
import org.apache.mina.transport.socket.nio.support.*;

public class SocketConnector extends DelegatedIoConnector
{
    /**
     * Creates a new instance.
     */
    public SocketConnector()
    {
        init(new SocketConnectorDelegate(this));
    }
}
