using System;
using System.Text;

namespace jpmorgan.mina.common
{
    /// An interface that builds {@link IoFilterChain} in predefined way
    /// when {@link IoSession} is created.  You can extract common filter chain
    /// modification logic to this interface.  For example, to add a filter
    /// to the chain,
    /// <pre>
    /// public class MyFilterChainBuilder implements IoFilterChainBuilder {
    ///     public void buildFilterChain( IoFilterChain chain ) throws Exception {
    ///         chain.addLast( "myFilter", new MyFilter() );
    ///     }
    /// }
    /// </pre>
    public interface IFilterChainBuilder
    {                   
        ///
        /// Modifies the specified <tt>chain</tt>.         
        void BuildFilterChain(IFilterChain chain);
    }
}
