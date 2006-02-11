using System;
using jpmorgan.mina.common;
using log4net;

using IBM.XMS;

namespace OpenAMQ.XMS.Client.Message
{
    /// <summary>
    /// Raised when a message body is received unexpectedly by the client. This typically occurs when the
    /// length of bodies received does not match with the declared length in the content header.
    /// </summary>
    public class UnexpectedBodyReceivedException : AMQException
    {
        public UnexpectedBodyReceivedException(ILog logger, string msg, Exception t)
            : base(logger, msg, t)
        {        
        }

        public UnexpectedBodyReceivedException(ILog logger, string msg)
            : base(logger, msg)
        {            
        }

        public UnexpectedBodyReceivedException(ILog logger, int errorCode, string msg)
            : base(logger, errorCode, msg)
        {                        
        }
    }
}
