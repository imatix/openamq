using System;
using jpmorgan.mina.common;
using log4net;


namespace OpenAMQ.XMS.Client
{
    public class AMQTopic : AMQDestination
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="AMQTopic"/> class.
        /// </summary>
        /// <param name="name">The name.</param>
        public AMQTopic(string name)
            : this(name, false)
        {            
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="AMQTopic"/> class.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <param name="temporary">if set to <c>true</c> [temporary].</param>
        public AMQTopic(string name, bool temporary)
            : base(AMQDestination.TOPIC_EXCHANGE_NAME, AMQDestination.TOPIC_EXCHANGE_CLASS, name, temporary, temporary, null)
        {            
        }
        
        public override string EncodedName
        {
            get
            {
                return 'T' + Name;
            }
        }

        public override string RoutingKey
        {
            get
            {
                return Name;
            }
        }
    }
}
