package org.openamq.management.jmx;

import org.apache.xmlbeans.XmlException;
import org.openamq.AMQException;
import org.openamq.management.ManagementConnection;
import org.openamq.management.messaging.CMLMessageFactory;
import org.openamq.schema.cml.CmlDocument;

import javax.jms.JMSException;
import javax.jms.TextMessage;
import javax.management.MBeanServer;
import java.lang.management.ManagementFactory;

/**
 * Main entry point for AMQ console implementation.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQConsole
{
    private ManagementConnection _connection;

    private MBeanInfoRegistry _mbeanInfoRegistry;

    private MBeanRegistrar _mbeanRegistrar;

    private MBeanServer _mbeanServer;

    public AMQConsole(String host, int port, String username, String password,
                      String context)
    {
        _connection = new ManagementConnection(host, port, username, password, context);
    }

    public void initialise() throws AMQException, JMSException, XmlException
    {
        _connection.connect();
        createMBeanInfo();
        _mbeanServer = ManagementFactory.getPlatformMBeanServer();
        _mbeanRegistrar = new MBeanRegistrar(_mbeanServer, _connection);
    }

    private void createMBeanInfo() throws JMSException, AMQException, XmlException
    {
        TextMessage tm = _connection.sendRequest(CMLMessageFactory.createSchemaRequest());
        CmlDocument cmlDoc = CmlDocument.Factory.parse(tm.getText());
        _mbeanInfoRegistry = new MBeanInfoRegistry(cmlDoc);
    }
}
