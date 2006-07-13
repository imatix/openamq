package org.openamq;

import java.io.*;
import java.net.*;
import java.util.*;

import org.apache.log4j.Logger;

import org.openamq.framing.*;
import org.openamq.transport.*;
import org.openamq.protocol.*;

public class AMQClientConnection {
    private static final Logger
        _logger = Logger.getLogger(AMQClientConnection.class);

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
        instance;
    int
        trace,
        timeout;
    TreeMap
        sessions;

    public static AuthData authPlain(String user, String password) {
        return new AuthDataPlain(user, password);
    }

    public AMQClientConnection() {}

    public AMQClientConnection(String host, String virtualHost, AuthData authData, String instance,
        int trace, int timeout) throws IOException {

        setHost(host);
        setVirtualHost(virtualHost);
        setAuthData(authData);
        setClientInstance(instance);
        setTrace(trace);
        setTimeout(timeout);

        connect();
    }

    public void connect() throws IOException {
        aph = new AMQProtocolHandler(this);
        asc = new AMQSocketConnection();
        conState = new AMQConnectionState(this);
        sessions = new TreeMap();

        asc.connect(aph, new InetSocketAddress(host, AMQConstant.DEFAULT_PORT));
        conState.waitConnectionOpened();
    }

    public void setHost(String host) {
        this.host = host;
    }

    public String getHost() {
        return host;
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

    public void setTrace(int trace) {
        this.trace = trace;
    }

    public int getTrace() {
        return trace;
    }

    public void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    public int getTimeout() {
        return timeout;
    }

    public AMQClientSession createSession() {
        AMQClientSession
            acs = new AMQClientSession(this);

        sessions.put(acs.getSessionId(), acs);
        acs.start();
        acs.getSessionState().waitChannelOpened();

        return acs;
    }

    public void start() {
        conThread = new Thread(conState);
        conThread.start();
    }

    public void close() throws Exception {
        close(AMQConstant.REPLY_SUCCESS, "Client closing connection.", 0, 0);
    }

    public void close(int replyCode, String replyText, int classId, int methodId) throws Exception {
        if (replyCode != AMQConstant.REPLY_SUCCESS)
            _logger.error(replyText);
        writeFrame(ConnectionCloseBody.createAMQFrame(0, replyCode, replyText, classId, methodId));
    }

    public void dispatchFrame(AMQFrame frame) {
        AMQClientSession
            acs = (AMQClientSession)sessions.get(frame.channel);

        if (acs != null) {
            AMQChannelState
                ass = acs.getSessionState();

            ass.setExternalEvent(frame);
        } else {
            _logger.warn("Not handling channel id: " + frame.channel);
        }
    }

    public void writeFrame(AMQDataBlock frame) throws Exception
    {
        aph.writeFrame(null, frame);
    }

    public AMQConnectionState getConnectionState() {
       return conState; 
    }

    public AMQProtocolHandler getProtocolHandler() {
        return aph;
    }

    public void setConnectionTuneData(ConnectionTuneBody ctb) {
        this.ctb = ctb;
        aph.setupHeartbeats(ctb.heartbeat);
    }

    public ConnectionTuneBody getConnectionTuneData() {
        return ctb;
    }
} 
