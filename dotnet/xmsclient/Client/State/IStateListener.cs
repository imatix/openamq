using System;
using System.Text;

namespace OpenAMQ.XMS.Client.State
{
    public interface IStateListener
    {
        void StateChanged(AMQState oldState, AMQState newState);

        void Error(Exception e);
    }
}
