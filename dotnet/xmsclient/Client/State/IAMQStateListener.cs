using System;
using System.Collections.Generic;
using System.Text;

namespace OpenAMQ.XMS.Client.State
{
    public interface IAMQStateListener
    {
        void StateChanged(AMQStateChangedEvent evt);               
    }
}
