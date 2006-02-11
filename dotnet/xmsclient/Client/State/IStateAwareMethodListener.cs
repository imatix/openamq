using System;
using System.Text;
using OpenAMQ.XMS.Client.Protocol;

namespace OpenAMQ.XMS.Client.State
{
    public interface IStateAwareMethodListener
    {
        void MethodReceived(AMQStateManager stateManager, AMQMethodEvent evt);
    }
}
