using System;
using System.Text;

namespace OpenAMQ.XMS.Client.State
{
    public interface IAMQStateListener
    {
        void StateChanged(AMQStateChangedEvent evt);               
    }
}
