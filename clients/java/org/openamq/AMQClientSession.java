package org.openamq;

import java.util.concurrent.*;

import org.apache.mina.common.ByteBuffer;
import org.apache.log4j.Logger;

import org.openamq.framing.*;
import org.openamq.protocol.*;

public class AMQClientSession {
    private static final Logger
        _logger = Logger.getLogger(AMQClientSession.class);
    static int
        sessions = 0;

    AMQProtocolHandler
        aph;
    AMQClientConnection
        acc;
    amqchannelstate
        sessionState;
    Thread
        sessionThread;
    LinkedBlockingQueue
        messages;
    int
        sessionId,
        prefetch;

    AMQClientSession(AMQClientConnection acc) {
        this(acc, AMQConstant.DEFAULT_PREFETCH);
    }

    AMQClientSession(AMQClientConnection acc, int prefetch) {
        this.acc = acc;
        this.prefetch = prefetch;
        this.sessionId = ++sessions;
        aph = acc.getProtocolHandler();
        sessionThread = null;
        sessionState = new amqchannelstate(acc, this);
        messages = new LinkedBlockingQueue();
    }

    public void channelFlow(boolean active) throws Exception {
        writeFrame(ChannelFlowBody.createAMQFrame(sessionId, active));
    }

    public void accessRequest(String realm) throws Exception {
        accessRequest(realm, false, false, true, true, true);
    }

    public void accessRequest(String realm, boolean exclusive, boolean passive, boolean active,
        boolean write, boolean read) throws Exception {

        writeFrame(AccessRequestBody.createAMQFrame(sessionId, realm, exclusive, passive,
            active, write, read));
    }

    public void exchangeDeclare(String exchange, String type) throws Exception {
        exchangeDeclare(exchange, type, false, true, false, false, null) throws Exception {
    }

    public void exchangeDeclare(String exchange, String type, boolean passive, boolean durable, boolean autoDelete,
        boolean internal, FieldTable arguments) throws Exception {

        writeFrame(ExchangeDeclareBody.createAMQFrame(sessionId, 0, exchange, type, passive, durable, autoDelete,
            internal, arguments));
    }

    public void exchangeDelete(String exchange) throws Exception {
        exchangeDelete(exchange, false);
    }

    public void exchangeDelete(String exchange, boolean ifUnused) throws Exception {
        writeFrame(ExchangeDeleteBody.createAMQFrame(sessionId, 0, exchange, ifUnused));
    }

    public void queueDeclare(String queue) throws Exception {
        queueDeclare(queue, false, true, false, false);
    }

    public void queueDeclare(String queue, boolean passive, boolean durable, boolean exclusive,
        boolean autoDelete) throws Exception {

        writeFrame(QueueDeclareBody.createAMQFrame(sessionId, 0, queue, passive, durable,
            exclusive, autoDelete, null));
    }

    public void queueBind(String queue, String routingKey) throws Exception {
        queueBind(queue, "", routingKey, null);
    }

    public void queueBind(String queue, String exchange, String routingKey) throws Exception {
        queueBind(queue, exchange, routingKey, null);
    }

    public void queueBind(String queue, String exchange, String routingKey, FieldTable arguments) throws Exception {
        writeFrame(QueueBindBody.createAMQFrame(sessionId, 0, queue, exchange, routingKey, arguments));
    }

    public void queuePurge(String queue) throws Exception {
        writeFrame(QueuePurgeBody.createAMQFrame(sessionId, 0, queue));
    }

    public void queueDelete(String queue) throws Exception {
        queueDelete(queue, false, false);
    }

    public void queueDelete(String queue, boolean ifUnused, boolean ifEmpty) throws Exception {
        writeFrame(QueueDeleteBody.createAMQFrame(sessionId, 0, queue, ifUnused, ifEmpty));
    }

    public void basicConsume(String queue) throws Exception {
        basicConsume(queue, "", 0, 0, false, true, false);
    }

    public void basicConsume(String queue, String consumerTag, long prefetchSize, int prefetchCount,
        boolean noLocal, boolean noAck, boolean exclusive) throws Exception {

        writeFrame(BasicConsumeBody.createAMQFrame(sessionId, 0, queue, consumerTag, prefetchSize,
            prefetchCount, noLocal, noAck, exclusive));
    }

    public void basicCancel(String consumerTag) throws Exception {
        writeFrame(BasicCancelBody.createAMQFrame(sessionId, consumerTag));
    }

    public void basicGet(String queue) throws Exception {
        basicGet(queue, true);
    }

    public void basicGet(String queue, boolean noAck) throws Exception {
        writeFrame(BasicGetBody.createAMQFrame(sessionId, 0, queue, noAck));
    }

    public void basicAck(long deliveryTag) throws Exception {
        basicAck(deliveryTag, true);
    }

    public void basicAck(long deliveryTag, boolean multiple) throws Exception {
        writeFrame(BasicAckBody.createAMQFrame(sessionId, deliveryTag, multiple));
    }

    public void basicAck(long deliveryTag) throws Exception {
        basicAck(deliveryTag, false);
    }

    public void basicAck(long deliveryTag, boolean multiple) throws Exception {
        writeFrame(BasicRejectBody.createAMQFrame(sessionId, deliveryTag, requeue));
    }

    public void basicPublish(AMQMessage message, String routingKey) throws Exception {
        basicPublish(message, "", routingKey, false, false);
    }

    public void basicPublish(AMQMessage message, String exchange, String routingKey) throws Exception {
        basicPublish(message, exchange, routingKey, false, false);
    }

    public void basicPublish(AMQMessage message, String exchange, String routingKey,
        boolean mandatory, boolean immediate) throws Exception {

        AMQFrame[]
            frames = new AMQFrame[2];
        ByteBuffer
            body = message.getBody();
        int
            frameLimit = (int)Math.min(acc.getConnectionTuneData().frameMax - 1, Integer.MAX_VALUE);

        frames[0] = BasicPublishBody.createAMQFrame(sessionId, 0, exchange, routingKey,
            mandatory, immediate);
        frames[1] = ContentHeaderBody.createAMQFrame(sessionId, 6, 0, message.getHeaders(), body.limit());

        writeFrame(new CompositeAMQDataBlock(frames, body, frameLimit));
    }

    public AMQMessage getMessage() throws Exception {
        _logger.debug("Waiting for message");
        return (AMQMessage)messages.take();
    }

    public AMQMessage createMessage() {
        return new AMQMessage();
    }

    public void start() {
        sessionThread = new Thread(sessionState);
        sessionThread.start();
    }

    public void close() throws Exception {
        close(AMQConstant.REPLY_SUCCESS, "Client closing session.", 0, 0);
    }

    public void close(int replyCode, String replyText, int classId, int methodId) throws Exception {
        if (replyCode != AMQConstant.REPLY_SUCCESS)
            _logger.error(replyText);
        writeFrame(ChannelCloseBody.createAMQFrame(sessionId, replyCode, replyText, classId, methodId));
    }

    public void messageReceived(AMQMessage message) {
        synchronized (messages) {
            messages.add(message);
        }
    }

    public void writeFrame(AMQDataBlock frame) throws Exception
    {
        aph.writeFrame(this, frame);
    }

    public amqchannelstate getSessionState() {
       return sessionState;
    }

    public int getSessionId() {
        return sessionId;
    }

    public int getPrefetch() {
        return prefetch;
    }
}
 
