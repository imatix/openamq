/*****************************************************************************
 * Filename    : ContentHeaderPropertiesFactory.java
 * Date Created: ${date}
 *****************************************************************************
 * (c) Copyright JP Morgan Chase Ltd 2005. All rights reserved. No part of
 * this program may be photocopied reproduced or translated to another
 * program language without prior written consent of JP Morgan Chase Ltd
 *****************************************************************************/
package org.openamq.framing;

import org.apache.mina.common.ByteBuffer;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
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
                                                                 ByteBuffer buffer)
             throws AMQFrameDecodingException
    {
        ContentHeaderProperties properties;
        switch (classId)
        {
            case 1:
                properties = new BasicContentHeaderProperties();
                break;
            case 7:
                properties = new JmsContentHeaderProperties();
                break;
            default:
                throw new AMQFrameDecodingException("Unsupport content header class id: " + classId);
        }
        properties.populatePropertiesFromBuffer(buffer, propertyFlags);
        return properties;
    }
}
