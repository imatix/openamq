/*****************************************************************************
 * Filename    : SocketAcceptor.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.nio;

import org.apache.mina.common.support.DelegatedIoAcceptor;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
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
