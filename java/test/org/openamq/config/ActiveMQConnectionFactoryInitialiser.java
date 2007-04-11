/*****************************************************************************
 * Filename    : ActiveMQConnectionFactoryInitialiser.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.config;

import org.apache.activemq.ActiveMQConnectionFactory;
import org.apache.activemq.ActiveMQPrefetchPolicy;
import org.openamq.config.ConnectionFactoryInitialiser;
import org.openamq.config.ConnectorConfig;

import javax.jms.ConnectionFactory;
import javax.jms.JMSException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
class ActiveMQConnectionFactoryInitialiser implements ConnectionFactoryInitialiser
{
    public ConnectionFactory getFactory(ConnectorConfig config) throws JMSException
    {
	String url = "tcp://" + config.getHost() + ":" +
	    config.getPort() + "?wireFormat.tcpNoDelayEnabled=" + Boolean.getBoolean("tcpNoDelayEnabled");
	System.out.println("Connecting with: " + url);
        ActiveMQConnectionFactory acf = new ActiveMQConnectionFactory(url);
        acf.setAsyncDispatch(false);
        acf.setOptimizedMessageDispatch(true);
        acf.setDisableTimeStampsByDefault(true);
        acf.setUseAsyncSend(true);
        ActiveMQPrefetchPolicy prefetchPolicy = new ActiveMQPrefetchPolicy();
        prefetchPolicy.setAll(5000);
        acf.setPrefetchPolicy(prefetchPolicy);
        return acf;
    }
}
