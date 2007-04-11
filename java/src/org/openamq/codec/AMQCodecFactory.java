/*****************************************************************************
 * Filename    : AMQCodecFactory.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.codec;

import org.apache.mina.filter.codec.ProtocolCodecFactory;
import org.apache.mina.filter.codec.ProtocolDecoder;
import org.apache.mina.filter.codec.ProtocolEncoder;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQCodecFactory implements ProtocolCodecFactory
{
    private AMQEncoder _encoder = new AMQEncoder();

    private AMQDecoder _frameDecoder;

    /**
     * @param expectProtocolInitiation true if the first frame received is going to be
     * a protocol initiation frame, false if it is going to be a standard AMQ data block.
     * The former case is used for the broker, which always expects to received the
     * protocol initiation first from a newly connected client.
     */
    public AMQCodecFactory(boolean expectProtocolInitiation)
    {
        _frameDecoder = new AMQDecoder(expectProtocolInitiation);
    }

    public ProtocolEncoder getEncoder()
    {
        return _encoder;
    }

    public ProtocolDecoder getDecoder()
    {
        return _frameDecoder;
    }
}
