/*****************************************************************************
 * Filename    : SocketConnector.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.nio;

import org.apache.mina.common.support.DelegatedIoConnector;
import org.apache.mina.transport.socket.nio.support.*;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
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