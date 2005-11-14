/*****************************************************************************
 * Filename    : ManagementDestination.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.management.messaging;

import org.openamq.client.AMQDestination;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class ManagementDestination extends AMQDestination
{
    public ManagementDestination()
    {
        super("amq.system", "system", "amq.console");
    }

    public String getEncodedName()
    {
        return null;
    }

    public String getRoutingKey()
    {
        return getDestinationName();
    }

    // TODO: remove
    public boolean isExclusive()
    {
        return false;
    }
}
