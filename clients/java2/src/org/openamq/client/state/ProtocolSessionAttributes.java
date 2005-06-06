package org.openamq.client.state;

/**
 * @author Robert Greig (robert.j.greig@jpmorgan.com)
 */
public interface ProtocolSessionAttributes
{
    static final String CURRENT_STATE_ATTRIBUTE_KEY = "CurrentState";

    static final String SECURITY_MECHANISM = "SecurityMechansim";

    static final String CONNECTION_TUNE_PARAMETERS = "ConnectionTuneParameters";

    static final String CLIENT_NAME = "ClientName";

    static final String VIRTUAL_PATH = "VirtualPath";

    static final String USERNAME = "UserName";

    static final String PASSWORD = "Password";

    static final String AMQ_CONNECTION = "AMQConnection";
}
