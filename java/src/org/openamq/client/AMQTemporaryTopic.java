package org.openamq.client;

import javax.jms.JMSException;
import javax.jms.TemporaryTopic;

/**
 * AMQ implementation of TemporaryTopic.
 */
class AMQTemporaryTopic extends AMQTopic implements TemporaryTopic {

    /**
     * Create new temporary topic.
     */
    public AMQTemporaryTopic() {
        super("TempQueue" + Long.toString(System.currentTimeMillis()), true);
    }

    /**
     * @see javax.jms.TemporaryTopic#delete()
     */
    public void delete() throws JMSException {
        throw new UnsupportedOperationException("Delete not supported, " +
                "will auto-delete when connection closed");
    }

}
