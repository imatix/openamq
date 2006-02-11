using System;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Protocol;

namespace OpenAMQ.XMS.Client.State.Listener
{
    public class SpecificMethodFrameListener : BlockingMethodFrameListener
    {
        private readonly Type _expectedClass;

        public SpecificMethodFrameListener(ushort channelId, Type expectedClass) : base(channelId)
        {        
            _expectedClass = expectedClass;
        }

        public bool processMethod(int channelId, AMQMethodBody frame)
        {
            return _expectedClass.IsInstanceOfType(frame);
        }
    }
}
