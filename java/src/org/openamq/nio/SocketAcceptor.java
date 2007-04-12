package org.openamq.nio;

import org.apache.mina.common.support.DelegatedIoAcceptor;

public class SocketAcceptor extends DelegatedIoAcceptor
{
    /**
     * Creates a new instance.
     */
    public SocketAcceptor()
    {
        init(new SocketAcceptorDelegate(this));
    }
}
