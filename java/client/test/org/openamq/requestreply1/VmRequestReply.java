/*****************************************************************************
 * Filename    : VmRequestReply.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.requestreply1;

import org.openamq.vmbroker.VmPipeBroker;
import org.junit.Test;
import org.apache.log4j.Logger;
import junit.framework.JUnit4TestAdapter;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class VmRequestReply extends VmPipeBroker
{
    private static final Logger _logger = Logger.getLogger(VmRequestReply.class);

    @Test
    public void simpleClient() throws Exception
    {
        ServiceProvidingClient serviceProvider = new ServiceProvidingClient("foo:123", "guest", "guest",
                                                                            "serviceProvidingClient", "/test",
                                                                            "serviceQ");

        ServiceRequestingClient serviceRequester = new ServiceRequestingClient("foo:123", "myClient", "guest", "guest",
                                                                               "/test", "serviceQ", 5000, 512);

        serviceProvider.run();
        Object waiter = new Object();
        serviceRequester.run(waiter);
        synchronized (waiter)
        {
            while (!serviceRequester.isCompleted())
            {
                waiter.wait();
            }
        }
    }

    public static junit.framework.Test suite()
    {
        return new JUnit4TestAdapter(VmRequestReply.class);
    }

    public static void main(String[] args)
    {
        VmRequestReply rr = new VmRequestReply();
        try
        {
            rr.initialiseBroker();
            rr.simpleClient();
        }
        catch (Exception e)
        {
            _logger.error("Error: " + e, e);
        }
    }
}
