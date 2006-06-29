package org.openamq;

import org.openamq.framing.ChannelCloseBody;
import org.openamq.framing.AMQFrame;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class AMQChannelException extends AMQException
{
    private final int _classId;
    private final int _methodId;

    public AMQChannelException(int errorCode, String msg, int classId, int methodId, Throwable t)
    {
        super(errorCode, msg, t);
        _classId = classId;
        _methodId = methodId;
    }

    public AMQChannelException(int errorCode, String msg, int classId, int methodId)
    {
        super(errorCode, msg);
        _classId = classId;
        _methodId = methodId;
    }

    public AMQFrame getCloseFrame(int channel)
    {
        return ChannelCloseBody.createAMQFrame(channel, getErrorCode(), getMessage(), _classId, _methodId);
    }
}
