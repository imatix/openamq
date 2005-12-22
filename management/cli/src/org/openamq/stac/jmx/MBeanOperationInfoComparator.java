package org.openamq.stac.jmx;

import javax.management.MBeanOperationInfo;
import java.util.Comparator;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class MBeanOperationInfoComparator implements Comparator<MBeanOperationInfo>
{
    public int compare(MBeanOperationInfo o1, MBeanOperationInfo o2)
    {
        return o1.getName().compareTo(o2.getName());
    }
}
