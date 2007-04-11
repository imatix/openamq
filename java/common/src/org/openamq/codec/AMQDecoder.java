/*****************************************************************************
 * Filename    : AMQDecoder.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.codec;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.CumulativeProtocolDecoder;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;
import org.openamq.framing.AMQDataBlockDecoder;
import org.openamq.framing.ProtocolInitiation;

/**
 * There is one instance of this class per session. Any changes or configuration done
 * at run time to the encoders or decoders only affects decoding/encoding of the
 * protocol session data to which is it bound.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQDecoder extends CumulativeProtocolDecoder
{
    private AMQDataBlockDecoder _dataBlockDecoder = new AMQDataBlockDecoder();

    private ProtocolInitiation.Decoder _piDecoder = new ProtocolInitiation.Decoder();

    private boolean _expectProtocolInitiation;

    public AMQDecoder(boolean expectProtocolInitiation)
    {
        _expectProtocolInitiation = expectProtocolInitiation;
    }

    protected boolean doDecode(IoSession session, ByteBuffer in, ProtocolDecoderOutput out) throws Exception
    {
        if (_expectProtocolInitiation)
        {
            return doDecodePI(session, in, out);
        }
        else
        {
            return doDecodeDataBlock(session, in, out);
        }
    }

    protected boolean doDecodeDataBlock(IoSession session, ByteBuffer in, ProtocolDecoderOutput out) throws Exception
    {
        int pos = in.position();
        boolean enoughData = _dataBlockDecoder.decodable(session, in);
        in.position(pos);
        if (!enoughData)
        {
            // returning false means it will leave the contents in the buffer and
            // call us again when more data has been read
            return false;
        }
        else
        {
            _dataBlockDecoder.decode(session, in, out);
            return true;
        }
    }

    private boolean doDecodePI(IoSession session, ByteBuffer in, ProtocolDecoderOutput out) throws Exception
    {
        boolean enoughData = _piDecoder.decodable(session, in);
        if (!enoughData)
        {
            // returning false means it will leave the contents in the buffer and
            // call us again when more data has been read
            return false;
        }
        else
        {
            _piDecoder.decode(session, in, out);
            return true;
        }
    }

    public void setExpectProtocolInitiation(boolean expectProtocolInitiation)
    {
        _expectProtocolInitiation = expectProtocolInitiation;
    }
}
