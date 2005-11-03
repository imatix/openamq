/*****************************************************************************
 * Filename    : HierarchicalMBean.java
 * Date Created: 05/10/2005
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.management.jmx;

import org.apache.log4j.Logger;

import javax.management.*;
import java.util.*;

/**
 * Provides support for managing MBeans that exist in a hierarchy.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public abstract class HierarchicalMBean implements MBeanRegistration
{
    public interface Visitor
    {
        /**
         * Callback invoked by the HierarchicalMBean.
         *
         * @param o the MBean being visited
         * @throws JMException since many use cases of this callback will be to perform
         *                     operations against the MBeanServer it makes sense to allow the callback to
         *                     propagate the exception.
         */
        void visited(HierarchicalMBean o) throws JMException;
    }

    /**
     * Non-static logger to allow actual MBean instance name to appear in log entries
     */
    protected final Logger _log = Logger.getLogger(getClass());

    /**
     * Reference to parent MBean. Can be null indicating root of the hierarchy.
     */
    protected HierarchicalMBean _parentMBean;

    /**
     * We cache the object name since it is non-trivial to compute
     */
    private ObjectName _cachedObjectName;

    protected HierarchicalMBean(HierarchicalMBean parent)
    {
        _parentMBean = parent;
    }

    protected HierarchicalMBean()
    {
    }

    /**
     * Visit this bean and its children
     *
     * @param visitor the callback to use
     */
    public abstract void visit(Visitor visitor) throws JMException;

    /**
     * Callback from MBeanRegistration interface. Used in this case to allow the bean
     * to construct its own ObjectName.
     *
     * @param server server to which this bean is going to be registered
     * @param name   the name if one was supplied to the register method on the server
     * @return the ObjectName that will be used.
     */
    public ObjectName preRegister(MBeanServer server, ObjectName name) throws JMException
    {
        ObjectName objectName = getObjectName();
        if (_log.isDebugEnabled())
        {
            _log.debug("About to register MBean " + toString() + " with object name " +
                    objectName);
        }
        return objectName;
    }

    public void postDeregister()
    {
        try
        {
            _log.debug("MBean " + getObjectName() + " has been deregistered");
        }
        catch (JMException e)
        {
            _log.error("Error: " + e, e);
        }
    }

    public void postRegister(Boolean registrationDone)
    {
        try
        {
            _log.info("MBean registration of " + getObjectName() +
                    (registrationDone ? " succeeded" : " failed"));
        }
        catch (JMException e)
        {
            _log.error("Error: " + e, e);
        }
    }

    public void preDeregister()
    {
        try
        {
            _log.debug("MBean " + getObjectName() + " is about to be deregistered");
        }
        catch (JMException e)
        {
            _log.error("Error: " + e, e);
        }
    }

    protected ObjectName getObjectName() throws JMException
    {
        if (_cachedObjectName == null)
        {
            // javax.management.ObjectName uses Hashtable
            Hashtable<String, String> nameMap = new Hashtable<String, String>();
            List<String> typeList = new LinkedList<String>();
            List<String> nameList = new LinkedList<String>();

            // we get the names and types of the individual beans in the hierarchy above
            // this bean, including this bean
            getComponentTypes(typeList);
            getComponentNames(nameList);

            assert typeList.size() == nameList.size();

            StringBuffer fullTypeName = new StringBuffer(typeList.size() * 5);
            for (String s : typeList)
            {
                fullTypeName.append('.').append(s);
            }
            // remove the leading period (.)
            fullTypeName.delete(0, 1);
            _log.debug("MBean full type name: " + fullTypeName);
            nameMap.put("type", fullTypeName.toString());           

            for (int i = 0; i < nameList.size(); i++)
            {
                final String name = nameList.get(i);
                String type = typeList.get(i);
                if (i == nameList.size() - 1)
                {
                    // in the case of the object itself, the "type" is actually
                    // name in the objectname conventions.
                    type = "name";
                }
                nameMap.put(type, name);
                _log.debug("Added name: " + name + " and type " + type);

            }

            _cachedObjectName = new ObjectName(getJMXDomainName(), nameMap);
        }
        return _cachedObjectName;
    }

    /**
     * Get the individual component name of this MBean. This is used in construction
     * of the full name. For example, if this is a QueueMBean the component could be
     * 'myQueue1'.
     *
     * @return the String name of this individual component
     */
    protected abstract String getMBeanComponentName();

    protected void getComponentTypes(List<String> list)
    {
        if (_parentMBean != null)
        {
            _parentMBean.getComponentTypes(list);
        }
        list.add(getMBeanComponentType());
    }

    protected void getComponentNames(List<String> list)
    {
        if (_parentMBean != null)
        {
            _parentMBean.getComponentNames(list);
        }
        list.add(getMBeanComponentName());
    }

    protected String getMBeanComponentType()
    {
        Class<?> clazz = getClass();
        Managable annotation = clazz.getAnnotation(Managable.class);

        if (annotation == null)
        {
            Class<?>[] interfaces = clazz.getInterfaces();
            for (Class<?> intf : interfaces)
            {
                annotation = intf.getAnnotation(Managable.class);
                if (annotation != null)
                {
                    break;
                }
            }
        }

        if (annotation != null)
        {
            return annotation.type();
        }
        else
        {
            return clazz.getName();
        }
    }

    /**
     * Gets the domain name to be used when registering this MBean. Looks first for
     * the Managable annotation and if it is present uses its domain attribute. If it
     * is not present the default domain specified in JmxConstants.JMX_DOMAIN is used.
     *
     * @return a String
     */
    protected String getJMXDomainName()
    {
        Managable annotation = getClass().getAnnotation(Managable.class);
        if (annotation == null)
        {
            _log.debug("No Managable annotation so using default domain " +
                    JmxConstants.JMX_DOMAIN);
            return JmxConstants.JMX_DOMAIN;
        }
        else
        {
            final String domain = annotation.domain();
            _log.debug("JMX Domain specified as: " + domain);
            return domain;
        }
    }
}
