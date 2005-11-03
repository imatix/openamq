package org.openamq.management.harness;

import javax.management.MBeanServerConnection;
import javax.management.ObjectName;
import javax.management.MalformedObjectNameException;
import javax.management.remote.JMXConnector;
import javax.management.remote.JMXConnectorFactory;
import javax.management.remote.JMXServiceURL;
import java.io.IOException;
import java.net.MalformedURLException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class SimpleJMXClient
{

    private static MBeanServerConnection connection;
    private static JMXConnector connector;
    private static final ObjectName service;

    // Initializing the object name for DomainRuntimeServiceMBean
    // so it can be used throughout the class.
    static
    {
        try
        {
            service = new ObjectName(
                    "org.openamq:name=runtime01,type=RuntimeServiceMXBean");
        }
        catch (MalformedObjectNameException e)
        {
            throw new AssertionError(e.getMessage());
        }
    }

    /*
    * Initialize connection to the Domain Runtime MBean Server
    */
    public static void initConnection() throws IOException,
            MalformedURLException
    {
        JMXServiceURL serviceURL = new JMXServiceURL("service:jmx:rmi:///jndi/rmi://localhost:8999/jmxrmi");
        connector = JMXConnectorFactory.connect(serviceURL, null);
        connection = connector.getMBeanServerConnection();
    }

    /*
    * Print an array of ServerRuntimeMBeans.
    * This MBean is the root of the runtime MBean hierarchy, and
    * each server in the domain hosts its own instance.
    */
    public static ObjectName[] getServerRuntimes() throws Exception
    {
        return (ObjectName[]) connection.getAttribute(service,
                                                      "BrokerRuntimes");
    }

    /*
    * Iterate through ServerRuntimeMBeans and get the name and state
    */
    public void printNameAndState() throws Exception
    {
        ObjectName[] serverRT = getServerRuntimes();
        System.out.println("got server runtimes");
        int length = serverRT.length;        
        for (int i = 0; i < length; i++)
        {
            String name = (String) connection.getAttribute(serverRT[i],
                                                           "HostName");
            String state = (String) connection.getAttribute(serverRT[i],
                                                            "Status");
            System.out.println("Server name: " + name + ".   Server state: " + state);
        }
    }

    public static void main(String[] args) throws Exception
    {

        SimpleJMXClient s = new SimpleJMXClient();
        initConnection();
        s.printNameAndState();
        connector.close();
    }
}
