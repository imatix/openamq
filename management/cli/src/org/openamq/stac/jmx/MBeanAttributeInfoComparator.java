package org.openamq.stac.jmx;

import javax.management.MBeanAttributeInfo;
import java.util.Comparator;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class MBeanAttributeInfoComparator implements Comparator<MBeanAttributeInfo>
{
    public int compare(MBeanAttributeInfo o1, MBeanAttributeInfo o2)
    {
        return o1.getName().compareTo(o2.getName());
    }    
}
