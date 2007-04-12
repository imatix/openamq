package org.openamq.client;

import javax.jms.JMSException;
import javax.jms.TemporaryQueue;

/**
 * AMQ implementation of a TemporaryQueue.
 */
final class AMQTemporaryQueue extends AMQQueue implements TemporaryQueue {

    /**
     * Create a new instance of an AMQTemporaryQueue
     */
    public AMQTemporaryQueue() {
        super("TempQueue" + Long.toString(System.currentTimeMillis()),
                null, true, true);
    }

    /**
     * @see javax.jms.TemporaryQueue#delete()
     */
    public void delete() throws JMSException {
        throw new UnsupportedOperationException("Delete not supported, " +
            "will auto-delete when connection closed");
    }
    
}
