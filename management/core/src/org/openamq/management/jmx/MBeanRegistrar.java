package org.openamq.management.jmx;

import org.apache.log4j.Logger;

import javax.management.JMException;
import javax.management.MBeanServer;
import javax.management.MXBean;
import javax.management.ObjectName;

/**
 * Responsible for registering MBeans. This class will navigate through
 * our hierarchy of MBeans, registering them with the appropriate ObjectNames.
 *
 * Currently only supports MXBeans, our favoured type of MBean, but could be
 * enhanced fairly easily to support other types of MBean.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class MBeanRegistrar
{
    private static final Logger _log = Logger.getLogger(MBeanRegistrar.class);

    /** The MBean server with which all MBeans will be registered. */
    private MBeanServer _targetMBeanServer;

    /**
     * Create a registrar for the specified MBean server
     * @param targetMBeanServer the MBean server with which all MBeans will be registered
     */
    public MBeanRegistrar(MBeanServer targetMBeanServer)
    {
        _targetMBeanServer = targetMBeanServer;
    }

    /**
     * Register a hierarchy of MBeans.
     * @param root the root of the hierarchy. All children will be registered, in the order
     * that is traversed by the visit() method overridden by the concrete MBeans.
     * @throws JMException if registration fails.
     */
    public void registerMBeanHierarchy(HierarchicalMBean root) throws JMException
    {
        _log.info("Registering MBean hierarchy from root " + root);

        if (!isMXBean(root))
        {
            throw new IllegalArgumentException("All MBeans must be MXBeans.");
        }

        // this registers all MBeans in the hierarchy using the common Visitor
        // design pattern (see GoF if unfamiliar).
        root.visit(new HierarchicalMBean.Visitor()
        {
            public void visited(HierarchicalMBean o) throws JMException
            {
                registerMBean(o);
            }
        });
        registerMBean(root);
    }

    /**
     * Helper method to register MBeans.
     * @param o the bean to register
     * @throws JMException if something goes wrong
     */
    private void registerMBean(HierarchicalMBean o) throws JMException
    {
         _targetMBeanServer.registerMBean(o, new ObjectName("org.banana:name=pies"));
    }

    /**
     * Utility method to test whether a given HierarchicalMBean is an MXBean. It is
     * tested by looking for the MXBean annotation first, then it checks the class name
     * to determine whether it ends in MXBean. Note that if it has the MXBean(false)
     * annotation the class name check is <b>skipped</b>.
     * @param bean the bean to test
     * @return true if it is an MXBean, false otherwise
     */
    private static boolean isMXBean(HierarchicalMBean bean)
    {
        final Class<?> beanClass = bean.getClass();
        if (beanClass.isAnnotationPresent(MXBean.class))
        {
            MXBean annotation = beanClass.getAnnotation(MXBean.class);
            return annotation.value();
        }
        else
        {
            // getInterfaces() is defined never to return null
            Class<?>[] interfaces = beanClass.getInterfaces();
            for (Class<?> ifc : interfaces)
            {
                if (ifc.getName().endsWith("MXBean"))
                {
                    return true;
                }
            }
            // if we reach here no suitable interface was found
            return false;
        }
    }
}
