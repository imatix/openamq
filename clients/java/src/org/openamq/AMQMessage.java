/*---------------------------------------------------------------------------
    AMQMessage.java

    Copyright (c) 2005-2006 iMatix Corporation
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.
    
    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
    
    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 *---------------------------------------------------------------------------*/

package org.openamq;

import org.apache.mina.common.ByteBuffer;

import org.openamq.protocol.AMQMessageI;
import org.openamq.framing.BasicContentHeaderProperties;

/**
 * AMQMessage objects are sent and received from AMQSessions.
 *
 * An AMQMessage object carries properties and payload. The payload carries bytes
 * which are interpreted by the client application.
 */
public class AMQMessage extends AMQMessageI {
    public AMQMessage() {
        super();
    }

    /**
     * Set message payload.
     *
     * @param data text payload.
     */
    public void setBody(String data) {
        super.setBody(data);
    }

    /**
     * Set message payload.
     *
     * @param data bytes payload.
     */
    public void setBody(byte[] data) {
        super.setBody(data);
    }

    /**
     * Set message payload.
     *
     * @param data ByteBuffer payload.
     */
    public void setBody(ByteBuffer data) {
        super.setBody(data);
    }

    /**
     * Release message payload.
     */
    public void clearBody() {
        super.clearBody();
    }

    /**
     * Get message payload.
     *
     * @return ByteBuffer payload.
     */
    public ByteBuffer getBody() {
        return super.getBody();
    }

    /**
     * Set message headers.
     *
     * @param headers routing and identification information.
     */
    public void setHeaders(BasicContentHeaderProperties headers) {
        super.setHeaders(headers);
    }

    /**
     * Reset message headers.
     */
    public void clearHeaders() {
        super.clearHeaders();
    }

    /**
     * Get message headers.
     *
     * @return message headers.
     */
    public BasicContentHeaderProperties getHeaders() {
        return super.getHeaders();
    }
}
