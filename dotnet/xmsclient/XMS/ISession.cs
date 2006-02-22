using System;
using System.Text;
using IBM.XMS;

namespace OpenAMQ.XMS
{
    public interface ISession : IBM.XMS.ISession
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

        IMessageConsumer CreateConsumer(IDestination destination,
                                        int prefetch,
                                        bool noLocal,
                                        bool exclusive,
                                        string selector);
               
        
        /// <param>defaultPrefetch the prefetch value used by default for consumers created on this session.</param>         
        int DefaultPrefetch
        {
            get;
            set;
        }
        
        /// <summary>
        /// Create a producer
        /// </summary>
        /// <param>destination</param>
        /// <param>mandatory the value of the mandatory flag used by default on the producer</param>
        /// <param>immediate the value of the immediate flag used by default on the producer</param>         
        /// <exception>JMSException</exception>         
        IMessageProducer CreateProducer(IDestination destination, bool mandatory, bool immediate);

        /// <summary>
        /// Create a producer
        /// </summary>
        /// <param>destination</param>
        /// <param>immediate the value of the immediate flag used by default on the producer</param>
        /// <return>the producer
        /// <exception>JMSException</exception>         
        IMessageProducer CreateProducer(IDestination destination, bool immediate);
    }
}
