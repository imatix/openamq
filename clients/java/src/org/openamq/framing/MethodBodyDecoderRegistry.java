package org.openamq.framing;

import java.util.Map;
import java.util.HashMap;
import org.apache.log4j.Logger;
import org.openamq.AMQException;

public final class MethodBodyDecoderRegistry
{
    private static final Logger _log = Logger.getLogger(MethodBodyDecoderRegistry.class);

    private static final Map _classMethodProductToMethodBodyMap = new HashMap();

    static
    {
        MainRegistry.register(_classMethodProductToMethodBodyMap);         
    }

    public static AMQMethodBody get(int clazz, int method) throws AMQFrameDecodingException
    {
	Class bodyClass = (Class) _classMethodProductToMethodBodyMap.get(new Integer(clazz * 1000 + method));
	if (bodyClass != null)
	{
	    try
	    {
	        return (AMQMethodBody) bodyClass.newInstance();
	    }
	    catch (Exception e)
	    {
	    	throw new AMQFrameDecodingException("Unable to instantiate body class for class " + clazz + " and method " + method + ": " + e, e);
	    }
	}
	else
	{
	    throw new AMQFrameDecodingException("Unable to find a suitable decoder for class " + clazz + " and method " + method);
	}    
    }
}
