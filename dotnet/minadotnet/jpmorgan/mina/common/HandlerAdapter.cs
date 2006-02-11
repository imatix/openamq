using System;
using log4net;

namespace jpmorgan.mina.common
{
    public class HandlerAdapter : IHandler
    {
        #region IHandler Members

        public virtual void SessionCreated(ISession session)
        {            
        }

        public virtual void SessionOpened(ISession session)
        {            
        }

        public virtual void SessionClosed(ISession session)
        {         
        }

        public virtual void SessionIdle(ISession session, IdleStatus status)
        {            
        }

        public virtual void ExceptionCaught(ISession session, Exception cause)
        {            
        }

        public virtual void MessageReceived(ISession session, object message)
        {            
        }

        public virtual void MessageSent(ISession session, object message)
        {            
        }

        #endregion
    }
}
