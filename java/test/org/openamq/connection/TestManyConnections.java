/*****************************************************************************
 * Filename    : TestManyConnections.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.connection;

import org.openamq.vmbroker.VmPipeBroker;
import org.openamq.AMQException;
import org.openamq.client.AMQConnection;
import org.junit.Test;
import org.apache.log4j.Logger;
import junit.framework.JUnit4TestAdapter;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestManyConnections extends VmPipeBroker
{
    private static final Logger _log = Logger.getLogger(TestManyConnections.class);

    private AMQConnection[] _connections;

    private void createConnection(int index, String brokerHosts, String clientID, String username, String password,
                                  String vpath) throws AMQException
    {
        _connections[index] = new AMQConnection(brokerHosts, username, password,
                                                clientID, vpath);
    }

    private void createConnections(int count) throws AMQException
    {
        _connections = new AMQConnection[count];
        long startTime = System.currentTimeMillis();
        for (int i = 0; i < count; i++)
        {
            createConnection(i, "foo:123", "myClient" + i, "guest", "guest", "/test");
        }
        long endTime = System.currentTimeMillis();
        _log.info("Time to create " + count + " connections: " + (endTime - startTime) +
                  "ms");
    }

    @Test
    public void create10Connections() throws AMQException
    {
        createConnections(10);
    }

    @Test
    public void create50Connections() throws AMQException
    {
        createConnections(50);
    }

    @Test
    public void create100Connections() throws AMQException
    {
        createConnections(100);
    }

    @Test
    public void create250Connections() throws AMQException
    {
        createConnections(250);
    }

    @Test
    public void create500Connections() throws AMQException
    {
        createConnections(500);
    }

    @Test
    public void create1000Connections() throws AMQException
    {
        createConnections(1000);
    }

    @Test
    public void create5000Connections() throws AMQException
    {
        createConnections(5000);
    }

    public static junit.framework.Test suite()
    {
        return new JUnit4TestAdapter(TestManyConnections.class);
    }
}
