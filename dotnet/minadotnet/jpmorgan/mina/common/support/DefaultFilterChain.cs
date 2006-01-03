using System;
using System.Collections;
using System.Collections.Specialized;
using jpmorgan.mina.common;

namespace jpmorgan.mina.common.support
{
    public class DefaultFilterChain : IFilterChain
    {
        /// <summary>
        /// We had to write our own LinkedHashtable since nothing suitable exists
        /// in .NET 1.x.
        /// </summary>
        private LinkedHashtable _filters = new LinkedHashtable();

        #region IFilterChain Members

        public ISession Session
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public IFilter Get(string name)
        {
            return (IFilter) _filters[name];
        }

        public IFilter GetNextFilter(string name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public ICollection GetAll()
        {
            return _filters.Values;
        }

        public System.Collections.IList GetAllReversed()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public bool Contains(string name)
        {
            return _filters.Contains(name);
        }

        public bool Contains(IFilter filter)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public bool Contains(Type filterType)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void AddFirst(string name, IFilter filter)
        {
            _filters[name] = filter;
            _filters.MoveToHead(name);
        }

        public void AddLast(string name, IFilter filter)
        {
            // add to end is default behaviour of our linked hashmap
            _filters[name] = filter;
        }

        public void AddBefore(string baseName, string name, IFilter filter)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void AddAfter(string baseName, string name, IFilter filter)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public IFilter Remove(string name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void Clear()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        ///
        /// Filters {@link IoHandler#messageReceived(IoSession,Object)}
        /// event.        
        public void MessageReceived(ISession session, object message)
        {
            foreach (IFilter filter in _filters)
            {
                filter.MessageReceived(session, message, this);
            }
        }

        ///
        /// Filters {@link IoHandler#exceptionCaught(IoSession,Throwable)}
        /// event.
        ///
        public void ExceptionCaught(ISession session, Exception cause)
        {
            foreach (IFilter filter in _filters)
            {
                filter.ExceptionCaught(session, cause, this);
            }
        }

        ///
        /// Filters {@link IoHandler#messageSent(IoSession,Object)}
        /// event.         
        public void MessageSent(ISession session, object message)
        {
            foreach (IFilter filter in _filters)
            {
                filter.MessageSent(session, message, this);
            }
        }

        #endregion        

    }
}
