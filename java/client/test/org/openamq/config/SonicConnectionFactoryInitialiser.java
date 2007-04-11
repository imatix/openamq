package org.openamq.config;

import org.openamq.config.ConnectionFactoryInitialiser;
import org.openamq.config.ConnectorConfig;

import javax.jms.ConnectionFactory;
import javax.jms.JMSException;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class SonicConnectionFactoryInitialiser implements ConnectionFactoryInitialiser
{
    public ConnectionFactory getFactory(ConnectorConfig config)
            throws JMSException
    {
        return new progress.message.jclient.ConnectionFactory(config.getHost());
    }
}
