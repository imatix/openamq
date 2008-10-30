package org.openamq.client.transport;

/**
 * The TransportConnection is a helper class responsible for connecting to an AMQ server. It sets up
 * the underlying connector, which currently always uses TCP/IP sockets. It creates the
 * "protocol handler" which deals with MINA protocol events.
 *
 * Could be extended in future to support different transport types by turning this into concrete class/interface
 * combo.
 *
 */
public class TransportConnection
{
    private static ITransportConnection _instance;

    static
    {
        if(Boolean.getBoolean("amqj.useBlockingIo"))
        {
            _instance = new BlockingSocketTransportConnection();
        }
        else
        {
            _instance = new SocketTransportConnection();
        }
    }

    public static void setInstance(ITransportConnection transport)
    {
        _instance = transport;
    }

    public static ITransportConnection getInstance()
    {
        return _instance;
    }
}
