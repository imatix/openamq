using System;
using System.Net.Sockets;
using jpmorgan.mina.common;

namespace jpmorgan.mina.transport.socket.networkstream
{
    public interface SocketSession : ISession
    {
        /// <summary>
        /// The underlying socket used for communication with the endpoint
        /// </summary>
        Socket Socket
        {
            get;
        }

        /// <summary>
        /// The size of the buffer used to store data read from the socket
        /// </summary>
        int ReceiveBufferSize
        {
            get;
            set;
        }

        /// <summary>
        /// The buffer used to store the data read from the socket
        /// </summary>
        ByteBuffer Buffer
        {
            get;
            set;
        }
    }
}
