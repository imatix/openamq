using System;
using System.Collections;
using jpmorgan.mina.common;
using log4net;

namespace jpmorgan.mina.common.support
{
    /// <summary>
    /// An abstract implementation of IFilterChain that provides common operations for developers to
    /// implement their own transport layer.
    /// <p/>
    /// The only method that a developer should implement is DoWrite(ISession, WriteRequest). This method is
    /// invoked when a filter chain is evaluated for IFilter.FilterWrite and finally to be written out.
    /// </summary>
    public abstract class AbstractFilterChain : IFilterChain
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(AbstractFilterChain));
        
        protected readonly ISession _session;
                
        private ArrayList _filters = new ArrayList(10);

        /// <summary>
        /// Our position in the list
        /// </summary>
        private int _pos = -1;

        private Stack _context = new Stack(5);
        
        protected AbstractFilterChain(ISession session)
        {
            if (session == null)
            {
                throw new ArgumentNullException("session");
            }
            _session = session;
        }

        public ISession Session
        {
            get { return _session; }
        }


        private void PushContext()
        {            
            _context.Push(_pos);
            _pos = 0;
        }

        private void PopContext()
        {                        
            _pos = (int) _context.Pop();            
        }

        public void AddFirst(string name, IFilter filter)
        {
            _filters.Insert(0, filter);
        }

        public void AddLast(string name, IFilter filter)
        {
            _filters.Add(filter);
        }


        public bool Contains(Type filterType)
        {
            foreach (object o in _filters)
            {
                if (filterType.IsAssignableFrom(o.GetType()))
                {
                    return true;
                }
            }
            return false;
        }

        public void SessionCreated()
        {
            PushContext();
            try
            {
                NextSessionCreated();
            }
            finally
            {
                PopContext();
            }            
        }
        
        public void NextSessionCreated()
        {
            if (_pos == _filters.Count)
            {
                _session.Handler.SessionCreated(_session);                
            }
            else
            {
                try
                {
                    ((IFilter) _filters[_pos++]).SessionCreated(_session, this);
                    _pos--;
                }
                catch (Exception e)
                {
                    _pos = 0;
                    ExceptionCaught(e);
                }
            }
        }

        public void SessionOpened()
        {
            PushContext();
            try
            {
                NextSessionOpened();
            }
            finally
            {
                PopContext();
            }            
        }
        
        public void NextSessionOpened()
        {
            if (_pos == _filters.Count)
            {
                _session.Handler.SessionOpened(_session);                
            }
            else
            {
                try
                {
                    ((IFilter) _filters[_pos++]).SessionOpened(_session, this);
                    _pos--;
                }
                catch (Exception e)
                {
                    _pos = 0;
                    ExceptionCaught(e);
                }
            }
        }

        public void SessionClosed()
        {
            PushContext();
            try
            {
                NextSessionClosed();
            }
            finally
            {
                PopContext();
            }            
        }
        
        public void NextSessionClosed()
        {
            if (_pos == _filters.Count)
            {
                _session.Handler.SessionClosed(_session);                
            }
            else
            {
                try
                {
                    ((IFilter) _filters[_pos++]).SessionClosed(_session, this);
                    _pos--;
                }
                catch (Exception e)
                {
                    _pos = 0;
                    ExceptionCaught(e);
                }
            }
        }

        public void MessageReceived(object message)
        {
            if (_logger.IsDebugEnabled)
            {
                _logger.Debug("Message received called with message of type " + message.GetType());
            }
            PushContext();
            try
            {
                NextMessageReceived(message);
            }
            finally
            {
                PopContext();
            }            
        }
        
        public void NextMessageReceived(object message)
        {            
            if (_pos == _filters.Count)
            {                
                _session.Handler.MessageReceived(_session, message);                
            }
            else
            {
                try
                {                    
                    ((IFilter) _filters[_pos++]).MessageReceived(_session, message, this);                    
                    _pos--;
                }
                catch (Exception e)
                {
                    _pos = 0;
                    ExceptionCaught(e);
                }
            }
        }

        public void ExceptionCaught(Exception cause)
        {
            PushContext();
            try
            {
                NextExceptionCaught(cause);
            }
            finally
            {
                PopContext();
            }            
        }
        
        public void NextExceptionCaught(Exception cause)
        {
            if (_pos == _filters.Count)
            {
                _session.Handler.ExceptionCaught(_session, cause);
                _pos = 0;                
            }
            else
            {
                try
                {
                    ((IFilter) _filters[_pos++]).ExceptionCaught(_session, cause, this);
                    _pos--;
                }
                catch (Exception e)
                {
                    _pos = 0;
                    Console.Error.WriteLine("Exception in ExceptionCaught handler: " + e);
                }
            }
        }

        public void MessageSent(object message)
        {
            PushContext();
            try
            {
                NextMessageSent(message);
            }
            finally
            {
                PopContext();
            }            
        }
        
        public void NextMessageSent(object message)
        {
            if (_pos == _filters.Count)
            {
                _session.Handler.MessageSent(_session, message);                
            }
            else
            {
                try
                {
                    ((IFilter) _filters[_pos++]).MessageReceived(_session, message, this);
                    _pos--;
                }
                catch (Exception e)
                {
                    _pos = 0;
                    ExceptionCaught(e);
                }
            }
        }

        public void FilterWrite(WriteRequest writeRequest)
        {
            PushContext();
            try
            {
                NextFilterWrite(writeRequest);
            }
            finally
            {
                PopContext();
            }
        }
    
        public void NextFilterWrite(WriteRequest writeRequest)
        {
            if (_pos == _filters.Count)
            {                
                DoWrite(writeRequest);                
            }
            else
            {
                try
                {
                    ((IFilter) _filters[_pos++]).FilterWrite(_session, writeRequest, this);
                    _pos--;
                }
                catch (Exception e)
                {
                    _pos = 0;
                    ExceptionCaught(e);
                }
            }
        }

        protected abstract void DoWrite(WriteRequest writeRequest);

        public void FilterClose(CloseFuture closeFuture)
        {
            PushContext();
            try
            {
                NextFilterClose(closeFuture);
            }
            finally
            {
                PopContext();
            }            
        }
        
        public void NextFilterClose(CloseFuture closeFuture)
        {
            if (_pos == _filters.Count)
            {                
                DoClose(closeFuture);
            }
            else
            {
                try
                {
                    ((IFilter) _filters[_pos++]).FilterClose(_session, closeFuture, this);
                    _pos--;
                }
                catch (Exception e)
                {
                    _pos = 0;
                    ExceptionCaught(e);
                }
            }
        }

        protected abstract void DoClose(CloseFuture closeFuture);
    }
}
