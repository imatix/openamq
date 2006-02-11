using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.XMS.Client
{
    public class AMQQueue : AMQDestination
    {
        /// <summary>
        /// Create a reference to a non temporary queue. Note this does not actually imply the queue exists.        
        /// </summary>
        /// <param name="name">the name of the queue</param>
        public AMQQueue(string name) : this(name, false)
        {            
        }

        /// <summary>
        /// Create a queue with a specified name.
        /// </summary>
        /// <param name="name">the destination name (used in the routing key)</param>
        /// <param name="temporary">if true the broker will generate a queue name, also if true then the queue is autodeleted
        /// and exclusive</param>         
        public AMQQueue(string name, bool temporary)
            : this(name, temporary ? null : name, temporary, temporary)
        {
            // queue name is set to null indicating that the broker assigns a name in the case of temporary queues
            // temporary queues are typically used as response queues            
        }

        /// <summary>
        /// Create a reference to a queue. Note this does not actually imply the queue exists.
        /// <param name="destinationName">the queue name</param>
        /// <param name="queueName">the queue name</param>
        /// <param name="exclusive">true if the queue should only permit a single consumer</param>
        /// <param name="autoDelete">true if the queue should be deleted automatically when the last consumers detaches</param>        
        public AMQQueue(string destinationName, string queueName, bool exclusive, bool autoDelete)
               :base(AMQDestination.QUEUE_EXCHANGE_NAME, AMQDestination.QUEUE_EXCHANGE_CLASS, destinationName, exclusive,
                     autoDelete, queueName)
        {        
        }

        public string EncodedName
        {
            get
            {
                return 'Q' + QueueName;
            }
        }

        public string RoutingKey
        {
            get
            {
                return QueueName;
            }
        }    

    }
}
