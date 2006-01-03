using System;
using System.Collections.Generic;
using System.Text;

namespace jpmorgan.mina.transport.socket.networkstream
{
    interface ISocketSessionManager
    {
        //
        // The number of worker threads assigned to perform socket I/O.
        //
        int Processors
        {
            get;
            set;
        }
    }
}
