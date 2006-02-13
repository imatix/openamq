using jpmorgan.mina.common;

namespace jpmorgan.mina.handler.demux
{
    public interface IMessageHandler
    {
        void MessageReceived(ISession session, object message);
    }
}
