package org.openamq.cluster;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQSession;
import org.openamq.client.AMQTopic;
import org.openamq.client.AMQQueue;
import org.openamq.AMQException;

import javax.jms.MessageListener;
import javax.jms.Message;
import javax.jms.Session;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.TextMessage;
import java.util.Random;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class Client
{
    private final Random random = new Random();
    private final String name;
    private final Session session;
    private final MessageProducer topicProducer;
    private final MessageProducer queueProducer;

    Client(AMQConnection connection, String name) throws JMSException, InterruptedException
    {
        this.name = name;
        session = connection.createSession(false, AMQSession.NO_ACKNOWLEDGE);

        AMQTopic topic = new AMQTopic("cluster_test_topic", true);
        AMQQueue queue = new AMQQueue("cluster_test_queue");

        topicProducer = session.createProducer(topic);
        queueProducer = session.createProducer(queue);

        //subscribe to a known topic
        session.createConsumer(topic).setMessageListener(new TopicHandler());
        //subscribe to a known queue
        session.createConsumer(queue).setMessageListener(new QueueHandler());

        connection.start();

        while(true)
        {
            Thread.sleep(random.nextInt(60000));
            sendToQueue(name + ":" + randomString(5));
        }
    }

    private synchronized void sendToTopic(String message) throws JMSException
    {
        topicProducer.send(session.createTextMessage(message));
    }

    private synchronized void sendToQueue(String message) throws JMSException
    {
        queueProducer.send(session.createTextMessage(message));
    }

    private String randomString(int length){
        char[] c = new char[length];
        for(int i = 0; i < length; i++)
        {
            c[i] = (char) ('A' + random.nextInt(26));
        }
        return new String(c);
    }

    private class QueueHandler implements MessageListener
    {
        public void onMessage(Message message)
        {
            try
            {
                sendToTopic(((TextMessage) message).getText());
            }
            catch (JMSException e)
            {
                e.printStackTrace();
            }
        }
    }

    private class TopicHandler implements MessageListener
    {
        public void onMessage(Message message)
        {
            try
            {
                System.out.println(((TextMessage) message).getText());
            }
            catch (JMSException e)
            {
                e.printStackTrace();
            }
        }
    }

    public static void main(String[] argv) throws AMQException, JMSException, InterruptedException
    {
        //assume args describe the set of brokers to try
        new Client(new AMQConnection(argv[0], "guest", "guest", argv[1], "/test"), argv[1]);
    }
}
