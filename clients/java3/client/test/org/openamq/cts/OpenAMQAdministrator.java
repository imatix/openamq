/**
 * Date Created: 22-Jun-2005
 *************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of 
 * this program may be photocopied reproduced or translated to another 
 * program language without prior written consent of JP Morgan Chase Ltd
 *************************************************************************/
package org.openamq.cts;

import org.exolab.jmscts.provider.Administrator;
import org.openamq.client.AMQConnectionFactory;

import javax.naming.NamingException;
import javax.jms.JMSException;
import java.util.HashMap;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class OpenAMQAdministrator implements Administrator
{
    private String _host = "localhost";

    private int _port = 7654;

    private String _virtualPath = "/test";

    private HashMap _directory = new HashMap();

    private static final String CONNECTION_FACTORY = "ConnectionFactory";

    public void initialise() throws JMSException
    {

    }

    public void cleanup() throws JMSException
    {

    }

    public String getQueueConnectionFactory()
    {
        return CONNECTION_FACTORY;
    }

    public String getTopicConnectionFactory()
    {
        return CONNECTION_FACTORY;
    }

    public String getXAQueueConnectionFactory()
    {
        return null;
    }

    public String getXATopicConnectionFactory()
    {
        return null;
    }

    public Object lookup(String name) throws NamingException
    {
        Object item = _directory.get(name);
        if (item == null)
        {
            item = constructItemAndRegister(name);
        }
        return item;
    }

    private Object constructItemAndRegister(String name)
    {
        Object val = null;
        if (name.equals(CONNECTION_FACTORY))
        {
            val = new AMQConnectionFactory(_host, _port, _virtualPath);
        }
        else
        {
            return null;
        }

        _directory.put(name, val);
        return val;
    }

    public void createDestination(String string, boolean b) throws JMSException
    {
        
    }

    public void destroyDestination(String string) throws JMSException
    {

    }

    public boolean destinationExists(String string) throws JMSException
    {
        return false;
    }
}
