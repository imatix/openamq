package org.openamq.config;

import javax.jms.ConnectionFactory;
import javax.jms.JMSException;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public interface ConnectionFactoryInitialiser
{
    public ConnectionFactory getFactory(ConnectorConfig config) throws JMSException;
}
