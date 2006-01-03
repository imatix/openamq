using System;
using System.Collections;
using System.Text;

namespace jpmorgan.mina.common
{
    ///
    /// A container of {@link IoFilter}s that forwards {@link IoHandler} events
    /// to the consisting filters and terminal {@link IoHandler} sequentially.
    /// Every {@link IoSession} has its own {@link IoFilterChain} (1-to-1 relationship).
    public interface IFilterChain
    {
        ///
        /// Returns the parent {@link IoSession} of this chain.
        /// @return {@link IoSession}     
        ISession Session
        {
            get;
        }           
        
        ///
        /// Returns the {@link IoFilter} with the specified <tt>name</tt> in this chain.
        /// @return <tt>null</tt> if there's no such name in this chain        
        IFilter Get(string name);
        
        ///
        /// Returns the {@link NextFilter} of the {@link IoFilter} with the
        /// specified <tt>name</tt> in this chain.
        /// @return <tt>null</tt> if there's no such name in this chain         
        IFilter GetNextFilter(string name);
        
        ///
        /// Returns the list of all {@link Entry}s this chain contains.         
        ICollection GetAll();
        
        ///
        /// Returns the reversed list of all {@link Entry}s this chain contains.         
        IList GetAllReversed();
        
        ///
        /// Returns <tt>true</tt> if this chain contains an {@link IoFilter} with the
        /// specified <tt>name</tt>.         
        bool Contains(string name);
        
        ///
        /// Returns <tt>true</tt> if this chain contains the specified <tt>filter</tt>.        
        bool Contains(IFilter filter);
        
        ///
        /// Returns <tt>true</tt> if this chain contains an {@link IoFilter} of the
        /// specified <tt>filterType</tt>.         
        bool Contains(Type filterType);
        
        ///
        /// Adds the specified filter with the specified name at the beginning of this chain.
        /// @throws IoFilterLifeCycleException
        ///             if {@link IoFilter#onPostAdd(IoFilterChain, String, NextFilter)} or
        ///             {@link IoFilter#init()} throws an exception.         
        void AddFirst(string name, IFilter filter);

        ///
        /// Adds the specified filter with the specified name at the end of this chain.
        /// @throws IoFilterLifeCycleException
        ///             if {@link IoFilter#onPostAdd(IoFilterChain, String, NextFilter)} or
        ///             {@link IoFilter#init()} throws an exception.
        void AddLast(string name, IFilter filter);

        ///
        /// Adds the specified filter with the specified name just before the filter whose name is
        /// <code>baseName</code> in this chain.
        /// @throws IoFilterLifeCycleException
        ///             if {@link IoFilter#onPostAdd(IoFilterChain, String, NextFilter)} or
        ///             {@link IoFilter#init()} throws an exception.         
        void AddBefore(string baseName, string name, IFilter filter);

        ///
        /// Adds the specified filter with the specified name just after the filter whose name is
        /// <code>baseName</code> in this chain.
        /// @throws IoFilterLifeCycleException
        ///            if {@link IoFilter#onPostAdd(IoFilterChain, String, NextFilter)} or
        ///             {@link IoFilter#init()} throws an exception.         
        void AddAfter(string baseName, string name, IFilter filter);

        ///
        /// Removes the filter with the specified name from this chain.
        /// @throws IoFilterLifeCycleException
        ///             if {@link IoFilter#onPostRemove(IoFilterChain, String, NextFilter)} or
        ///             {@link IoFilter#destroy()} throws an exception.         
        IFilter Remove(string name);

        ///
        /// Removes all filters added to this chain.
        ///@throws Exception if {@link IoFilter#onPostRemove(IoFilterChain, String, NextFilter)} thrown an exception.
        ///
        void Clear();

        ///
        /// Filters {@link IoHandler#messageReceived(IoSession,Object)}
        /// event.        
        void MessageReceived(ISession session, object message);

        ///
        /// Filters {@link IoHandler#exceptionCaught(IoSession,Throwable)}
        /// event.
        ///
        void ExceptionCaught(ISession session, Exception cause);        

        ///
        /// Filters {@link IoHandler#messageSent(IoSession,Object)}
        /// event.         
        void MessageSent(ISession session, object message);
    }
}
