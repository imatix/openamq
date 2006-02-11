using System;
using System.Collections;
using System.Text;

namespace OpenAMQ.XMS.Client.Protocol
{
    public interface IAMQMethodListener
    {
        /// <summary>
        /// Invoked when a method frame has been received
        /// <param name="evt">the event</param>
        /// <returns>true if the handler has processed the method frame, false otherwise. Note
        /// that this does not prohibit the method event being delivered to subsequent listeners
        /// but can be used to determine if nobody has dealt with an incoming method frame.</param>
        /// <exception cname="AMQException">if an error has occurred. This exception will be delivered
        /// to all registered listeners using the error() method (see below) allowing them to
        /// perform cleanup if necessary.</exception>     
        bool MethodReceived(AMQMethodEvent evt);

        /// <summary>
        /// Callback when an error has occurred. Allows listeners to clean up.
        /// </summary>
        /// <param name="e">the exception</param>         
        void Error(Exception e);
    }
}
