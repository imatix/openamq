package org.openamq.blz18;

import org.openamq.client.AMQConnection;
import org.openamq.client.AMQQueue;
import org.openamq.client.AMQSession;
import javax.jms.MessageListener;
import javax.jms.Message;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class Service implements MessageListener
{
    private final AMQConnection _connection;
    private final AMQSession _session;

    Service(String broker) throws Exception
    {
        this(connect(broker));
    }

    Service(AMQConnection connection) throws Exception
    {
        _connection = connection;
        AMQQueue queue = new SpecialQueue("ServiceQueue");
        _session = (AMQSession) _connection.createSession(false, AMQSession.NO_ACKNOWLEDGE);
        _session.createConsumer(queue).setMessageListener(this);
        _connection.start();
    }

    public void onMessage(Message request)
    {
        try
        {
            Message response = _session.createTextMessage("Response!");
            _session.createProducer(request.getJMSReplyTo()).send(response);
        }
        catch(Exception e)
        {
            e.printStackTrace(System.out);
        }
    }

    static AMQConnection connect(String broker) throws Exception
    {
        return new AMQConnection(broker, "guest", "guest", "Client" + System.currentTimeMillis(), "/test_path");
    }

    public static void main(String[] argv) throws Exception
    {
        new Service(connect(argv[0]));
        //declare a queue and bind it to amq.direct with a 'well known' routing key
        //register a consumer for this queue and send a response to every message received

    }
}
