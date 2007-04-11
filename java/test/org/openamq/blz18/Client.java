package org.openamq.blz18;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQSession;
import org.openamq.client.AMQQueue;

import javax.jms.Message;
import javax.jms.MessageListener;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class Client implements MessageListener
{
    private final AMQConnection _connection;
    private final AMQSession _session;
    private final int _expected;
    private int _count;

    Client(String broker, int expected) throws Exception
    {
        this(connect(broker), expected);
    }

    Client(AMQConnection connection, int expected) throws Exception
    {
        _connection = connection;
        _expected = expected;
        _session = (AMQSession) _connection.createSession(false, AMQSession.NO_ACKNOWLEDGE);
        AMQQueue response = new AMQQueue("ResponseQueue", true);
        _session.createConsumer(response).setMessageListener(this);
        _connection.start();
        AMQQueue service = new SpecialQueue("ServiceQueue");
        Message request = _session.createTextMessage("Request!");
        request.setJMSReplyTo(response);
        _session.createProducer(service).send(request);
    }

    void shutdownWhenComplete() throws Exception
    {
        waitUntilComplete();
        _connection.close();
    }


    public void onMessage(Message response)
    {
        System.out.println("Received " + (++_count) + " of " + _expected  + " responses.");
        if(_count == _expected)
        {
            synchronized(this)
            {
                notifyAll();
            }
        }
    }

    synchronized void waitUntilComplete() throws InterruptedException
    {
        while(_count < _expected)
        {
            wait();
        }
    }

    static AMQConnection connect(String broker) throws Exception
    {
        return new AMQConnection(broker, "guest", "guest", "Client" + System.currentTimeMillis(), "/test_path");
    }

    public static void main(String[] argv) throws Exception
    {
        //declare a private temporary response queue
        //send a message to amq.direct with a well known routing key with the
        //private response queue as the reply-to destination
        //consume responses
        new Client(connect(argv[0]), Integer.parseInt(argv[1])).shutdownWhenComplete();
    }

}
