package org.openamq.management;

import org.openamq.management.jmx.Managable;
import org.openamq.management.jmx.JmxConstants;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
@Managable(domain = JmxConstants.JMX_DOMAIN,
           type = "broker")
public interface BrokerRuntimeMXBean
{
    /**
     * @return status of the broker
     */
    String getStatus();

    /**
     * @return a version String
     */
    String getVersion();

    /**
     * The time the broker thinks it is
     */
    String getBrokerLocalTime();

    String getHostName();

    String getOperatingSystemName();

    String getOperatingSystemVersion();

    long getUptimeInSeconds();


}
