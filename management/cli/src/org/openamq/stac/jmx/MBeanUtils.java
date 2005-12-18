package org.openamq.stac.jmx;

import javax.management.MBeanAttributeInfo;

/**
 * Useful MBean methods. Also provides functionality for our CMLMBean
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class MBeanUtils
{
    public static boolean isHidden(MBeanAttributeInfo ai)
    {
        String hidden = (String) ai.getDescriptor().getFieldValue("hidden");
        return hidden != null && "true".equals(hidden);
    }
}
