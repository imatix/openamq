package org.openamq.jms;

import javax.jms.*;
import javax.jms.MessageConsumer;


public interface Session extends javax.jms.Session
{
    /**
     * Indicates that no client acknowledgements are required. Broker assumes that once it has delivered
     * a message packet successfully it is acknowledged.
     */
    static final int NO_ACKNOWLEDGE = 257;

    /**
     * Pre acknowledge means that an ack is sent per message but sent before user code has processed
     * the message (i.e. before the onMessage() call or the receive() method has returned).
     */
    static final int PRE_ACKNOWLEDGE = 258;

    MessageConsumer createConsumer(Destination destination,
                                   int prefetch,
                                   boolean noLocal,
                                   boolean exclusive,
                                   String selector) throws JMSException;

    /**
     * @return the prefetch value used by default for consumers created on this session.
     */
    int getDefaultPrefetch();

    /**
     * @param defaultPrefetch the prefetch value used by default for consumers created on this session.
     */
    void setDefaultPrefetch(int defaultPrefetch);
}
