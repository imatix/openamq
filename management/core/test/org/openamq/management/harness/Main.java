package org.openamq.management.harness;

import org.apache.log4j.Logger;
import org.openamq.management.RuntimeServiceMXBean;
import org.openamq.management.jmx.HierarchicalMBean;
import org.openamq.management.jmx.MBeanRegistrar;

import javax.management.JMException;
import javax.management.MBeanServer;
import javax.management.remote.JMXConnectorServer;
import javax.management.remote.JMXConnectorServerFactory;
import javax.management.remote.JMXServiceURL;
import java.io.IOException;
import java.lang.management.ManagementFactory;
import java.rmi.registry.LocateRegistry;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class Main
{
    private static final Logger _log = Logger.getLogger(Main.class);

    public static void main(String[] args)
    {
        MBeanServer server = ManagementFactory.getPlatformMBeanServer();

        _log.info("Creating RuntimeServicesMBean");
        RuntimeServiceMXBean runtimeServiceMBean = new RuntimeServiceMXBeanImpl("runtime01");
        MBeanRegistrar registrar = new MBeanRegistrar(server);

        try
        {
            registrar.registerMBeanHierarchy((HierarchicalMBean)runtimeServiceMBean);
            LocateRegistry.createRegistry(8999);
            JMXServiceURL serviceURL = new JMXServiceURL("service:jmx:rmi:///jndi/rmi://localhost:8999/jmxrmi");
            JMXConnectorServer jmxServer = JMXConnectorServerFactory.newJMXConnectorServer(serviceURL,
                                                                                           null, server);
            jmxServer.start();
            _log.info("JMX Server bound in RMI Registry and started");
        }
        catch (IOException e)
        {
            _log.error("Error registering MBean server with RMI: " + e, e);
        }
        catch (JMException e)
        {
            _log.error("Error registering MBean: " + e, e);
        }
    }
}
