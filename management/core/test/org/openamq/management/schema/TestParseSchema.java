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
import org.openamq.management.ManagementConnection;
import org.openamq.management.messaging.CMLMessageFactory;
import org.openamq.schema.cml.ClassDocument;
import org.openamq.schema.cml.CmlDocument;
import org.openamq.schema.cml.SchemaDocument;

import javax.jms.TextMessage;

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
            _con = new ManagementConnection(args[0], Integer.parseInt(args[1]), args[2], args[3],
                                            args[4]);

            _con.connect();
            TextMessage tm = _con.sendRequest(CMLMessageFactory.createSchemaRequest());
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
                catch (Exception e)
                {
                    _logger.error("Error closing connection: " + e);
                }
            }
        }

    }
}
