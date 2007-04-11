package org.openamq.config;

import org.openamq.client.AMQConnectionFactory;
import org.openamq.config.ConnectionFactoryInitialiser;
import org.openamq.config.ConnectorConfig;

import javax.jms.ConnectionFactory;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
class AMQConnectionFactoryInitialiser implements ConnectionFactoryInitialiser
{
    public ConnectionFactory getFactory(ConnectorConfig config)
    {
        return new AMQConnectionFactory(config.getHost(), config.getPort(), "/test_path");
    }
}
