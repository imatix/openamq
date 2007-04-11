package org.openamq.topic;

import org.openamq.client.AMQSession;
import org.openamq.config.ConnectorConfig;
import org.openamq.config.ConnectionFactoryInitialiser;
import org.openamq.config.Connector;
import org.openamq.config.AbstractConfig;

import javax.jms.Connection;
import javax.jms.ConnectionFactory;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
class Config extends AbstractConfig implements ConnectorConfig
{

    private String host = "localhost";
    private int port = 5672;
    private String factory = null;

    private int payload = 256;
    private int messages = 1000;
    private int clients = 1;
    private int batch = 1;
    private long delay = 1;
    private int warmup;
    private int ackMode= AMQSession.NO_ACKNOWLEDGE;
    private String clientId;
    private String subscriptionId;
    private boolean persistent;

    public Config()
    {
    }

    int getAckMode()
    {
        return ackMode;
    }

    void setPayload(int payload)
    {
        this.payload = payload;
    }

    int getPayload()
    {
        return payload;
    }

    void setClients(int clients)
    {
        this.clients = clients;
    }

    int getClients()
    {
        return clients;
    }

    void setMessages(int messages)
    {
        this.messages = messages;
    }

    int getMessages()
    {
        return messages;
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

    public String getFactory()
    {
        return factory;
    }

    public void setPort(int port)
    {
        this.port = port;
    }

    int getBatch()
    {
        return batch;
    }

    void setBatch(int batch)
    {
        this.batch = batch;
    }

    int getWarmup()
    {
        return warmup;
    }

    void setWarmup(int warmup)
    {
        this.warmup = warmup;
    }

    public long getDelay()
    {
        return delay;
    }

    public void setDelay(long delay)
    {
        this.delay = delay;
    }

    String getClientId()
    {
        return clientId;
    }

    String getSubscriptionId()
    {
        return subscriptionId;
    }

    boolean usePersistentMessages()
    {
        return persistent;
    }

    public void setOption(String key, String value)
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
        else if("-payload".equalsIgnoreCase(key))
        {
            setPayload(parseInt("Bad payload size", value));
        }
        else if("-messages".equalsIgnoreCase(key))
        {
            setMessages(parseInt("Bad message count", value));
        }
        else if("-clients".equalsIgnoreCase(key))
        {
            setClients(parseInt("Bad client count", value));
        }
        else if("-batch".equalsIgnoreCase(key))
        {
            setBatch(parseInt("Bad batch count", value));
        }
        else if("-delay".equalsIgnoreCase(key))
        {
            setDelay(parseLong("Bad batch delay", value));
        }
        else if("-warmup".equalsIgnoreCase(key))
        {
            setWarmup(parseInt("Bad warmup count", value));
        }
        else if("-ack".equalsIgnoreCase(key))
        {
            ackMode = parseInt("Bad ack mode", value);
        }
        else if("-factory".equalsIgnoreCase(key))
        {
            factory = value;
        }
        else if("-clientId".equalsIgnoreCase(key))
        {
            clientId = value;
        }
        else if("-subscriptionId".equalsIgnoreCase(key))
        {
            subscriptionId = value;
        }
        else if("-persistent".equalsIgnoreCase(key))
        {
            persistent = "true".equalsIgnoreCase(value);
        }
        else
        {
            System.out.println("Ignoring unrecognised option: " + key);
        }
    }

    static String getAckModeDescription(int ackMode)
    {
        switch(ackMode)
        {
            case AMQSession.NO_ACKNOWLEDGE: return "NO_ACKNOWLEDGE";
            case AMQSession.AUTO_ACKNOWLEDGE: return "AUTO_ACKNOWLEDGE";
            case AMQSession.CLIENT_ACKNOWLEDGE: return "CLIENT_ACKNOWLEDGE";
            case AMQSession.DUPS_OK_ACKNOWLEDGE: return "DUPS_OK_ACKNOWELDGE";
            case AMQSession.PRE_ACKNOWLEDGE: return "PRE_ACKNOWLEDGE";
        }
        return "AckMode=" + ackMode;
    }

    public Connection createConnection() throws Exception
    {
        return new Connector().createConnection(this);
    }
}
