package org.openamq.headers;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQSession;
import org.openamq.testutil.Config;

import javax.jms.*;

public class Publisher implements MessageListener
{
    private final Object _lock = new Object();
    private final AMQConnection _connection;
    private final AMQSession _session;
    private final Destination _exchange;
    private final MessageFactory _factory;
    private final MessageProducer _publisher;
    private int _count;

    Publisher(AMQConnection connection, Destination exchange) throws Exception
    {
        _connection = connection;
        _exchange = exchange;
        _session = (AMQSession) _connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
        _factory = new MessageFactory(_session, 0, 19);
        _publisher = _session.createProducer(_exchange);
    }

    Publisher(Config config) throws Exception
    {
        this((AMQConnection) config.getConnection(), config.getDestination());
    }

    private void test(int msgCount, int consumerCount) throws Exception
    {
        _count = consumerCount;
        _factory.createController(_exchange).setMessageListener(this);
        _connection.start();
        long start = System.currentTimeMillis();
        publish(msgCount);
        waitForCompletion(consumerCount);
        long end = System.currentTimeMillis();

        System.out.println("Completed in " + (end - start) + " ms.");

        //request shutdown
        _publisher.send(_factory.createShutdownMessage());

        _connection.stop();
        _connection.close();
    }

    private void publish(int count) throws Exception
    {

        //send events
        for (int i = 0; i < count; i++)
        {
            _publisher.send(_factory.createEventMessage());
            if ((i + 1) % 100 == 0)
            {
                System.out.println("Sent " + (i + 1) + " messages");
            }
        }

        //request report
        _publisher.send(_factory.createReportRequestMessage());
    }

    private void waitForCompletion(int consumers) throws Exception
    {
        System.out.println("Waiting for completion...");
        synchronized (_lock)
        {
            while (_count > 0)
            {
                _lock.wait();
            }
        }
    }


    public void onMessage(Message message)
    {
        System.out.println("Received report " + _factory.getReport(message) + " " + --_count + " remaining");
        if (_count == 0)
        {
            synchronized (_lock)
            {
                _lock.notify();
            }
        }
    }


    public static void main(String[] argv) throws Exception
    {
        if (argv.length >= 2)
        {
            int msgCount = Integer.parseInt(argv[argv.length - 2]);
            int consumerCount = Integer.parseInt(argv[argv.length - 1]);

            Config config = new Config();
            config.setType(Config.HEADERS);
            config.setName("test_headers_exchange");
            String[] options = new String[argv.length - 2];
            System.arraycopy(argv, 0, options, 0, options.length);
            config.setOptions(options);

            new Publisher(config).test(msgCount, consumerCount);
        }

    }
}
