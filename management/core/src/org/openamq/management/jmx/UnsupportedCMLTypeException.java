/*****************************************************************************
 * Filename    : UnsupportedCMLTypeException.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.management.jmx;

import org.openamq.AMQException;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class UnsupportedCMLTypeException extends AMQException
{
    private String _type;

    public UnsupportedCMLTypeException(String type)
    {
        super("CML type " + type + " is unsupported by the JMX layer");
        _type = type;
    }

    public String getType()
    {
        return _type;
    }
}
