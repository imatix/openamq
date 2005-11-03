package org.openamq.management.harness;

import org.openamq.management.BrokerRuntimeMXBean;
import org.openamq.management.jmx.JmxConstants;
import org.openamq.management.jmx.HierarchicalMBean;

import javax.management.*;
import java.text.SimpleDateFormat;
import java.net.InetAddress;
import java.net.UnknownHostException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class BrokerRuntimeMXBeanImpl extends HierarchicalMBean implements BrokerRuntimeMXBean
{
    private String _name;

    public BrokerRuntimeMXBeanImpl(HierarchicalMBean parent, String name)
    {
        super(parent);
        _name = name;
    }

    public String getStatus()
    {
        return "Gubbed";
    }

    public String getVersion()
    {
        return "0.9.3";
    }

    public String getBrokerLocalTime()
    {
        return SimpleDateFormat.getInstance().format(new java.util.Date());
    }

    public String getHostName()
    {
        try
        {
            return InetAddress.getLocalHost().getHostName();
        }
        catch (UnknownHostException e)
        {
            return "Error retrieving host";
        }
    }

    public String getOperatingSystemName()
    {
        return System.getProperty("os.name", "Operating System name unavailable");
    }

    public String getOperatingSystemVersion()
    {
        return System.getProperty("os.version", "Operating System version unavailable");
    }

    public long getUptimeInSeconds()
    {
        return 99;
    }

    public void visit(Visitor visitor) throws JMException
    {
        // No children so nothing to do
    }

    protected String getMBeanComponentName()
    {
        return _name;
    }
}
