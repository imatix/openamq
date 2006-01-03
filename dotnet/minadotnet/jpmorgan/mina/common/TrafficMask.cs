using System;
using System.Net.Sockets;

namespace jpmorgan.mina.common
{
    ///
    /// A type-safe mask that is used to control the traffic of {@link IoSession}
    /// with {@link IoSession#setTrafficMask(TrafficMask)}.
    [Flags]
    public enum TrafficMask
    {
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0x2
    }            
}
