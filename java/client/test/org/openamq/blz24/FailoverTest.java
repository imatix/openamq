package org.openamq.blz24;

import org.openamq.client.AMQConnection;
import org.openamq.AMQException;
import org.openamq.jms.ConnectionListener;

import javax.jms.MessageListener;
import javax.jms.Message;
import javax.jms.Destination;
import javax.jms.MessageProducer;
import javax.jms.Session;
import javax.jms.TextMessage;
import javax.jms.JMSException;
import javax.jms.Connection;
import javax.jms.Topic;
import javax.jms.Queue;
import java.util.TimerTask;
import java.util.Timer;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class FailoverTest implements MessageListener, ConnectionListener
{
    private static final long TIMEOUT = 10000;
    private static final long INTERVAL = 5000;
    private final Timer _timer = new Timer(true);
    private final Connection _connection;
    private final Session _session;
    private final MessageProducer _producer;
    private Timeout _timeout;
    private int _count;

    FailoverTest(String brokers) throws JMSException, AMQException
    {
        this(new AMQConnection(brokers, "guest", "guest", "BLZ-24" + System.currentTimeMillis(), "/test"));
        ((AMQConnection) _connection).setConnectionListener(this);
    }

    FailoverTest(Connection connection) throws JMSException
    {
        _connection = connection;
        _session = _connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
        Topic topic = _session.createTopic("BLZ-24");
        Queue queue = _session.createTemporaryQueue();
        _producer = _session.createProducer(topic);
        _session.createConsumer(queue).setMessageListener(this);
        //new TopicListener(_session, topic);
        new TopicListener(_connection.createSession(false, Session.AUTO_ACKNOWLEDGE), topic);
        _connection.start();

        Message msg = _session.createTextMessage("Init");
        msg.setJMSReplyTo(queue);
        send(msg);
    }

    public synchronized void onMessage(Message message)
    {
        try
        {
            //cancel timeout:
            _timeout.clear();
            new DelayedSend(_session.createTextMessage("Message" + (++_count)), INTERVAL);
        }
        catch (JMSException e)
        {
            error(e);
        }
    }

    private synchronized void send(Message msg) throws JMSException
    {
        _producer.send(msg);
        //start timeout:
        _timeout = new Timeout(TIMEOUT);
    }

    private void error(Exception e)
    {
        e.printStackTrace();
        stop();
    }

    private void stop()
    {
        System.out.println("Stopping...");
        try
        {
            _connection.close();
        }
        catch (JMSException e)
        {
            System.out.println("Failed to shutdown: " + e);
            e.printStackTrace();
        }
    }

    private void timeout()
    {
        error(new RuntimeException("Timed out: count = " + _count));
    }

    public void bytesSent(long count)
    {
    }

    public void bytesReceived(long count)
    {
    }

    public boolean preFailover(boolean redirect)
    {
        System.out.println("preFailover(" + redirect + ") called");
        return true;
    }

    public boolean preResubscribe()
    {
        System.out.println("preResubscribe() called");
        return true;
    }

    public void failoverComplete()
    {
        System.out.println("failoverComplete() called");
    }

    private class TopicListener implements MessageListener
    {
        private final Session _session;
        private MessageProducer _producer;
        private int _received;

        TopicListener(Session session, Topic topic) throws JMSException
        {
            _session = session;
            _session.createConsumer(topic).setMessageListener(this);
        }

        public void onMessage(Message message)
        {
            try
            {
                //if(_received++ % 100 == 0)
                {
                    System.out.println("Received: " + ((TextMessage) message).getText());
                }
                if(_producer == null)
                {
                    _producer = init(message);
                }
                reply(message);
            }
            catch (JMSException e)
            {
               error(e);
            }
        }

        private void reply(Message message) throws JMSException
        {
            _producer.send(_session.createTextMessage(((TextMessage) message).getText()));
        }

        private MessageProducer init(Message message) throws JMSException
        {
            return _session.createProducer(message.getJMSReplyTo());
        }
    }

    private class Timeout extends TimerTask
    {
        private volatile boolean _cancelled;

        Timeout(long time)
        {
            _timer.schedule(this, time);
        }

        void clear()
        {
            _cancelled = true;
        }

        public void run()
        {
            if(!_cancelled)
            {
                timeout();
            }
        }
    }

    private class DelayedSend extends TimerTask
    {
        private final Message _msg;

        DelayedSend(Message msg, long delay)
        {
            _msg = msg;
            _timer.schedule(this, delay);
        }

        public void run()
        {
            try
            {
                send(_msg);
            }
            catch (JMSException e)
            {
                error(e);
            }
        }
    }

    public static void main(String[] argv) throws Exception
    {
        new FailoverTest(argv[0]);
        while(true)
        {
            //must leave main thread active or VM will shutdown on failover.
            Thread.sleep(Long.MAX_VALUE);
        }
    }
}
