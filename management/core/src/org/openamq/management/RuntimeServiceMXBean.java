package org.openamq.management;

import org.openamq.management.jmx.Managable;
import org.openamq.management.jmx.JmxConstants;
import org.openamq.management.jmx.HierarchicalMBean;

/**
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
@Managable(domain = JmxConstants.JMX_DOMAIN,
           type = "runtimeService")
public interface RuntimeServiceMXBean
{
    /**
     * @return all broker runtimes
     */
    BrokerRuntimeMXBean[] getBrokerRuntimes();

    /**
     * @param name the name of the broker you want
     * @return the broker runtime representing the specified broker
     */
    BrokerRuntimeMXBean findBrokerRuntime(String name);
}
