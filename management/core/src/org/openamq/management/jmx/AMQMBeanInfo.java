package org.openamq.management.jmx;

import javax.management.openmbean.OpenMBeanAttributeInfo;
import java.util.Map;
import java.util.HashMap;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class AMQMBeanInfo
{
    private Map<String, OpenMBeanAttributeInfo> _name2AttributeInfoMap = new HashMap<String, OpenMBeanAttributeInfo>();

    public AMQMBeanInfo(OpenMBeanAttributeInfo[] attributeInfos)
    {
        for (OpenMBeanAttributeInfo attributeInfo: attributeInfos)
        {
            _name2AttributeInfoMap.put(attributeInfo.getName(), attributeInfo);
        }
    }

    public OpenMBeanAttributeInfo getAttributeInfo(String name)
    {
        return _name2AttributeInfoMap.get(name);
    }
}
