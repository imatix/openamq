using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.XMS.Client.State
{
    public class AMQStateChangedEvent
    {
        private readonly AMQState _oldState;

        private readonly AMQState _newState;

        public AMQStateChangedEvent(AMQState oldState, AMQState newState)
        {
            _oldState = oldState;
            _newState = newState;
        }

        public AMQState OldState
        {
            get
            {
                return _oldState;
            }
        }

        public AMQState NewState
        {
            get
            {
                return _newState;
            }
        }

    }
}
