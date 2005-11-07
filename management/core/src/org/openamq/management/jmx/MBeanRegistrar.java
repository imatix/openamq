package org.openamq.management.jmx;

import org.apache.log4j.Logger;
import org.apache.xmlbeans.XmlException;
import org.openamq.management.ManagementConnection;
import org.openamq.management.messaging.CMLMessageFactory;
import org.openamq.AMQException;
import org.openamq.schema.cml.CmlDocument;
import org.openamq.schema.cml.SchemaDocument;
import org.openamq.schema.cml.ClassDocument;
import org.openamq.schema.cml.InspectDocument;

import javax.management.MBeanServer;
import javax.jms.JMSException;
import javax.jms.TextMessage;

/**
 * Responsible for registering MBeans. This class will navigate through
 * our hierarchy of MBeans, registering them with the appropriate ObjectNames.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class MBeanRegistrar
{
    private static final Logger _log = Logger.getLogger(MBeanRegistrar.class);

    /** The MBean server with which all MBeans will be registered. */
    private MBeanServer _targetMBeanServer;

    /** The connection used to communicate with the broker */
    private ManagementConnection _connection;

    private MBeanInfoRegistry _mbeanInfoRegistry;

    /**
     * Create a registrar for the specified MBean server
     * @param targetMBeanServer the MBean server with which all MBeans will be registered
     */
    public MBeanRegistrar(MBeanServer targetMBeanServer, ManagementConnection connection,
                          MBeanInfoRegistry mbeanInfoRegistry)
    {
        _targetMBeanServer = targetMBeanServer;
        _connection = connection;
        _mbeanInfoRegistry = mbeanInfoRegistry;
    }

    public CMLMBean getRootMBean() throws AMQException, JMSException
    {
        TextMessage response = _connection.sendRequest(CMLMessageFactory.createInspectRequest(0));
        try
        {
            CmlDocument cmlDoc = CmlDocument.Factory.parse(response.getText());
            CmlDocument.Cml cml = cmlDoc.getCml();
            InspectDocument.Inspect inspect = cml.getInspect();
            inspect.getClass1()

        }
        catch (XmlException e)
        {
            throw new AMQException("Error parsing broker response: " + e, e);
        }
    }
}
