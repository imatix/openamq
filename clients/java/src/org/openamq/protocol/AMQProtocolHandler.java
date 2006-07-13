package org.openamq.protocol;

import org.apache.log4j.Logger;
import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;
import org.apache.mina.common.IdleStatus;
import org.apache.mina.filter.codec.ProtocolCodecFilter;

import org.openamq.AMQClientConnection;
import org.openamq.AMQClientSession;
import org.openamq.AuthData;
import org.openamq.protocol.AMQConnectionState;
import org.openamq.framing.*;

import java.util.TreeMap;

public class AMQProtocolHandler extends IoHandlerAdapter {
    private static final Logger
        _logger = Logger.getLogger(AMQProtocolHandler.class);

    AMQClientConnection                 // The client connection for which this
        connection;                     //   protocol handler is created.
    IoSession                           // The MINA protocol session handles the
        protocolSession = null;         //   AMQ method flow.
    AMQConnectionState                  // The MINA handler relies on a libero
        connectionState;                //   state machine.
    TreeMap
        session2Filter = new TreeMap();

    public AMQProtocolHandler(AMQClientConnection connection) {
        this.connection = connection; 
        connectionState = null;
    }

    public void sessionCreated(IoSession session) throws Exception {
        // Set up the protocol session
        _logger.debug("Protocol session created.");
        protocolSession = session;

        ProtocolCodecFilter
            pcf = new ProtocolCodecFilter(new AMQProtocolCodecFactory());

        session.getFilterChain().addLast("AMQProtocolCodecFilter", pcf);
    }

    public void sessionOpened(IoSession session) throws Exception {
        connectionState = connection.getConnectionState();
        connection.start();
    }

    public void sessionClosed(IoSession session) throws Exception {
        _logger.debug("Protocol session closed.");
    }

    public void sessionIdle(IoSession session, IdleStatus status) throws Exception {
        if(status == IdleStatus.WRITER_IDLE)
            writeFrame(null, HeartbeatBody.FRAME);
        else if(status == IdleStatus.READER_IDLE) 
            connectionState.setExternalEvent(ConnectionCloseOkBody.createAMQFrame(0));
    }

    public void exceptionCaught(IoSession session, Throwable t) throws Exception {
        _logger.error("Exception caught in protocol session.", t);
    }

    public void messageReceived(IoSession session, Object message) throws Exception {
        AMQFrame
            frame = (AMQFrame)message;

        if (_logger.isDebugEnabled())
            _logger.debug("Frame received: " + frame);

        connectionState.setExternalEvent(frame);
    }

    public void messageSent(IoSession session, Object message) throws Exception {
    }

    public void writeFrame(AMQClientSession session, AMQDataBlock frame) throws Exception
    {
        if (_logger.isDebugEnabled())
            _logger.debug("Writing frame: " + frame);

        if (connectionState.isConnectionOpening() || connectionState.isConnectionOpened()) {
            if (session == null || session.getSessionState().isChannelOpening() ||
                session.getSessionState().isChannelOpened())

                if (protocolSession.isConnected())
                    protocolSession.write(frame);
                else
                    _logger.warn("Trying to write a frame on a closed socket: " + frame);
            else
                throw new IllegalStateException("Trying to write a frame from a closed session: " + frame);
        } else {
            throw new IllegalStateException("Trying to write a frame from a closed connection: " + frame);
        }
    }

    public void setupHeartbeats(int timeout) {
        protocolSession.setIdleTime(IdleStatus.WRITER_IDLE, timeout);
        protocolSession.setIdleTime(IdleStatus.READER_IDLE, timeout);
    }

    public void closeProtocolSession() {
        protocolSession.close().join();
    }
}
