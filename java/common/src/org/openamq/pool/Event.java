package org.openamq.pool;

import org.apache.log4j.Logger;
import org.apache.mina.common.IoFilter;
import org.apache.mina.common.IoSession;
import org.apache.mina.common.IdleStatus;

/**
 * Represents an operation on IoFilter.
 */
enum EventType
{
    OPENED, CLOSED, READ, WRITE, WRITTEN, RECEIVED, SENT, IDLE, EXCEPTION
}

class Event
{
    private static final Logger _log = Logger.getLogger(Event.class);

    private final EventType type;
    private final IoFilter.NextFilter nextFilter;
    private final Object data;

    public Event(IoFilter.NextFilter nextFilter, EventType type, Object data)
    {
        this.type = type;
        this.nextFilter = nextFilter;
        this.data = data;
        if (type == EventType.EXCEPTION)
        {
            _log.error("Exception event constructed: " + data, (Exception) data);
        }
    }

    public Object getData()
    {
        return data;
    }


    public IoFilter.NextFilter getNextFilter()
    {
        return nextFilter;
    }


    public EventType getType()
    {
        return type;
    }

    void process(IoSession session)
    {
        if (_log.isDebugEnabled())
        {
            _log.debug("Processing " + this);
        }
        if (type == EventType.RECEIVED)
        {
            nextFilter.messageReceived(session, data);
            //ByteBufferUtil.releaseIfPossible( data );
        }
        else if (type == EventType.SENT)
        {
            nextFilter.messageSent(session, data);
            //ByteBufferUtil.releaseIfPossible( data );
        }
        else if (type == EventType.EXCEPTION)
        {
            nextFilter.exceptionCaught(session, (Throwable) data);
        }
        else if (type == EventType.IDLE)
        {
            nextFilter.sessionIdle(session, (IdleStatus) data);
        }
        else if (type == EventType.OPENED)
        {
            nextFilter.sessionOpened(session);
        }
        else if (type == EventType.WRITE)
        {
            nextFilter.filterWrite(session, (IoFilter.WriteRequest) data);
        }
        else if (type == EventType.CLOSED)
        {
            nextFilter.sessionClosed(session);
        }
    }

    public String toString()
    {
        return "Event: type " + type +  ", data: " + data;
    }
}
