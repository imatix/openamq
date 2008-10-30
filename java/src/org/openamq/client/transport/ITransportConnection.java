package org.openamq.client.transport;

import org.openamq.client.protocol.AMQProtocolHandler;
import org.openamq.client.AMQConnection;

import java.io.IOException;

public interface ITransportConnection
{
    void connect(AMQProtocolHandler protocolHandler, AMQConnection.BrokerDetail brokerDetail)
            throws IOException;
}
