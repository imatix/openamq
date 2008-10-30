package org.openamq.jms;

import javax.jms.*;
import javax.jms.MessageConsumer;
import javax.jms.MessageProducer;


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
     * Create a producer
     * @param destination
     * @param mandatory the value of the mandatory flag used by default on the producer
     * @param immediate the value of the immediate flag used by default on the producer
     * @return
     * @throws JMSException
     */
    MessageProducer createProducer(Destination destination, boolean mandatory, boolean immediate)
            throws JMSException;

    /**
     * Create a producer
     * @param destination     
     * @param immediate the value of the immediate flag used by default on the producer
     * @return
     * @throws JMSException
     */
    MessageProducer createProducer(Destination destination, boolean immediate)
            throws JMSException;
}
