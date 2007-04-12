package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

public class ContentHeaderPropertiesFactory
{
    private static final ContentHeaderPropertiesFactory _instance = new ContentHeaderPropertiesFactory();

    public static ContentHeaderPropertiesFactory getInstance()
    {
        return _instance;
    }

    private ContentHeaderPropertiesFactory()
    {
    }

    public ContentHeaderProperties createContentHeaderProperties(int classId, int propertyFlags,
                                                                 ByteBuffer buffer, int size)
             throws AMQFrameDecodingException
    {
        ContentHeaderProperties properties;
        switch (classId)
        {
            case BasicConsumeBody.CLASS_ID:
                properties = new BasicContentHeaderProperties();
                break;
            default:
                throw new AMQFrameDecodingException("Unsupport content header class id: " + classId);
        }
        properties.populatePropertiesFromBuffer(buffer, propertyFlags, size);
        return properties;
    }
}
