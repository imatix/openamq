using System;

namespace jpmorgan.mina.common
{
    public class NOOPIoFilterChainBuilder : IFilterChainBuilder
    {
        public void BuildFilterChain(IFilterChain chain)
        {
        }

        public override string ToString()
        {
            return "NOOP";
        }
    }
}
