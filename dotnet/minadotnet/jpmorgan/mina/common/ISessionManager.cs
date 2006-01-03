using System;
using System.Collections.Generic;
using System.Text;

namespace jpmorgan.mina.common
{
    ///
    /// Base interface for all acceptors and connectors that manage
    /// sessions.
    public interface ISessionManager
    {
        ///
        /// Returns the {@link IoFilterChainBuilder} which will modify the
        /// {@link IoFilterChain} of all {@link IoSession}s which is managed
        /// by this manager.
        /// The default value is an empty {@link DefaultIoFilterChainBuilder}.         
        IFilterChainBuilder FilterChainBuilder
        {
            get;
            set;
        }
        
        ///
        /// A shortcut for <tt>( ( DefaultIoFilterChainBuilder ) </tt>{@link #getFilterChainBuilder()}<tt> )</tt>.
        /// Please note that the returned object is not a <b>real</b> {@link IoFilterChain}
        /// but a {@link DefaultIoFilterChainBuilder}.  Modifying the returned builder
        /// won't affect the existing {@link IoSession}s at all, because
        /// {@link IoFilterChainBuilder}s affect only newly created {@link IoSession}s.
        ///
        /// @throws IllegalStateException if the current {@link IoFilterChainBuilder} is
        /// not a {@link DefaultIoFilterChainBuilder}         
        DefaultIoFilterChainBuilder FilterChain
        {
            // RG note - what is this property really for?
            get;
        }

    }
}
