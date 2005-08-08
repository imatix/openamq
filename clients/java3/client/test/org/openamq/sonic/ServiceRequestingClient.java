package org.openamq.sonic;

import org.apache.log4j.Logger;

import javax.jms.*;

/**
 * Created by IntelliJ IDEA.
 * User: U806869
 * Date: 28-May-2005
 * Time: 21:54:51
 * To change this template use File | Settings | File Templates.
 */
public class ServiceRequestingClient
{
    private static final Logger _log = Logger.getLogger(ServiceRequestingClient.class);

    private static class CallbackHandler implements MessageListener
    {
        private int _expectedMessageCount;

        private int _actualMessageCount;

        private long _startTime;

        private long _averageLatency;

        public CallbackHandler(int expectedMessageCount, long startTime)
        {
            _expectedMessageCount = expectedMessageCount;
            _startTime = startTime;
        }

        public void onMessage(Message m)
        {
            if (_log.isDebugEnabled())
            {
                _log.debug("Message received: " + m);
            }
            try
            {
                if (m.propertyExists("timeSent"))
                {
                    long timeSent = m.getLongProperty("timeSent");
                    long now = System.currentTimeMillis();
                    if (_averageLatency == 0)
                    {
                        _averageLatency = now - timeSent;
                        _log.info("Latency " + _averageLatency);
                    }
                    else
                    {
                        _log.info("Individual latency: " + (now-timeSent));
                        _averageLatency = (_averageLatency + (now - timeSent))/2;
                        _log.info("Average latency now: " + _averageLatency);
                    }
                }
            }
            catch (JMSException e)
            {
                _log.error("Could not calculate latency");
            }

            _actualMessageCount++;
            if (_actualMessageCount%1000 == 0)
            {
                try
                {
                    m.acknowledge();
                }
                catch (JMSException e)
                {
                    _log.error("Error acknowledging message");
                }
                _log.info("Received message count: " + _actualMessageCount);

            }
            /*if (!"henson".equals(m.toString()))
           {
               _log.error("Message response not correct: expected 'henson' but got " + m.toString());
           }
           else
           {
               if (_log.isDebugEnabled())
               {
                   _log.debug("Message " + m + " received");
               }
               else
               {
                   _log.info("Message received");
               }
           } */

            if (_actualMessageCount == _expectedMessageCount)
            {
                long timeTaken = System.currentTimeMillis() - _startTime;
                System.out.println("Total time taken to receive " + _expectedMessageCount+ " messages was " +
                                   timeTaken + "ms, equivalent to " +
                                   (_expectedMessageCount/(timeTaken/1000.0)) + " messages per second");
                System.out.println("Average latency is: " + _averageLatency);
            }
        }
    }

    public static void main(String[] args) throws Exception
    {
        if (args.length != 3)
        {
            System.out.println("Usage: ServiceRequestingClient <Sonic Broker Host:Port> <serviceQueue> <count> will publish count messages to ");
            System.out.println("queue sendQueue and waits for a response on a temp queue");
            System.exit(1);
        }

        String url = args[0];
        String sendQueue = args[1];
        int messageCount = Integer.parseInt(args[2]);

        progress.message.jclient.ConnectionFactory qconFactory = new progress.message.jclient.ConnectionFactory(url);
        qconFactory.setLoadBalancing(false);
        Connection qcon = qconFactory.createConnection();
        Connection qcon2 = qconFactory.createConnection();

        Session qsession = qcon.createSession(false, Session.CLIENT_ACKNOWLEDGE);
        ((progress.message.jclient.Session)qsession).setFlowControlDisabled(true);
        Session qsession2 = qcon2.createSession(false, Session.CLIENT_ACKNOWLEDGE);
        ((progress.message.jclient.Session)qsession2).setFlowControlDisabled(true);
        //Queue sendQ = qsession.createQueue(sendQueue);
        Queue sendQ = qsession.createQueue(sendQueue);
        //Queue receiveQ = qsession2.createTemporaryQueue();
        Queue receiveQ = qsession.createQueue("serviceResponseQ");
        MessageProducer qsender = qsession.createProducer(sendQ);
        qsender.setDisableMessageTimestamp(true);
        qsender.setDeliveryMode(progress.message.jclient.DeliveryMode.NON_PERSISTENT_ASYNC);
        _log.debug("Queue sender created for service queue " + sendQ);

        MessageConsumer messageConsumer = (MessageConsumer) qsession2.createConsumer(receiveQ);
        ((progress.message.jclient.MessageConsumer)messageConsumer).setPrefetchCount(50);
        ((progress.message.jclient.MessageConsumer)messageConsumer).setPrefetchThreshold(5);

        //TextMessage msg = _session.createTextMessage(tempDestination.getQueueName() + "/Presented to in conjunction with Mahnah Mahnah and the Snowths");
        final long startTime = System.currentTimeMillis();

        messageConsumer.setMessageListener(new CallbackHandler(messageCount, startTime));
        qcon.start();
        qcon2.start();
        for (int i = 0; i < messageCount; i++)
        {
            TextMessage msg = qsession.createTextMessage("/Presented to in conjunction with Mahnah Mahnah and the Snowths:" + i);
            msg.setJMSReplyTo(receiveQ);
            if (i%1000 == 0)
            {
                long timeNow = System.currentTimeMillis();
                msg.setLongProperty("timeSent", timeNow);
            }
            qsender.send(msg);
        }

        new Thread("foo").start();
        //qsession.close();
        //qcon.close();
    }
}
