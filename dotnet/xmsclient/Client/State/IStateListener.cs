using System;
using System.Collections.Generic;
using System.Text;

namespace OpenAMQ.XMS.Client.State
{
    public interface IStateListener
    {
        void StateChanged(AMQState oldState, AMQState newState);

        void Error(Exception e);
    }
}
