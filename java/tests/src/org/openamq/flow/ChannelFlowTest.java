package org.openamq.flow;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQDestination;
import org.openamq.client.AMQSession;
import org.openamq.client.AMQQueue;
import org.openamq.testutil.Config;

import javax.jms.MessageListener;
import javax.jms.Message;
import javax.jms.MessageProducer;

public class ChannelFlowTest implements MessageListener
{
    private int sent;
    private int received;

    ChannelFlowTest(String broker) throws Exception
    {
        this(new AMQConnection(broker, "guest", "guest", randomize("Client"), "/test_path"));
    }

    ChannelFlowTest(AMQConnection connection) throws Exception
    {
        this(connection, new AMQQueue(randomize("ChannelFlowTest"), true));
    }

    ChannelFlowTest(AMQConnection connection, AMQDestination destination) throws Exception
    {
        AMQSession session = (AMQSession) connection.createSession(false, AMQSession.NO_ACKNOWLEDGE, 50);

        //set up a slow consumer
        session.createConsumer(destination).setMessageListener(this);
        connection.start();

        //create a publisher
        MessageProducer producer = session.createProducer(destination);
        Message msg = session.createTextMessage("Message");

        //publish in bursts that are fast enough to cause channel flow control
        for(int i = 0; i < 10; i++)
        {
            for(int j = 0; j < 100; j++)
            {
                producer.send(msg);
                sent++;
            }
            waitUntilReceived(sent - 40);
        }

        waitUntilReceived(sent);

        session.close();
        connection.close();
    }


    private synchronized void waitUntilReceived(int count) throws InterruptedException
    {
        while(received <count)
        {
            wait();
        }
    }

    public synchronized void onMessage(Message message)
    {
        try
        {
            Thread.sleep(50);

            received++;
            notify();
        }
        catch (InterruptedException e)
        {
            e.printStackTrace();
        }
    }

    private static String randomize(String in)
    {
        return in + System.currentTimeMillis();
    }

    public static void main(String[] argv) throws Exception
    {
        new ChannelFlowTest(argv.length == 0 ? "localhsot:5672" : argv[0]);
    }

}
