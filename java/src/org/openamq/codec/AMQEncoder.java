/*****************************************************************************
 * Filename    : AMQEncoder.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.codec;

import org.apache.mina.filter.codec.ProtocolEncoder;
import org.apache.mina.filter.codec.ProtocolEncoderOutput;
import org.apache.mina.common.IoSession;
import org.openamq.framing.AMQDataBlockEncoder;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQEncoder implements ProtocolEncoder
{
    private AMQDataBlockEncoder _dataBlockEncoder = new AMQDataBlockEncoder();

    public void encode(IoSession session, Object message, ProtocolEncoderOutput out) throws Exception
    {
        _dataBlockEncoder.encode(session, message, out);
    }

    public void dispose(IoSession session) throws Exception
    {

    }
}
