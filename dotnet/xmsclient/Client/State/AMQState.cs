using System;
using jpmorgan.mina.common;
using log4net;

namespace OpenAMQ.XMS.Client.State
{
    public enum AMQState
    {
        CONNECTION_NOT_STARTED,    
        CONNECTION_NOT_TUNED,    
        CONNECTION_NOT_OPENED,
        CONNECTION_OPEN,
        CONNECTION_CLOSING,    
        CONNECTION_CLOSED,
        ALL // all is a special state used in the state manager. It is not valid to be "in" the state "all".
    }
}
