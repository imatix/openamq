package org.openamq.testutil;

import org.openamq.client.AMQQueue;
import org.openamq.client.AMQTopic;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQHeadersExchange;

import javax.jms.Destination;
import javax.jms.Connection;

public class Config
{
    public static final String QUEUE = "queue";
    public static final String TOPIC = "topic";
    public static final String HEADERS = "headers";

    private String host = "localhost";
    private int port = 5672;
    private String type;
    private String name = "simple_test_queue";

    public Config()
    {
        this("localhost", 5672, QUEUE, "simple_test_queue");
    }

    public Config(String host, int port, String type, String name)
    {
        setHost(host);
        setPort(port);
        setType(type);
        setName(name);
    }

    public String getHost()
    {
        return host;
    }

    public void setHost(String host)
    {
        this.host = host;
    }

    public int getPort()
    {
        return port;
    }

    public void setPort(int port)
    {
        this.port = port;
    }

    public String getType()
    {
        return type;
    }

    public void setType(String type)
    {
        this.type = type;
    }

    public boolean isQueue()
    {
        return QUEUE.equalsIgnoreCase(type);
    }

    public boolean isTopic()
    {
        return TOPIC.equalsIgnoreCase(type);
    }

    private boolean isHeaders()
    {
        return HEADERS.equalsIgnoreCase(type);
    }

    public void setQueue(boolean queue)
    {
        type = queue ? QUEUE : TOPIC;
    }

    public String getName()
    {
        return name;
    }

    public void setName(String name)
    {
        this.name = name;
    }

    public Destination getDestination()
    {
        if(isQueue())
        {
            System.out.println("Using queue named " + name);
            return new AMQQueue(name);
        }
        else if(isTopic())
        {
            System.out.println("Using topic named " + name);
            return new AMQTopic(name);
        }
        else if(isHeaders())
        {
            System.out.println("Using headers exhange named " + name);
            return new AMQHeadersExchange(name);
        }
        return null;
    }

    public Connection getConnection() throws Exception
    {
        System.out.println("Connecting to " + host + " on " + port + "...");
        return new AMQConnection(host, port, "guest", "guest", "Client" + System.currentTimeMillis(), "/test_path");
    }

    public boolean setOptions(String[] argv)
    {
        try
        {
            for(int i = 0; i < argv.length - 1; i += 2)
            {
                String key = argv[i];
                String value = argv[i+1];
                setOption(key, value);
            }
            return true;
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
            }
        return false;
    }

    private void setOption(String key, String value)
    {
        if("-host".equalsIgnoreCase(key))
        {
            setHost(value);
        }
        else if("-port".equalsIgnoreCase(key))
        {
            try
            {
                setPort(Integer.parseInt(value));
            }
            catch(NumberFormatException e)
            {
                throw new RuntimeException("Bad port number: " + value);
            }
        }
        else if("-name".equalsIgnoreCase(key))
        {
            setName(value);
        }
        else if("-type".equalsIgnoreCase(key))
        {
            if(QUEUE.equalsIgnoreCase(value)
                    || TOPIC.equalsIgnoreCase(value)
                    || HEADERS.equalsIgnoreCase(value))
            {
                type = value;
            }
            else{
                throw new RuntimeException("Bad destination type: " + value);
            }
        }
        else
        {
            System.out.println("Ignoring unrecognised option: " + key);
        }
    }
}
