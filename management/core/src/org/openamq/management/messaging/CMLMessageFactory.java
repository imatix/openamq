package org.openamq.management.messaging;

import org.openamq.schema.cml.CmlDocument;
import org.openamq.schema.cml.InspectDocument;

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
        cml.addNewSchema();
        return cmlDoc.toString();
    }

    public static String createInspectRequest(int objectId)
    {
        CmlDocument cmlDoc = CmlDocument.Factory.newInstance();
        CmlDocument.Cml cml = cmlDoc.addNewCml();
        cml.setVersion("1.0");
        InspectDocument.Inspect inspect = cml.addNewInspect();
        inspect.setObject2(objectId);
        return cmlDoc.toString();
    }
}
