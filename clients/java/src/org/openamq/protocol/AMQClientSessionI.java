package org.openamq.protocol;

import java.util.LinkedList;

import org.apache.mina.common.ByteBuffer;
import org.apache.log4j.Logger;

import org.openamq.*;
import org.openamq.framing.*;

public class AMQClientSessionI {
    private static final Logger
        _logger = Logger.getLogger(AMQClientSessionI.class);
    static int
        sessions = 0;

    AMQProtocolHandler
        aph;
    AMQClientConnectionI
        acc;
    AMQChannelState
        sessionState;
    Thread
        sessionThread;
    LinkedList
        messages;
    int
        sessionId;

    void initialize(AMQClientConnectionI acc) {
        this.acc = acc;
        this.sessionId = ++sessions;
        aph = acc.getProtocolHandler();
        sessionThread = null;
        sessionState = new AMQChannelState(acc, this);
        messages = new LinkedList();

        acc.putSession(this);
    }

    public int getSessionId() {
        return sessionId;
    }

    public AMQMessage getMessage() throws Exception {
        _logger.debug("Waiting for message");
        synchronized (messages) {
            try {
                if (messages.isEmpty() && acc.containsSession(sessionId))
                    messages.wait();
            } catch (InterruptedException e) {}
            if (!messages.isEmpty())
                return (AMQMessage)messages.removeFirst();
            else
                throw new AMQException("Cannot get message");
        }
    }

    public AMQMessage createMessage() {
        return new AMQMessage();
    }

    void start() {
        sessionThread = new Thread(sessionState);
        sessionThread.start();
    }

    public void close(int replyCode, String replyText, int classId, int methodId) throws Exception {
        disableSession();

        if (replyCode != AMQConstant.REPLY_SUCCESS)
            _logger.error(replyText);
        if (sessionState.isChannelOpening() || sessionState.isChannelOpened())
            writeFrame(ChannelCloseBody.createAMQFrame(sessionId, replyCode, replyText, classId, methodId));
    }

    void disableSession() {
        synchronized (messages) {
            _logger.debug("Disabling session: " + sessionId);
            acc.removeSession(sessionId);
            messages.notifyAll();
        }
    }

    void messageReceived(AMQMessageI message) {
        synchronized (messages) {
            messages.add(message);
            messages.notifyAll();
        }
    }

    public CompositeAMQDataBlock preparePublish(AMQMessage message, String exchange, String routingKey,
        boolean mandatory, boolean immediate) throws Exception {

        AMQFrame[]
            frames = new AMQFrame[2];
        ByteBuffer
            body = message.getBody();
        int
            frameLimit = (int)Math.min(acc.getConnectionTuneData().frameMax - 1, Integer.MAX_VALUE);

        frames[0] = BasicPublishBody.createAMQFrame(getSessionId(), 0, exchange, routingKey,
            mandatory, immediate);
        frames[1] = BasicContentHeaderBody.createAMQFrame(getSessionId(), BasicConsumeBody.CLASS_ID, 0, message.getHeaders(), body.limit());

        return new CompositeAMQDataBlock(frames, body, frameLimit);
    }

    public void writeFrame(AMQDataBlock frame) throws Exception
    {
        aph.writeFrame(this, frame);
    }

    AMQChannelState getSessionState() {
       return sessionState;
    }
}
 
