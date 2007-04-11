package org.openamq.config;

import org.openamq.config.ConnectionFactoryInitialiser;
import org.openamq.config.ConnectorConfig;

import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.management.MBeanServerConnection;
import javax.management.ObjectName;
import javax.management.MBeanException;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import javax.naming.NameNotFoundException;
import java.util.Hashtable;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class JBossConnectionFactoryInitialiser implements ConnectionFactoryInitialiser
{
    public ConnectionFactory getFactory(ConnectorConfig config) throws JMSException
    {
        ConnectionFactory cf = null;
        InitialContext ic = null;
        Hashtable ht = new Hashtable();
        ht.put(InitialContext.INITIAL_CONTEXT_FACTORY, "org.jnp.interfaces.NamingContextFactory");
        String jbossHost = System.getProperty("jboss.host", "eqd-lxamq01");
        String jbossPort = System.getProperty("jboss.port", "1099");
        ht.put(InitialContext.PROVIDER_URL, "jnp://" + jbossHost + ":" + jbossPort);
        ht.put(InitialContext.URL_PKG_PREFIXES, "org.jboss.naming:org.jnp.interfaces");

        try
        {
            ic = new InitialContext(ht);
            if (!doesDestinationExist("topictest.messages", ic))
            {
                deployTopic("topictest.messages", ic);
            }
            if (!doesDestinationExist("topictest.control", ic))
            {
                deployTopic("topictest.control", ic);
            }

            cf = (ConnectionFactory) ic.lookup("/ConnectionFactory");
            return cf;
        }
        catch (NamingException e)
        {
            throw new JMSException("Unable to lookup object: " + e);
        }
        catch (Exception e)
        {
            throw new JMSException("Error creating topic: " + e);
        }
    }

    private boolean doesDestinationExist(String name, InitialContext ic) throws Exception
    {
        try
        {
            ic.lookup("/" + name);
        }
        catch (NameNotFoundException e)
        {
            return false;
        }
        return true;
    }

    private void deployTopic(String name, InitialContext ic) throws Exception
    {
        MBeanServerConnection mBeanServer = lookupMBeanServerProxy(ic);

        ObjectName serverObjectName = new ObjectName("jboss.messaging:service=ServerPeer");

        String jndiName = "/" + name;
        try
        {
            mBeanServer.invoke(serverObjectName, "createTopic",
                               new Object[]{name, jndiName},
                               new String[]{"java.lang.String", "java.lang.String"});
        }
        catch (MBeanException e)
        {
            System.err.println("Error: " + e);
            System.err.println("Cause: " + e.getCause());
        }
    }

    private MBeanServerConnection lookupMBeanServerProxy(InitialContext ic) throws NamingException
    {
        return (MBeanServerConnection) ic.lookup("jmx/invoker/RMIAdaptor");
    }
}
