package org.openamq.management.jmx;

import org.apache.xmlbeans.XmlException;
import org.apache.log4j.Logger;
import org.openamq.AMQException;
import org.openamq.management.ManagementConnection;
import org.openamq.management.messaging.CMLMessageFactory;
import org.openamq.schema.cml.CmlDocument;
import org.openamq.schema.cml.SchemaReplyType;
import org.openamq.schema.cml.ClassType;

import javax.jms.JMSException;
import javax.jms.TextMessage;
import javax.management.MBeanServer;
import java.lang.management.ManagementFactory;
import java.io.IOException;
import java.io.Reader;
import java.io.InputStreamReader;

/**
 * Main entry point for AMQ console implementation.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQConsole
{
    private static final Logger _log = Logger.getLogger(AMQConsole.class);

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
        _mbeanRegistrar = new MBeanRegistrar(_mbeanServer, _connection, _mbeanInfoRegistry);
    }

    public CMLMBean getRootMBean() throws JMSException, AMQException
    {
        return _mbeanRegistrar.getRootMBean();
    }

    private void createMBeanInfo() throws JMSException, AMQException, XmlException
    {
        TextMessage tm = _connection.sendRequest(CMLMessageFactory.createSchemaRequest());
        if (_log.isDebugEnabled())
        {
            _log.debug("Response document: \n" + tm.getText());
        }
        CmlDocument cmlDoc = CmlDocument.Factory.parse(tm.getText());
        _mbeanInfoRegistry = new MBeanInfoRegistry(cmlDoc);
    }

    public static void main(String[] args)
    {
        AMQConsole console = new AMQConsole(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                            args[4]);
        try
        {
            console.initialise();
            console.getRootMBean();
        }
        catch (Exception e)
        {
            _log.error("Console initialisation error: " + e, e);
        }
    }
}
