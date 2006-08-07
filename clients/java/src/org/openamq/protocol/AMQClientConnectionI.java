package org.openamq.protocol;

import java.io.*;
import java.net.*;
import java.util.*;

import org.apache.log4j.Logger;

import org.openamq.*;
import org.openamq.framing.*;
import org.openamq.transport.*;

public class AMQClientConnectionI {
    private static final Logger
        _logger = Logger.getLogger(AMQClientConnectionI.class);

    AMQProtocolHandler
        aph;
    AMQSocketConnection
        asc;
    AMQConnectionState
        conState;
    Thread
        conThread;
    AuthData
        authData;
    ConnectionTuneBody
        ctb;
    String
        host,
        virtualHost,
        fullHost,
        instance;
    TreeMap
        sessions;
    int
        port,
        timeout;

    public static AuthData authPlain(String user, String password) {
        return new AuthDataPlain(user, password);
    }

    public AMQClientConnectionI() {
        setHost(AMQConstant.DEFAULT_HOST);
        setVirtualHost(AMQConstant.DEFAULT_VHOST);
        setAuthData(null);
        setClientInstance("");
        setTimeout(-1);
    }

    public AMQClientConnectionI(String host, String virtualHost, AuthData authData, String instance,
        int timeout) {

        setHost(host);
        setVirtualHost(virtualHost);
        setAuthData(authData);
        setClientInstance(instance);
        setTimeout(timeout);
    }

    public void connect() throws Exception {
        aph = new AMQProtocolHandler(this);
        asc = new AMQSocketConnection();
        conState = new AMQConnectionState(this);
        sessions = new TreeMap();

        asc.connect(aph, new InetSocketAddress(host, port));
        conState.waitConnectionOpened();
    }

    public void setHost(String fullHost) {
        StringTokenizer
            st = new StringTokenizer(fullHost, ":");

        this.fullHost = fullHost;

        if (st.hasMoreTokens())
            host = st.nextToken();

        if (st.hasMoreTokens())
            port = Integer.parseInt(st.nextToken());
        else
            port = AMQConstant.DEFAULT_PORT;
    }

    public String getHost() {
        return fullHost;
    }

    public void setVirtualHost(String virtualHost) {
        this.virtualHost = virtualHost;
    }

    public String getVirtualHost() {
        return virtualHost;
    }

    public void setAuthData(AuthData authData) {
        this.authData = authData;
    }

    public AuthData getAuthData() {
        return authData;
    }

    public void setClientInstance(String instance) {
        this.instance = instance;
    }

    public String getClientInstance() {
        return instance;
    }

    public void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    public int getTimeout() {
        return timeout;
    }

    public AMQClientSessionI createSession(AMQClientSessionI acs) throws Exception {
        if (conState != null && conState.isConnectionOpened()) {
            acs.initialize(this);
            acs.start();
            acs.getSessionState().waitChannelOpened();

            return acs;
        } else {
            throw new IllegalStateException("Connection is not opened");
        }
    }

    void start() {
        conThread = new Thread(conState);
        conThread.start();
    }

    public void close(int replyCode, String replyText, int classId, int methodId) throws Exception {
        disableSessions();

        if (replyCode != AMQConstant.REPLY_SUCCESS)
            _logger.error(replyText);
        if (conState.isConnectionOpening() || conState.isConnectionOpened())
            writeFrame(ConnectionCloseBody.createAMQFrame(0, replyCode, replyText, classId, methodId));
    }

    void disableSessions()  {
        synchronized (sessions) {
            if (_logger.isDebugEnabled())
                _logger.debug("Disabling sessions for connection: " + instance);

            while (!sessions.isEmpty())
                ((AMQClientSessionI)sessions.get(sessions.firstKey())).disableSession();
        }
    }

    void dispatchFrame(AMQFrame frame) {
        AMQClientSessionI
            acs = null;

        synchronized (sessions) {
            acs = (AMQClientSessionI)sessions.get(frame.channel);
        }

        if (acs != null) {
            AMQChannelState
                ass = acs.getSessionState();

            if (ass.isChannelOpening() || ass.isChannelOpened()) {
                if (_logger.isDebugEnabled()) {
                    _logger.debug("Dispatching frame to session");
                    _logger.debug("Connection: " + this);
                    _logger.debug("Session: " + acs.getSessionId());
                }

                ass.setExternalEvent(frame);
            } else {
                _logger.warn("Receiving frame on a closed session");
                _logger.warn("Session: " + acs.getSessionId());
                _logger.warn(frame);
            }
        } else {
            _logger.warn("Not handling channel id: " + frame.channel);
        }
    }

    public void writeFrame(AMQDataBlock frame) throws Exception
    {
        aph.writeFrame(null, frame);
    }

    AMQConnectionState getConnectionState() {
       return conState; 
    }

    AMQProtocolHandler getProtocolHandler() {
        return aph;
    }

    void setConnectionTuneData(ConnectionTuneBody ctb) {
        this.ctb = ctb;
        aph.setupHeartbeats(ctb.heartbeat);
    }

    ConnectionTuneBody getConnectionTuneData() {
        return ctb;
    }

    void putSession(AMQClientSessionI acs) {
        synchronized (sessions) {
            sessions.put(acs.getSessionId(), acs);
        }
    }

    void removeSession(int sessionId) {
        synchronized (sessions) {
            sessions.remove(sessionId);
        }
    }

    boolean containsSession(int sessionId) {
        synchronized (sessions) {
            return sessions.containsKey(sessionId);
        }
    }
} 
