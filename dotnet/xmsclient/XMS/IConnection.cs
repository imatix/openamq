namespace OpenAMQ.XMS
{
    public interface IConnection : IBM.XMS.IConnection
    {        
        /// <return>the maximum number of sessions supported by this Connection</return>     
        ushort MaximumChannelCount
        {
            get;
        }
        
        /// <summary>
        /// Get the connection listener that has been registered with this connection, if any
        /// </summary>
        /// <return>the listener or null if none has been set</return>         
        IConnectionListener ConnectionListener
        {
            get;
            set;
        }
    }
}
