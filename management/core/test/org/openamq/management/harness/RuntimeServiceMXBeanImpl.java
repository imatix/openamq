package org.openamq.management.harness;

import org.openamq.management.BrokerRuntimeMXBean;
import org.openamq.management.RuntimeServiceMXBean;
import org.openamq.management.jmx.JmxConstants;
import org.openamq.management.jmx.HierarchicalMBean;

import javax.management.*;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class RuntimeServiceMXBeanImpl extends HierarchicalMBean implements RuntimeServiceMXBean
{
    private Map<String, BrokerRuntimeMXBean> _brokerRuntimes = new LinkedHashMap<String, BrokerRuntimeMXBean>();

    private String _name;

    public RuntimeServiceMXBeanImpl(String name)
    {
        if (name == null)
        {
            throw new IllegalArgumentException("Name must not be null");
        }
        _name = name;
        _brokerRuntimes.put("kermit", new BrokerRuntimeMXBeanImpl(this, "kermit"));
    }

    public BrokerRuntimeMXBean[] getBrokerRuntimes()
    {
        BrokerRuntimeMXBean[] mbeans = new BrokerRuntimeMXBean[_brokerRuntimes.size()];
        return _brokerRuntimes.values().toArray(mbeans);
    }

    public BrokerRuntimeMXBean findBrokerRuntime(String name)
    {
        return _brokerRuntimes.get(name);
    }

    public void visit(Visitor visitor) throws JMException
    {
        BrokerRuntimeMXBean[] runtimes = getBrokerRuntimes();
        for (BrokerRuntimeMXBean brokerRuntimeMXBean : runtimes)
        {
            visitor.visited((HierarchicalMBean)brokerRuntimeMXBean);
        }
    }

    protected String getMBeanComponentName()
    {
        return _name;
    }
}
