using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.XMS.Client.State
{
    public class IllegalStateTransitionException : AMQException
    {
        private AMQState _originalState;

        private Type _frame;

        public IllegalStateTransitionException(AMQState originalState, Type frame)
            : base("No valid state transition defined for receiving frame " + frame +
                   " from state " + originalState)
        {            
            _originalState = originalState;
            _frame = frame;
        }

        public AMQState OriginalState            
        {
            get
            {
                return _originalState;
            }
        }

        public Type FrameType
        {
            get
            {
                return _frame;
            }
        }
    }
}
