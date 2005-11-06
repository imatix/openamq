/*****************************************************************************
 * Filename    : TestParseSchema.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.management.schema;

import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;
import org.openamq.client.AMQQueue;
import org.openamq.jms.Session;
import org.openamq.jms.MessageProducer;
import org.openamq.management.messaging.ManagementDestination;
import org.openamq.management.ManagementConnection;
import org.openamq.schema.cml.SchemaDocument;
import org.openamq.schema.cml.ClassDocument;
import org.openamq.schema.cml.CmlDocument;

import javax.jms.*;
import java.net.InetAddress;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class TestParseSchema
{
    private static final Logger _logger = Logger.getLogger(TestParseSchema.class);

    private static ManagementConnection _con;

    private static void parseCMLSchema(String xml) throws Exception
    {
        CmlDocument cmlDoc = CmlDocument.Factory.parse(xml);
        CmlDocument.Cml cml = cmlDoc.getCml();
        SchemaDocument.Schema schema = cml.getSchema();
        for (ClassDocument.Class classDefn: schema.getClass1List())
        {
            System.out.println("Class: " + classDefn.getName());
        }
    }

    public static void main(String[] args)
    {
        _logger.info("Starting...");

        if (args.length != 5)
        {
            System.out.println("Usage: host port username password vhost");
            System.exit(1);
        }
        try
        {
            InetAddress address = InetAddress.getLocalHost();
            _con = new ManagementConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                            args[4]);
            String schemaRequestMsg = "<?xml?><cml version = \"1.0\"><schema /></cml>";

            TextMessage tm = _con.sendRequest(schemaRequestMsg);
            parseCMLSchema(tm.getText());
            _logger.info("Closing management connection");
            _con.close();

            //_logger.info("Waiting...");
        }
        catch (Throwable t)
        {
            _logger.error("Fatal error: " + t, t);
        }
        finally
        {
            if (_con != null)
            {
                _logger.info("Closing connection");
                try
                {
                    _con.close();
                }
                catch (JMSException e)
                {
                    _logger.error("Error closing connection: " + e);
                }
            }
        }

    }
}
