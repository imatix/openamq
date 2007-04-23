package org.openamq.pubsub1;

import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQTopic;
import org.openamq.jms.Session;

import javax.jms.MessageConsumer;
import javax.jms.MessageListener;
import javax.jms.Topic;
import java.net.InetAddress;

public class TestSubscriber
{
    private static final Logger _logger = Logger.getLogger(TestSubscriber.class);

    private static class TestMessageListener implements MessageListener
    {
        private String _name;

        private int _messageCount;

        private long _startTime = 0;

        public TestMessageListener(String name)
        {
            _name = name;
        }

        public void onMessage(javax.jms.Message message)
        {
            if (_messageCount++ == 0)
            {
                _startTime = System.currentTimeMillis();
            }
            if (_messageCount % 1000 == 0) {
                long totalTime = System.currentTimeMillis() - _startTime;
                _logger.info(_name + ": Total time to receive " + _messageCount + " messages was " +
                                totalTime + "ms.");
            }
        }
    }

    public static void main(String[] args)
    {
        _logger.info("Starting...");

        if (args.length != 6)
        {
            System.out.println("Usage: host port username password virtual-path selector");
            System.exit(1);
        }
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            AMQConnection con1 = new AMQConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                                  address.getHostName(), args[4]);
            final org.openamq.jms.Session session1 = (org.openamq.jms.Session) con1.createSession(false, Session.AUTO_ACKNOWLEDGE);

            AMQConnection con2 = new AMQConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                                  address.getHostName(), args[4]);
            final org.openamq.jms.Session session2 = (org.openamq.jms.Session) con2.createSession(false, Session.AUTO_ACKNOWLEDGE);
            String selector = args[5];

            _logger.info("Message selector is <" + selector + ">...");

            Topic t = new AMQTopic("pubsub", false);
            MessageConsumer consumer1 = session1.createConsumer(t,
                                                                100, false, false, selector);
            MessageConsumer consumer2 = session2.createConsumer(t,
                                                                100, false, false, selector);

            consumer1.setMessageListener(new TestMessageListener("Listener 1"));
            consumer2.setMessageListener(new TestMessageListener("Listener 2"));
            con1.start();
            con2.start();
        }
        catch (Throwable t)
        {
            System.err.println("Fatal error: " + t);
            t.printStackTrace();
        }

        System.out.println("Waiting...");
    }
}

