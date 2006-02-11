using System;
using System.Text;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;

namespace OpenAMQ.XMS.Client.Protocol
{
    public class AMQMethodEvent
    {
        private AMQMethodBody _method;

        private ushort _channelId;

        private AMQProtocolSession _protocolSession;

        public AMQMethodEvent(ushort channelId, AMQMethodBody method, AMQProtocolSession protocolSession)
        {
            _channelId = channelId;
            _method = method;
            _protocolSession = protocolSession;
        }

        public AMQMethodBody Method
        {
            get
            {
                return _method;
            }
        }

        public ushort ChannelId
        {
            get
            {
                return _channelId;
            }
        }

        public AMQProtocolSession ProtocolSession
        {
            get
            {
                return _protocolSession;
            }            
        }

        public override String ToString()
        {
            StringBuilder buf = new StringBuilder("Method event: ");
            buf.Append("\nChannel id: ").Append(_channelId);
            buf.Append("\nMethod: ").Append(_method);
            return buf.ToString();
        }
    }
}
