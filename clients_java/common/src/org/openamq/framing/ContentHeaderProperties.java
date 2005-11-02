/*****************************************************************************
 * Filename    : ContentHeaderProperties.java
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
public interface ContentHeaderProperties
{
    void writePropertyListPayload(ByteBuffer buffer);

    void populatePropertiesFromBuffer(ByteBuffer buffer, int propertyFlags)
        throws AMQFrameDecodingException;

    int getPropertyListSize();

    int getPropertyFlags();
}
