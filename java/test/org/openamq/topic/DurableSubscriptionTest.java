package org.openamq.topic;

import junit.framework.JUnit4TestAdapter;
import org.junit.Assert;
import org.junit.Test;
import org.openamq.AMQException;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQSession;
import org.openamq.client.AMQTopic;
import org.openamq.vmbroker.VmPipeBroker;

import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.MessageProducer;
import javax.jms.Session;
import javax.jms.TextMessage;
import javax.jms.TopicSubscriber;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class DurableSubscriptionTest extends VmPipeBroker
{
    @Test
    public void unsubscribe() throws AMQException, JMSException
    {
        AMQTopic topic = new AMQTopic("MyTopic");
        AMQConnection con = new AMQConnection("invm:0000", "guest", "guest", "test", "/test");
        Session session1 = con.createSession(false, AMQSession.NO_ACKNOWLEDGE);
        MessageConsumer consumer1 = session1.createConsumer(topic);
        MessageProducer producer = session1.createProducer(topic);

        Session session2 = con.createSession(false, AMQSession.NO_ACKNOWLEDGE);
        TopicSubscriber consumer2 = session2.createDurableSubscriber(topic, "MySubscription");

        con.start();

        producer.send(session1.createTextMessage("A"));

        Message msg;
        msg = consumer1.receive();
        Assert.assertEquals("A", ((TextMessage) msg).getText());
        msg = consumer1.receive(1000);
        Assert.assertEquals(null, msg);

        msg = consumer2.receive();
        Assert.assertEquals("A", ((TextMessage) msg).getText());
        msg = consumer2.receive(1000);
        Assert.assertEquals(null, msg);

        session2.unsubscribe("MySubscription");

        producer.send(session1.createTextMessage("B"));

        msg = consumer1.receive();
        Assert.assertEquals("B", ((TextMessage) msg).getText());
        msg = consumer1.receive(1000);
        Assert.assertEquals(null, msg);

        msg = consumer2.receive(1000);
        Assert.assertEquals(null, msg);

        con.close();
    }

    @Test
    public void durability() throws AMQException, JMSException
    {
        AMQTopic topic = new AMQTopic("MyTopic");
        AMQConnection con = new AMQConnection("invm:0000", "guest", "guest", "test", "/test");
        Session session1 = con.createSession(false, AMQSession.NO_ACKNOWLEDGE);
        MessageConsumer consumer1 = session1.createConsumer(topic);
        MessageProducer producer = session1.createProducer(topic);

        Session session2 = con.createSession(false, AMQSession.NO_ACKNOWLEDGE);
        TopicSubscriber consumer2 = session2.createDurableSubscriber(topic, "MySubscription");

        con.start();

        producer.send(session1.createTextMessage("A"));

        Message msg;
        msg = consumer1.receive();
        Assert.assertEquals("A", ((TextMessage) msg).getText());
        msg = consumer1.receive(1000);
        Assert.assertEquals(null, msg);

        msg = consumer2.receive();
        Assert.assertEquals("A", ((TextMessage) msg).getText());
        msg = consumer2.receive(1000);
        Assert.assertEquals(null, msg);

        consumer2.close();

        Session session3 = con.createSession(false, AMQSession.NO_ACKNOWLEDGE);
        MessageConsumer consumer3 = session3.createDurableSubscriber(topic, "MySubscription");

        producer.send(session1.createTextMessage("B"));

        msg = consumer1.receive();
        Assert.assertEquals("B", ((TextMessage) msg).getText());
        msg = consumer1.receive(1000);
        Assert.assertEquals(null, msg);

        msg = consumer3.receive();
        Assert.assertEquals("B", ((TextMessage) msg).getText());
        msg = consumer3.receive(1000);
        Assert.assertEquals(null, msg);

        con.close();
    }

    public static junit.framework.Test suite()
    {
        return new JUnit4TestAdapter(DurableSubscriptionTest.class);
    }

}
