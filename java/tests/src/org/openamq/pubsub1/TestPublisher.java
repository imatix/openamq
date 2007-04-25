package org.openamq.pubsub1;

import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQTopic;
import org.openamq.jms.MessageProducer;
import org.openamq.jms.Session;
import org.openamq.framing.FieldTable;
import org.openamq.client.message.AbstractJMSMessage;
import org.openamq.AMQUndeliveredException;

import javax.jms.ExceptionListener;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.TextMessage;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.io.StreamTokenizer;
import java.io.StringReader;
import java.io.IOException;

public class TestPublisher
{
    private static final Logger _log = Logger.getLogger(TestPublisher.class);

    private AMQConnection _connection;

    private Session _session;

    public TestPublisher(String host, int port, String clientID, String fields,
                                   int messageCount) throws Exception
    {
        try
        {
            int r = messageCount % 1000;

            if (r != 0)
            {
                int use = messageCount - r;

                if (use < 1000)
                    use = 1000;

                _log.warn("Requested message count: " + messageCount + ", using: " + use);

                messageCount = use;
            }

            String commandQueueName = "pubsub";
            createConnection(host, port, clientID);
            
            _session = (Session) _connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
            AMQTopic destination = new AMQTopic(commandQueueName);
            MessageProducer producer = (MessageProducer) _session.createProducer(destination);

            _connection.start();
            final long startTime = System.currentTimeMillis();

            TextMessage msg = _session.createTextMessage(destination.getTopicName());

            parseFields(fields, msg);

            for (int i = 0; i < messageCount; i++)
                producer.send(msg);

            _log.info("Finished sending " + messageCount + " messages");
        }
        catch (JMSException e)
        {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        }
    }

    private void parseFields(String selector, TextMessage msg) throws JMSException
    {
        StreamTokenizer st = new StreamTokenizer(new StringReader(selector));

        st.wordChars('=', '=');
        st.whitespaceChars(',', ',');
        // No number parsing for now
        st.wordChars('0', '9');
        st.wordChars('-', '-');
        st.wordChars('.', '.');

        try
        {
            parseFields(st, msg, null, false);
        }
        catch (Exception e)
        {
            JMSException
                je = new JMSException("Unexpected error parsing selector: " + selector);

            je.setLinkedException(e);
            throw je;
        }
    }

    private void parseFields(StreamTokenizer st, TextMessage msg, String field, boolean value) throws Exception
    {
        while (true)
        {
            int ttype = st.nextToken();

            if (ttype != st.TT_EOF)
            {
                if (field == null)
                {
                    field = st.sval;
                    continue;
                }
                else
                {
                    if ("=".equals(st.sval))
                    {
                        if (value)
                            _log.warn("Extra equal sign after field: " + field);

                        parseFields(st, msg, field, true);
                    }
                    else
                    {
                        if (value)
                        {
                            msg.setStringProperty(field, st.sval);
                            break;
                        }
                        else
                        {
                            msg.setStringProperty(field, null);
                        }
                    }

                    field = null;
                }
            }
            else
            {
                if (value)
                    _log.warn("Cannot get value for field: " + field);

                break;
            }
        }
    }

    private void createConnection(String host, int port, String clientID) throws Exception
    {
        _connection = new AMQConnection(host, port, "guest", "guest",
                                        clientID, "/test");
        _connection.setExceptionListener(
            new ExceptionListener() {
                int count;

                public void onException(JMSException exception)
                {
                    if (exception.getLinkedException() instanceof AMQUndeliveredException)
                    {
                        _log.warn("No matching subscriber found: " + exception);
                    }
                    else
                    {
                        _log.warn("Exception in client: " + exception);
                    }

                    count++;
                    if (count > 10)
                    {
                        _log.error("Too many unhandled exceptions, test terminating.");
                        System.exit(1);
                    }
                }
            }
        );
    }

    /**
     *
     * @param args argument 1 if present specifies the name of the temporary queue to create. Leaving it blank
     * means the server will allocate a name.
     */
    public static void main(String[] args)
    {
        if (args.length == 0)
        {
            System.err.println("Usage: TestPublisher <host> <port> <fields> <number of messages>");
        }
        try
        {
            int port = Integer.parseInt(args[1]);
            InetAddress address = InetAddress.getLocalHost();
            String clientID = address.getHostName() + System.currentTimeMillis();
            TestPublisher client = new TestPublisher(args[0], port, clientID, args[2], Integer.parseInt(args[3]));
        }
        catch (UnknownHostException e)
        {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        }
        catch (Exception e)
        {
            System.err.println("Error in client: " + e);
            e.printStackTrace();
        }
    }
}
