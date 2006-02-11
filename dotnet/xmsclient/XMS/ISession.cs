using System;
using System.Collections.Generic;
using System.Text;
using IBM.XMS;

namespace OpenAMQ.XMS
{
    public interface ISession
    {
            /**
         * Indicates that no client acknowledgements are required. Broker assumes that once it has delivered
         * a message packet successfully it is acknowledged.
         */
        //const int NO_ACKNOWLEDGE = 257;

        /**
         * Pre acknowledge means that an ack is sent per message but sent before user code has processed
         * the message (i.e. before the onMessage() call or the receive() method has returned).
         */
        //const int PRE_ACKNOWLEDGE = 258;

        IMessageConsumer createConsumer(IDestination destination,
                                        int prefetch,
                                        bool noLocal,
                                        bool exclusive,
                                        string selector);

        
         /// <returns>the prefetch value used by default for consumers created on this session.</returns> 
        int getDefaultPrefetch();
        
        /// <param>defaultPrefetch the prefetch value used by default for consumers created on this session.</param>         
        void setDefaultPrefetch(int defaultPrefetch);

        /// <summary>
        /// Create a producer
        /// </summary>
        /// <param>destination</param>
        /// <param>mandatory the value of the mandatory flag used by default on the producer</param>
        /// <param>immediate the value of the immediate flag used by default on the producer</param>         
        /// <exception>JMSException</exception>         
        IMessageProducer createProducer(IDestination destination, bool mandatory, bool immediate);

        /// <summary>
        /// Create a producer
        /// </summary>
        /// <param>destination</param>
        /// <param>immediate the value of the immediate flag used by default on the producer</param>
        /// <return>the producer
        /// <exception>JMSException</exception>         
        IMessageProducer createProducer(IDestination destination, bool immediate);
    }
}
