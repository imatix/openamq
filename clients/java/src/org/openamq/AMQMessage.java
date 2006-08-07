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
 *
 * The consumer for this message can be identified by calling the consumerTag
 * function. If the consumerTag is null, this message was returned, and the replyCode
 * and replyText functions can provide the details.
 *
 * Messages can be acknowledged by calling the acknowledge functions.
 */
public class AMQMessage {
    AMQClientSession
        session;
    AMQMessageI
        impl;

    AMQMessage(AMQClientSession session, AMQMessageI impl) {
        this.session = session;
        this.impl = impl;
    }

    /**
     * Set message payload.
     *
     * @param data text payload.
     */
    public void setBody(String data) {
        impl.setBody(data);
    }

    /**
     * Set message payload.
     *
     * @param data bytes payload.
     */
    public void setBody(byte[] data) {
        impl.setBody(data);
    }

    /**
     * Set message payload.
     *
     * @param data ByteBuffer payload.
     */
    public void setBody(ByteBuffer data) {
        impl.setBody(data);
    }

    /**
     * Release message payload.
     */
    public void clearBody() {
        impl.clearBody();
    }

    /**
     * Get native message payload.
     *
     * @return ByteBuffer payload.
     */
    public ByteBuffer getBody() {
        return impl.getBody();
    }

    /**
     * Copy message payload to a new byte[].
     *
     * @return ByteBuffer payload.
     */
    public byte[] getBytes() {
        ByteBuffer
            data = getBody();
        byte[]
            bytes = new byte[data.remaining()];

        data.get(bytes);
        data.flip();

        return bytes;
    }

    /**
     * Set message headers.
     *
     * @param headers routing and identification information.
     */
    public void setHeaders(BasicContentHeaderProperties headers) {
        impl.setHeaders(headers);
    }

    /**
     * Reset message headers.
     */
    public void clearHeaders() {
        impl.clearHeaders();
    }

    /**
     * Get message headers.
     *
     * @return message headers.
     */
    public BasicContentHeaderProperties getHeaders() {
        return impl.getHeaders();
    }

    /**
     * Get the consumer tag.
     *
     * @return the consumer tag or null if this is a returned message.
     */
    public String consumerTag() {
        return impl.consumerTag();
    }

    /**
     * Get the delivery tag.
     *
     * @return the delivery tag if the consumerTag != null (otherwise returns -1).
     */
    public long deliveryTag() {
        return impl.deliveryTag();
    }

    /**
     * Get the reply code.
     *
     * @return the reply code if the consumerTag == null (otherwise returns -1).
     */
    public int replyCode() {
        return impl.replyCode();
    }

    /**
     * Get the reply message.
     *
     * @return the reply message if the consumerTag == null (otherwise returns null).
     */
    public String replyText() {
        return impl.replyText();
    }

    public void acknowledge() throws Exception {
        session.basicAck(deliveryTag());
    }

    public void acknowledgeAll() throws Exception {
        session.basicAck(deliveryTag(), true);
    }
}
