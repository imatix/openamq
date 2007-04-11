package org.openamq.transacted;

import org.openamq.config.ConnectorConfig;
import org.openamq.config.AbstractConfig;
import org.openamq.config.Connector;

import javax.jms.Connection;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
class Config extends AbstractConfig implements ConnectorConfig
{
    private String host = "localhost";
    private int port = 5672;
    private String factory;
    private boolean echo;
    private int batch = 100;
    private boolean persistent = true;

    Config(String[] argv)
    {
        setOptions(argv);
    }

    Connection createConnection() throws Exception
    {
        return new Connector().createConnection(this);
    }

    public boolean isEchoOn()
    {
        return echo;
    }

    public boolean usePersistentMessages()
    {
        return persistent;
    }

    public int getBatchSize()
    {
        return batch;
    }

    public String getHost()
    {
        return host;
    }

    public int getPort()
    {
        return port;
    }

    public String getFactory()
    {
        return factory;
    }

    public void setOption(String key, String value)
    {
        if("-host".equalsIgnoreCase(key))
        {
            host = value;
        }
        else if("-port".equalsIgnoreCase(key))
        {
            port = parseInt("Bad port number", value);
        }
        else if("-factory".equalsIgnoreCase(key))
        {
            factory = value;
        }
        else if("-echo".equalsIgnoreCase(key))
        {
            echo = "true".equalsIgnoreCase(value);
        }
        else if("-persistent".equalsIgnoreCase(key))
        {
            persistent = "true".equalsIgnoreCase(value);
        }
        else if("-batch".equalsIgnoreCase(key))
        {
            batch = parseInt("Bad batch size", value);
        }
        else
        {
            System.out.println("Ignoring nrecognised option " + key);
        }
    }

}
