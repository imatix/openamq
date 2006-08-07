/*---------------------------------------------------------------------------
    AMQClientConnection.java

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

import java.io.IOException;

import org.openamq.protocol.AMQClientConnectionI;

/**
 * An AMQConnection object opens a persistent connection to a specific virtual
 * host in an AMQP broker.
 *
 * An AMQConnection object can handle multiple sessions. Sessions handle message flow
 * between the application and the AMQP broker.
 *
 * The connection object is responsible for connection configuration and
 * authentication. After the connection parameters are set, the connect method
 * can be called. After the connect method returns, the connection with the
 * AMQP broker has been stablished, and sessions can be requested with the
 * createSession function.
 */
public class AMQClientConnection extends AMQClientConnectionI {

    /**
     * Given a user and password, get a plain-type authentication object.
     *
     * @param user account username. 
     * @param password password for the account.
     * @return authentication object to be passed as parameter for the connection.
     */
    public static AuthData authPlain(String user, String password) {
        return AMQClientConnectionI.authPlain(user, password);
    }

    /**
     * Constructs a new AMQClientConnection object to be configured at
     * least with authentication data before attempting connection.
     *
     * Default values will be used for most connection:
     *  host = "localhost:5672"
     *  virtualHost = "/"
     * Timeout will be configured from broker parameters.
     * The instance name should be configured.
     */
    public AMQClientConnection() {
        super();
    }

    /**
     * Constructs a new AMQClientConnection with the given connection parameters.
     *
     * @param host host and port to connect to (e.g. "localhost", "192.168.50.15:5555", etc.).
     * @param virtualHost virtual host to connect to (e.g. "/", "/stock/europe", etc.).
     * @param authData authentication data to be obtained with one of the static authXXX functions.
     * @param instance client name for the connection to the broker.
     * @param timeout disconnection after this amount of seconds if the broker (or the client) is not responding.
     */
    public AMQClientConnection(String host, String virtualHost, AuthData authData, String instance,
        int timeout) throws IOException {

        super(host, virtualHost, authData, instance, timeout);
    }

    /**
     * Set host.
     *
     * @param host AMQP host and port to connect to (e.g. "localhost", "192.168.50.15:5555", etc.).
     */
    public void setHost(String host) {
        super.setHost(host);
    }

    /**
     * Get host.
     *
     * @return AMQP host and port.
     */
    public String getHost() {
        return super.getHost();
    }

    /**
     * Set virtual host.
     *
     * @param virtualHost virtual host to connect to (e.g. "/", "/stock/europe", etc.).
     */
    public void setVirtualHost(String vHost) {
        super.setVirtualHost(vHost);
    }

    /**
     * Get virtual host.
     *
     * @return virtual host.
     */
    public String getVirtualHost() {
        return super.getVirtualHost();
    }

    /**
     * Set authentication data.
     *
     * @param authData authentication data to be obtained with one of the static authXXX functions.
     */
    public void setAuthData(AuthData auth) {
        super.setAuthData(auth);
    }

    /**
     * Get authentication data.
     *
     * @return authentication data.
     */
    public AuthData getAuthData() {
        return super.getAuthData();
    }

    /**
     * Set client instance name.
     *
     * @param instance client name for the connection to the broker.
     */
    public void setClientInstance(String instance) {
        super.setClientInstance(instance);
    }

    /**
     * Get client instance name.
     *
     * @return client instance name.
     */
    public String getClientInstance() {
        return super.getClientInstance();
    }

    /**
     * Set disconnection timeout.
     *
     * @param timeout disconnection after this amount of seconds if the broker (or the client) is not responding.
     */
    public void setTimeout(int timeout) {
        super.setTimeout(timeout);
    }

    /**
     * Get disconnection timeout.
     *
     * @return disconnection timeout.
     */
    public int getTimeout() {
        return super.getTimeout();
    }

    /**
     * Connects to the broker with the currently configured parameters.
     *
     * After the this method returns, the connection with the AMQP broker has
     * been stablished, and sessions can be requested with the createSession
     * function.
     * @exception Exception if there is a problem connecting to the broker.
     */
    public void connect() throws Exception {
        super.connect();
    }

    /**
     * Create a new session for this connection.
     *
     * @return a new session for this connection.
     * @exception Exception if it is not possible to create a new session.
     */
    public AMQClientSession createSession() throws Exception {
        return (AMQClientSession)super.createSession(new AMQClientSession());
    }

    /**
     * Close this connection.
     *
     * After a connection is closed with this method (or by the server), further
     * attempts to create sessions will result in exceptions thrown.
     * @exception Exception if an error occured while closing the connection.
     */
    public void close() throws Exception {
        super.close(AMQConstant.REPLY_SUCCESS, "Client closing connection.", 0, 0);
    }
}
