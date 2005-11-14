package org.openamq.management.messaging;

import org.openamq.schema.cml.CmlDocument;
import org.openamq.schema.cml.InspectRequestType;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class CMLMessageFactory
{
    public static String createSchemaRequest()
    {
        CmlDocument cmlDoc = CmlDocument.Factory.newInstance();
        CmlDocument.Cml cml = cmlDoc.addNewCml();
        cml.setVersion("1.0");
        cml.addNewSchemaRequest();
        return cmlDoc.toString();
    }

    public static String createInspectRequest(int objectId)
    {
        CmlDocument cmlDoc = CmlDocument.Factory.newInstance();
        CmlDocument.Cml cml = cmlDoc.addNewCml();
        cml.setVersion("1.0");
        InspectRequestType inspect = cml.addNewInspectRequest();
        inspect.setObject(objectId);
        return cmlDoc.toString();
    }
}
