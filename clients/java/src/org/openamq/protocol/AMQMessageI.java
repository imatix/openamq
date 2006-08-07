package org.openamq.protocol;

import org.apache.mina.common.ByteBuffer;

import org.openamq.*;
import org.openamq.framing.*;

public class AMQMessageI {
    ByteBuffer
        data;
    BasicContentHeaderProperties
        headers;
    AMQMethodBody
        delivery;

    public AMQMessageI() {
        clearBody();
        clearHeaders();
        delivery = null;
    }

    public void setBody(String data) {
        setBody(ByteBuffer.wrap(data.getBytes()));
    }

    public void setBody(byte[] data) {
        setBody(ByteBuffer.wrap(data));
    }

    public void setBody(ByteBuffer data) {
        this.data = data;
    }

    public void clearBody() {
        if (data != null)
            data.release();
        setBody((ByteBuffer)null);
    }

    public ByteBuffer getBody() {
        return data;
    }

    public void setHeaders(BasicContentHeaderProperties headers) {
        this.headers = headers;
    }

    public void clearHeaders() {
        setHeaders(new BasicContentHeaderProperties());
    }

    public BasicContentHeaderProperties getHeaders() {
        return headers;
    }

    void setDelivery(AMQMethodBody delivery) {
        this.delivery = delivery;
    }

    AMQMethodBody getDelivery() {
        return delivery;
    }
} 
