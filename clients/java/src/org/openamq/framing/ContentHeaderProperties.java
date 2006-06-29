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
 * There will be an implementation of this interface for each content type. All content types have associated
 * header properties and this provides a way to encode and decode them.
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface ContentHeaderProperties
{
    /**
     * Writes the property list to the buffer, in a suitably encoded form.
     * @param buffer The buffer to write to
     */
    void writePropertyListPayload(ByteBuffer buffer);

    /**
     * Populates the properties from buffer.
     * @param buffer The buffer to read from.
     * @param propertyFlags he property flags.
     * @throws AMQFrameDecodingException when the buffer does not contain valid data
     */
    void populatePropertiesFromBuffer(ByteBuffer buffer, int propertyFlags, int size)
        throws AMQFrameDecodingException;

    /**
     * @return the size of the encoded property list in bytes.
     */
    int getPropertyListSize();

    /**
     * Gets the property flags. Property flags indicate which properties are set in the list. The
     * position and meaning of each flag is defined in the protocol specification for the particular
     * content type with which these properties are associated.
     * @return flags
     */
    int getPropertyFlags();
}
