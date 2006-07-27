package org.openamq.protocol;

import org.apache.log4j.Logger;
import org.apache.mina.common.IoHandlerAdapter;
import org.apache.mina.common.IoSession;
import org.apache.mina.common.IdleStatus;
import org.apache.mina.filter.codec.ProtocolCodecFilter;

import org.openamq.AMQClientConnection;
import org.openamq.AMQClientSession;
import org.openamq.AMQConstant;
import org.openamq.AMQException;
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
        _logger.debug("Connection: " + connection.getClientInstance());
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
        _logger.debug("Connection: " + connection.getClientInstance());
    }

    public void sessionIdle(IoSession session, IdleStatus status) throws Exception {
        if(status == IdleStatus.WRITER_IDLE) {
            if (connectionState.isConnectionOpened())
                writeFrame(null, HeartbeatBody.FRAME);
        } else if(status == IdleStatus.READER_IDLE) {
            if (!(connectionState.isConnectionOpening() || connectionState.isConnectionOpened())) {
                closeProtocolSession();
            } else {
                Exception
                    e = new AMQException("Heartbeat timeout");
    
                _logger.error(e.getMessage());
                _logger.error("Connection: " + connection.getClientInstance());
    
                throw e;
            }
        }
    }

    public void exceptionCaught(IoSession session, Throwable t) throws Exception {
        _logger.error("Exception caught in protocol session", t);
        _logger.error("Connection: " + connection.getClientInstance());

        if (connectionState.isConnectionOpening() || connectionState.isConnectionOpened()) {
            connection.close(AMQConstant.INTERNAL_ERROR, t.getMessage(), 0, 0);
            connectionState.cleanup();
            connectionState.raiseException(0);
        }
    }

    public void messageReceived(IoSession session, Object message) throws Exception {
        AMQFrame
            frame = (AMQFrame)message;

        if (connectionState.isConnectionOpening() || connectionState.isConnectionOpened()) {
            if (_logger.isDebugEnabled()) {
                _logger.debug("Dispatching frame to connection");
                _logger.debug("Connection: " + connection.getClientInstance());
                _logger.debug(frame);
            }
            connectionState.setExternalEvent(frame);
        } else {
            _logger.warn("Receiving frame on a closed connection");
            _logger.warn("Connection: " + connection.getClientInstance());
            _logger.warn(frame);
        }
    }

    public void messageSent(IoSession session, Object message) throws Exception {
    }

    public void writeFrame(AMQClientSession session, AMQDataBlock frame) throws Exception
    {
        if (_logger.isDebugEnabled())
            _logger.debug("Writing: " + frame);

        if (connectionState.isConnectionOpening() || connectionState.isConnectionOpened()) {
            if (session == null || session.getSessionState().isChannelOpening() ||
                session.getSessionState().isChannelOpened()) {

                if (protocolSession.isConnected()) {
                    protocolSession.write(frame);
                } else {
                    _logger.debug("Trying to write a frame on a closed socket: " + frame);
                }
            } else {
                Exception
                    e = new IllegalStateException("Trying to write a frame from a closed session: " + frame);

                _logger.error(e.getMessage());
                _logger.error("Session: " + session.getSessionId());
                _logger.error(frame);

                throw e;
            }
        } else {
            Exception
                e = new IllegalStateException("Trying to write a frame from a closed connection");

            _logger.error(e.getMessage());
            _logger.error("Connection: " + connection.getClientInstance());
            _logger.error(frame);

            throw e;
        }
    }

    public void setupHeartbeats(int timeout) {
        protocolSession.setIdleTime(IdleStatus.WRITER_IDLE, timeout);
        protocolSession.setIdleTime(IdleStatus.READER_IDLE, timeout);
    }

    public void closeProtocolSession() {
        if (!protocolSession.isClosing())
            protocolSession.close().join();
    }
}
