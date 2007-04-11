package org.openamq.config;

import javax.jms.Connection;
import javax.jms.ConnectionFactory;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class Connector
{
    public Connection createConnection(ConnectorConfig config) throws Exception
    {
        return getConnectionFactory(config).createConnection();
    }

    ConnectionFactory getConnectionFactory(ConnectorConfig config) throws Exception
    {
        String factory = config.getFactory();
        if(factory == null) factory = AMQConnectionFactoryInitialiser.class.getName();
        System.out.println("Using " + factory);
        return ((ConnectionFactoryInitialiser) Class.forName(factory).newInstance()).getFactory(config);
    }
}
