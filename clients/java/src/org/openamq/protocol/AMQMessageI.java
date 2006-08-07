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
        delivery = null;
    }

    public BasicContentHeaderProperties getHeaders() {
        return headers;
    }

    public String consumerTag() {
        if (delivery != null && delivery instanceof BasicDeliverBody)
            return ((BasicDeliverBody)delivery).consumerTag;
        else
            return null;
    }

    public long deliveryTag() {
        if (delivery != null && delivery instanceof BasicDeliverBody)
            return ((BasicDeliverBody)delivery).deliveryTag;
        else
            return -1;
    }

    public int replyCode() {
        if (delivery != null && delivery instanceof BasicReturnBody)
            return ((BasicReturnBody)delivery).replyCode;
        else
            return -1;
    }

    public String replyText() {
        if (delivery != null && delivery instanceof BasicReturnBody)
            return ((BasicReturnBody)delivery).replyText;
        else
            return null;
    }

    void setDelivery(AMQMethodBody delivery) {
        this.delivery = delivery;
    }

    AMQMethodBody getDelivery() {
        return delivery;
    }
}
