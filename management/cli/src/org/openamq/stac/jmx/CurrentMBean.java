package org.openamq.stac.jmx;

import org.openamq.AMQException;

import javax.management.*;
import java.util.HashMap;
import java.util.Map;
import java.util.SortedSet;
import java.util.TreeSet;

/**
 * Stores information about the "current" MBean. This data is used when navigating the hierarchy.
 *
 * For example, we need to map between a name and an object id, and this stores that link.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class CurrentMBean
{
    private MBeanServerConnection _mbeanServerConnection;

    public static final String PARENT_ATTRIBUTE = "__parent";

    /**
     * Maps names to ObjectNames. Used for efficiency to avoid iterating through all names when doing a CD command.
     */
    private Map<String, ObjectName> _name2ObjectNameMap = new HashMap<String, ObjectName>();

    private ObjectName _mbeanObjectName;

    private MBeanInfo _mbeanInfo;

    public CurrentMBean(MBeanServerConnection mbeanServerConnection)
    {
        _mbeanServerConnection = mbeanServerConnection;
    }

    public void changeMBean(ObjectName objectName) throws AMQException
    {
        try
        {
            _mbeanInfo = _mbeanServerConnection.getMBeanInfo(objectName);
        }
        catch (Exception e)
        {
            throw new AMQException("Unable to look up MBean for object name " + objectName + ": " + e, e);
        }
        _mbeanObjectName = objectName;
    }

    public ObjectName getMBeanObjectName()
    {
        return _mbeanObjectName;
    }

    public MBeanInfo getMBeanInfo()
    {
        return _mbeanInfo;
    }

    public Object getAttributeValue(String name) throws AMQException
    {
        try
        {
            return _mbeanServerConnection.getAttribute(_mbeanObjectName, name);
        }
        catch (Exception e)
        {
            throw new AMQException("Unable to read attribute value for attribute name " + name, e);
        }
    }

    /**
     * Get the objects (i.e. "directories") under the current mbean, ordered alphabetically. This method also
     * refreshes the cache that maps from name to ObjectName (this saves iterating through the attributes again).
     * @return a set containing the attribute infos, sorted by name
     */
    public SortedSet<MBeanAttributeInfo> getOrderedObjects() throws AMQException
    {
        TreeSet<MBeanAttributeInfo> attributes = new TreeSet<MBeanAttributeInfo>(new MBeanAttributeInfoComparator());
        _name2ObjectNameMap.clear();
        for (MBeanAttributeInfo ai : _mbeanInfo.getAttributes())
        {
            String type = ai.getType();

            if ("javax.management.ObjectName".equals(type))
            {
                _name2ObjectNameMap.put(ai.getName(), (ObjectName)getAttributeValue(ai.getName()));
                attributes.add(ai);
            }
        }
        return attributes;
    }

    public void refreshNameToObjectNameMap() throws AMQException
    {
        _name2ObjectNameMap.clear();
        for (MBeanAttributeInfo ai : _mbeanInfo.getAttributes())
        {
            final String type = ai.getType();

            if ("javax.management.ObjectName".equals(type))
            {
                _name2ObjectNameMap.put(ai.getName(), (ObjectName)getAttributeValue(ai.getName()));
            }
        }
    }

    /**
     * Gets an object name, given the "display name"
     * @param name the display name (usually returned to the user when he does an ls()
     * @return the object name
     */
    public ObjectName getObjectNameByName(String name)
    {
        return _name2ObjectNameMap.get(name);
    }

    public SortedSet<MBeanAttributeInfo> getOrderedAttributes()
    {
        TreeSet<MBeanAttributeInfo> attributes = new TreeSet<MBeanAttributeInfo>(new MBeanAttributeInfoComparator());
        for (MBeanAttributeInfo ai : _mbeanInfo.getAttributes())
        {
            String type = ai.getType();
            if (!"javax.management.ObjectName".equals(type))
            {
                attributes.add(ai);
            }
        }
        return attributes;
    }
}
