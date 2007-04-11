package org.openamq.ack;

import junit.framework.JUnit4TestAdapter;
import org.apache.log4j.Logger;
import org.apache.log4j.xml.DOMConfigurator;
import org.junit.Assert;
import org.junit.Test;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQQueue;
import org.openamq.vmbroker.VmPipeBroker;

import javax.jms.MessageConsumer;
import javax.jms.MessageProducer;
import javax.jms.Session;
import javax.jms.TextMessage;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class RecoverTest extends VmPipeBroker
{
    private static final Logger _logger = Logger.getLogger(RecoverTest.class);

    static
    {
        DOMConfigurator.configure("../etc/log4j.xml");
    }

    @Test
    public void recoverResendsMsgs() throws Exception
    {
        AMQConnection con = new AMQConnection("foo:123", 1, "guest", "guest", "consumer1", "/test");

        Session consumerSession = con.createSession(false, Session.CLIENT_ACKNOWLEDGE);
        AMQQueue queue = new AMQQueue("someQ", "someQ", false, true);
        MessageConsumer consumer = consumerSession.createConsumer(queue);

        AMQConnection con2 = new AMQConnection("bar:123", 2, "guest", "guest", "producer1", "/test");
        Session producerSession = con2.createSession(false, Session.CLIENT_ACKNOWLEDGE);
        MessageProducer producer = producerSession.createProducer(queue);

        _logger.info("Sending four messages");
        producer.send(producerSession.createTextMessage("msg1"));
        producer.send(producerSession.createTextMessage("msg2"));
        producer.send(producerSession.createTextMessage("msg3"));
        producer.send(producerSession.createTextMessage("msg4"));

        _logger.info("Starting connection");
        con.start();
        TextMessage tm = (TextMessage) consumer.receive();
        tm.acknowledge();
        _logger.info("Received and acknowledged first message");
        consumer.receive();
        consumer.receive();
        consumer.receive();
        _logger.info("Received all four messages. Calling recover with three outstanding messages");
        // no ack for last three messages so when I call recover I expect to get three messages back
        consumerSession.recover();
        tm = (TextMessage) consumer.receive(3000);
        Assert.assertEquals(tm.getText(), "msg2");

        tm = (TextMessage) consumer.receive(3000);
        Assert.assertEquals(tm.getText(), "msg3");

        tm = (TextMessage) consumer.receive(3000);
        Assert.assertEquals(tm.getText(), "msg4");

        _logger.info("Received redelivery of three messages. Acknowledging last message");
        tm.acknowledge();

        _logger.info("Calling acknowledge with no outstanding messages");
        // all acked so no messages to be delivered
        consumerSession.recover();

        tm = (TextMessage) consumer.receiveNoWait();
        Assert.assertNull(tm);
        _logger.info("No messages redelivered as is expected");
    }

    public static junit.framework.Test suite()
    {
        return new JUnit4TestAdapter(RecoverTest.class);
    }
}

