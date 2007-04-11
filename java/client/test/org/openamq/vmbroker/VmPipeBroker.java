/*****************************************************************************
 * Filename    : VmPipeBroker.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.vmbroker;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.log4j.Logger;
import org.apache.mina.common.IoAcceptor;
import org.apache.mina.common.IoServiceConfig;
import org.apache.mina.transport.vmpipe.VmPipeAcceptor;
import org.apache.mina.transport.vmpipe.VmPipeAddress;
import org.junit.After;
import org.junit.Before;
import org.openamq.client.transport.TransportConnection;
import org.openamq.pool.ReadWriteThreadModel;
import org.openamq.server.protocol.AMQPProtocolProvider;
import org.openamq.server.registry.ApplicationRegistry;
import org.openamq.server.util.NullApplicationRegistry;
import org.openamq.transport.VmPipeTransportConnection;

/**
 * This class is a useful base class when you want to run a test where the
 * broker and the client(s) run in the same VM.
 *
 * By extending this class, the InVmBroker is started, and the client transport
 * is set to use InVmPipe rather than TCP/IP sockets. This means that clients
 * will run unmodified.
 *
 * If you extend this class but need to run the test outside of JUnit then you
 * should invoke initialiseBroker() before creating any clients.
 *
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class VmPipeBroker
{
    private static final Logger _logger = Logger.getLogger(VmPipeBroker.class);

    private IoAcceptor _acceptor;

    @Before
    public void initialiseBroker() throws Exception
    {
        try
        {
            ApplicationRegistry.initialise(new NullApplicationRegistry());
        }
        catch (ConfigurationException e)
        {
            _logger.error("Error configuring application: " + e, e);
            throw e;
        }

        _acceptor = new VmPipeAcceptor();
        IoServiceConfig config = _acceptor.getDefaultConfig();     

        config.setThreadModel(new ReadWriteThreadModel());
        _acceptor.bind(new VmPipeAddress(1),
                      new AMQPProtocolProvider().getHandler());

        _logger.info("InVM Blaze.AMQP listening on port " + 1);
        TransportConnection.setInstance(new VmPipeTransportConnection());
    }

    @After
    public void killBroker()
    {
        _acceptor.unbindAll();
        _acceptor = null;
    }
}
