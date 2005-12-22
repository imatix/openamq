package org.openamq.management.messaging;

import org.openamq.schema.cml.CmlDocument;
import org.openamq.schema.cml.InspectRequestType;
import org.openamq.schema.cml.MethodRequestType;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public class CMLMessageFactory
{
    public static String createSchemaRequest()
    {
        CmlDocument cmlDoc = CmlDocument.Factory.newInstance();
        CmlDocument.Cml cml = createStandardCml(cmlDoc);
        cml.addNewSchemaRequest();
        return cmlDoc.toString();
    }

    public static String createInspectRequest(int objectId)
    {
        CmlDocument cmlDoc = CmlDocument.Factory.newInstance();
        CmlDocument.Cml cml = createStandardCml(cmlDoc);
        InspectRequestType inspect = cml.addNewInspectRequest();
        inspect.setObject(objectId);
        return cmlDoc.toString();
    }

    public static String createMethodRequest(int objectId, String methodName)
    {
        CmlDocument cmlDoc = CmlDocument.Factory.newInstance();
        CmlDocument.Cml cml = createStandardCml(cmlDoc);
        MethodRequestType methodReq = cml.addNewMethodRequest();
        methodReq.setObject(objectId);
        methodReq.setName(methodName);
        return cmlDoc.toString();
    }

    private static CmlDocument.Cml createStandardCml(CmlDocument cmlDoc)
    {
        CmlDocument.Cml cml = cmlDoc.addNewCml();
        cml.setVersion("1.0");
        return cml;
    }
}
