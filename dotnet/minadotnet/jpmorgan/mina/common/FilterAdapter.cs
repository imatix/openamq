using System;
using jpmorgan.mina.common;

namespace minadotnet.jpmorgan.mina.common
{
    public class FilterAdapter : IFilter
    {        
        public virtual void Init()
        {     
        }

        public virtual void Destroy()
        {         
        }

        public virtual void OnPreAdd(IFilterChain parent, string name, IFilter nextFilter)
        {         
        }

        public virtual void OnPostAdd(IFilterChain parent, string name, IFilter nextFilter)
        {         
        }

        public virtual void OnPreRemove(IFilterChain parent, string name, IFilter nextFilter)
        {         
        }

        public virtual void OnPostRemove(IFilterChain parent, string name, IFilter nextFilter)
        {         
        }

        public virtual void SessionCreated(ISession session, IFilterChain filterChain)
        {         
        }

        public virtual void SessionOpened(ISession session, IFilterChain filterChain)
        {
            filterChain.NextSessionOpened();
        }

        public virtual void SessionClosed(ISession session, IFilterChain filterChain)
        {         
            filterChain.NextSessionClosed();
        }

        public virtual void SessionIdle(ISession session, IdleStatus status, IFilterChain filterChain)
        {         
            // Possibly remove this method?
        }

        public virtual void ExceptionCaught(ISession session, Exception cause, IFilterChain filterChain)
        {         
            filterChain.NextExceptionCaught(cause);
        }

        public virtual void MessageReceived(ISession session, object message, IFilterChain filterChain)
        {         
            filterChain.NextMessageReceived(message);
        }

        public virtual void MessageSent(ISession session, object message, IFilterChain filterChain)
        {         
            filterChain.NextMessageSent(message);
        }

        public virtual void FilterClose(ISession session, CloseFuture closeFuture, IFilterChain filterChain)
        {         
            filterChain.NextFilterClose(closeFuture);
        }

        public virtual void FilterWrite(ISession session, WriteRequest writeRequest, IFilterChain filterChain)
        {         
            filterChain.NextFilterWrite(writeRequest);
        }
    }
}
