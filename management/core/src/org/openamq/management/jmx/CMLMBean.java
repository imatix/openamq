/*****************************************************************************
 * Filename    : CMLMBean.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.management.jmx;

import org.openamq.schema.cml.InspectReplyDocument;

import javax.management.*;
import javax.management.openmbean.OpenMBeanInfoSupport;
import javax.management.openmbean.OpenMBeanInfo;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class CMLMBean implements DynamicMBean
{
    private OpenMBeanInfoSupport _mbeanInfo;

    private InspectReplyDocument.InspectReply _inspectReply;

    public CMLMBean(OpenMBeanInfoSupport mbeanInfo, InspectReplyDocument.InspectReply inspectReply)
    {
        _mbeanInfo = mbeanInfo;
        _inspectReply = inspectReply;
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
