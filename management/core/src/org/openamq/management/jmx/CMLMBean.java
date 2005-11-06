/*****************************************************************************
 * Filename    : CMLMBean.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.management.jmx;

import javax.management.*;
import javax.management.openmbean.OpenMBeanInfoSupport;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class CMLMBean implements DynamicMBean
{
    private OpenMBeanInfoSupport _mbeanInfo;

    public CMLMBean(OpenMBeanInfoSupport mbeanInfo)
    {
        _mbeanInfo = mbeanInfo;
    }

    public Object getAttribute(String attribute) throws AttributeNotFoundException, MBeanException, ReflectionException
    {
        return null;
    }

    public void setAttribute(Attribute attribute) throws AttributeNotFoundException, InvalidAttributeValueException,
                                                         MBeanException, ReflectionException
    {

    }

    public AttributeList getAttributes(String[] attributes)
    {
        return null;
    }

    public AttributeList setAttributes(AttributeList attributes)
    {
        return null;
    }

    public Object invoke(String actionName, Object params[], String signature[]) throws MBeanException,
                                                                                        ReflectionException
    {
        return null;
    }

    public MBeanInfo getMBeanInfo()
    {
        return _mbeanInfo;
    }
}
