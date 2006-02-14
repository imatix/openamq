using System;
using System.Text;

namespace jpmorgan.mina.common
{
    /// A filter which intercepts {@link IoHandler} events like Servlet
    /// filters.  Filters can be used for these purposes:
    /// <ul>
    ///   <li>Event logging,</li>
    ///   <li>Performance measurement,</li>
    ///   <li>Authorization,</li>
    ///   <li>Overload control,</li>
    ///   <li>Message transformation (e.g. encryption and decryption, ...),</li>
    ///   <li>and many more.</li>
    /// </ul>
    /// <p>
    /// <strong>Please NEVER implement your filters to wrap
    /// {@link IoSession}s.</strong> Users can cache the reference to the
    /// session, which might malfunction if any filters are added or removed later.
    /// 
    /// <h3>The Life Cycle</h3>
    /// {@link IoFilter}s are activated only when they are inside {@link IoFilterChain}.
    /// <p>
    /// When you add an {@link IoFilter} to an {@link IoFilterChain}:
    /// <ol>
    ///  <li>{@link #init()} is invoked by {@link IoFilterChain} if the filter is
    ///       added at the first time.</li>
    ///   <li>{@link #onPreAdd(IoFilterChain, String, NextFilter)} is invoked to notify
    ///       that the filter will be added to the chain.</li>
    ///   <li>The filter is added to the chain, and all events and I/O requests
    ///       pass through the filter from now.</li>
    ///   <li>{@link #onPostAdd(IoFilterChain, String, NextFilter)} is invoked to notify
    ///       that the filter is added to the chain.</li>
    ///   <li>The filter is removed from the chain if {@link #onPostAdd(IoFilterChain, String,
    /// org.apache.mina.common.IoFilter.NextFilter)}
    ///       threw an exception.  {@link #destroy()} is also invoked if the filter
    ///       is the last filter which was added to {@link IoFilterChain}s.</li>
    /// </ol>
    /// <p>
    /// When you remove an {@link IoFilter} from an {@link IoFilterChain}:
    /// <ol>
    ///   <li>{@link #onPreRemove(IoFilterChain, String, NextFilter)} is invoked to
    ///       notify that the filter will be removed from the chain.</li>
    ///   <li>The filter is removed from the chain, and any events and I/O requests
    ///       don't pass through the filter from now.</li>
    ///   <li>{@link #onPostRemove(IoFilterChain, String, NextFilter)} is invoked to
    ///       notify that the filter is removed from the chain.</li>
    ///   <li>{@link #destroy()} is invoked if the removed filter was the last one.</li>
    /// </ol>           
    /// 
    /// @see IoFilterAdapter
    public interface IFilter
    {
        ///
        /// Invoked when this filter is added to a {@link IoFilterChain}
        /// at the first time, so you can initialize shared resources.
        ///
        void Init();

        ///
        /// Invoked when this filter is not used by any {@link IoFilterChain}
        /// anymore, so you can destroy shared resources.
        ///
        void Destroy();
        
        ///
        /// Invoked before this filter is added to the specified <tt>parent</tt>.
        /// Please note that this method can be invoked more than once if
        /// this filter is added to more than one parents.  This method is not
        /// invoked before {@link #init()} is invoked. 
        ///
        /// @param parent the parent who called this method
        /// @param name the name assigned to this filter
        /// @param nextFilter the {@link NextFilter} for this filter.  You can reuse
        ///                   this object until this filter is removed from the chain.        
        void OnPreAdd(IFilterChain parent, string name, IFilter nextFilter);
        
        ///
        /// Invoked after this filter is added to the specified <tt>parent</tt>.
        /// Please note that this method can be invoked more than once if
        /// this filter is added to more than one parents.  This method is not
        /// invoked before {@link #init()} is invoked. 
        ///
        /// @param parent the parent who called this method
        /// @param name the name assigned to this filter
        /// @param nextFilter the {@link NextFilter} for this filter.  You can reuse
        ///                   this object until this filter is removed from the chain.
        void OnPostAdd(IFilterChain parent, string name, IFilter nextFilter);
        
        ///
        /// Invoked before this filter is removed from the specified <tt>parent</tt>.
        /// Please note that this method can be invoked more than once if
        /// this filter is removed from more than one parents.
        /// This method is always invoked before {@link #destroy()} is invoked.
        ///  
        /// @param parent the parent who called this method
        /// @param name the name assigned to this filter
        /// @param nextFilter the {@link NextFilter} for this filter.  You can reuse
        ///                   this object until this filter is removed from the chain.
        ///
        void OnPreRemove(IFilterChain parent, string name, IFilter nextFilter);
        
        ///
        /// Invoked after this filter is removed from the specified <tt>parent</tt>.
        /// Please note that this method can be invoked more than once if
        /// this filter is removed from more than one parents.
        /// This method is always invoked before {@link #destroy()} is invoked.
        ///  
        /// @param parent the parent who called this method
        /// @param name the name assigned to this filter
        /// @param nextFilter the {@link NextFilter} for this filter.  You can reuse
        ///                   this object until this filter is removed from the chain.
        void OnPostRemove(IFilterChain parent, string name, IFilter nextFilter);
        
        ///
        /// Filters {@link IoHandler#sessionCreated(IoSession)} event.        
        void SessionCreated(ISession session, IFilterChain filterChain);
        
        ///
        /// Filters {@link IoHandler#sessionOpened(IoSession)} event.
        ///
        void SessionOpened(ISession session, IFilterChain filterChain);

        ///
        /// Filters {@link IoHandler#sessionClosed(IoSession)} event.
        ///
        void SessionClosed(ISession session, IFilterChain filterChain);

        ///
        /// Filters {@link IoHandler#sessionIdle(IoSession,IdleStatus)}
        /// event.         
        void SessionIdle(ISession session, IdleStatus status, IFilterChain filterChain);

        ///
        /// Filters {@link IoHandler#exceptionCaught(IoSession,Throwable)}
        /// event.
        ///
        void ExceptionCaught(ISession session, Exception cause, IFilterChain filterChain);

        ///
        /// Filters {@link IoHandler#messageReceived(IoSession,Object)}
        /// event.        
        void MessageReceived(ISession session, object message, IFilterChain filterChain);

        ///
        /// Filters {@link IoHandler#messageSent(IoSession,Object)}
        /// event.         
        void MessageSent(ISession session, object message, IFilterChain filterChain);

        ///
        /// Filters {@link IoSession#close()} method invocation.         
        /// ????? RG
        void FilterClose(ISession session, CloseFuture closeFuture, IFilterChain filterChain);
        
        ///
        /// Filters {@link IoSession#write(Object)} method invocation.               
        void FilterWrite(ISession session, WriteRequest writeRequest, IFilterChain filterChain);
    }
}
