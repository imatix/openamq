package org.openamq.jms;

import javax.jms.ResourceAllocationException;

/**
 * Indicates that the maximum number of sessions per connection limit has been reached.
 */
public class ChannelLimitReachedException extends ResourceAllocationException
{
    private static final String ERROR_CODE = "1";

    private int _limit;

    public ChannelLimitReachedException(int limit)
    {
        super("Unable to create session since maximum number of sessions per connection is " +
              limit + ". Either close one or more sessions or increase the " +
              "maximum number of sessions per connection (or contact your OpenAMQ administrator.", ERROR_CODE);
        _limit = limit;
    }

    public int getLimit()
    {
        return _limit;
    }
}
