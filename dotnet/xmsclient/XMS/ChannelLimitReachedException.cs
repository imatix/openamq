using System;
using IBM.XMS;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.XMS
{
    public class ChannelLimitReachedException : ResourceAllocationException
    {
        private long _limit;

        private const string ERROR_CODE = "1";
        
        public ChannelLimitReachedException(long limit)
            : base("Unable to create session since maximum number of sessions per connection is " +
                   limit + ". Either close one or more sessions or increase the " +
                   "maximum number of sessions per connection (or contact your OpenAMQ administrator.", ERROR_CODE)
        {            
            _limit = limit;
        }

        public long Limit
        {
            get
            {
                return _limit;
            }
        }
    }
}
