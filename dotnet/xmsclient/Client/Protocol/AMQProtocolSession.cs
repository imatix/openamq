using System;
using System.Collections;
using jpmorgan.mina.common;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Message;

namespace OpenAMQ.XMS.Client.Protocol
{
    public class AMQProtocolSession
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(AMQProtocolSession));

        private const string SECURITY_MECHANISM = "SecurityMechansim";

        private const string CONNECTION_TUNE_PARAMETERS = "ConnectionTuneParameters";

        private const string AMQ_CONNECTION = "AMQConnection";

        private readonly jpmorgan.mina.common.ISession _minaProtocolSession;

        /// <summary>
        /// Maps from the channel id to the AMQSession that it represents.
        /// </summary>
        //private ConcurrentMap _channelId2SessionMap = new ConcurrentHashMap();
        private Hashtable _channelId2SessionMap = Hashtable.Synchronized(new Hashtable());

        //private ConcurrentMap _closingChannels = new ConcurrentHashMap();
        private Hashtable _closingChannels = Hashtable.Synchronized(new Hashtable());

        /// <summary>
        /// Maps from a channel id to an unprocessed message. This is used to tie together the
        /// JmsDeliverBody (which arrives first) with the subsequent content header and content bodies.
        /// </summary>
        //private ConcurrentMap _channelId2UnprocessedMsgMap = new ConcurrentHashMap();
        private Hashtable _channelId2UnprocessedMsgMap = Hashtable.Synchronized(new Hashtable());

        public AMQProtocolSession(jpmorgan.mina.common.ISession protocolSession, AMQConnection connection)
        {
            _minaProtocolSession = protocolSession;
            // properties of the connection are made available to the event handlers
            _minaProtocolSession.SetAttribute(AMQ_CONNECTION, connection);

            // start the process of setting up the connection. This is the first place that
            // data is written to the server.
            _minaProtocolSession.Write(new ProtocolInitiation());
        }

        public string ClientID
        {
            get
            {            
                return AMQConnection.ClientID;
            }            
            set
            {
               AMQConnection.ClientID = value; 
            }
        }
        
        public string VirtualPath
        {
            get
            {
                return AMQConnection.VirtualPath;
            }            
        }

        public string Username
        {
            get
            {
                return AMQConnection.Username;
            }            
        }

        public string Password
        {
            get
            {
                return AMQConnection.Password;
            }            
        }

        public jpmorgan.mina.common.ISession Session
        {
            get
            {
                return _minaProtocolSession;
            }
            
        }

        public string SecurityMechanism
        {
            get
            {
                return (string) _minaProtocolSession.GetAttribute(SECURITY_MECHANISM);
            }
            set
            {
                _minaProtocolSession.SetAttribute(SECURITY_MECHANISM, value);
            }
        }        

        public ConnectionTuneParameters ConnectionTuneParameters
        {
            get
            {
                return (ConnectionTuneParameters) _minaProtocolSession.GetAttribute(CONNECTION_TUNE_PARAMETERS);
            }            
            set
            {
                _minaProtocolSession.SetAttribute(CONNECTION_TUNE_PARAMETERS, value);
                AMQConnection con = this.AMQConnection;
                con.MaximumChannelCount = value.ChannelMax;
                con.MaximumFrameSize = value.FrameMax;
            }
        }
        
        /// <summary>
        /// Callback invoked from the BasicDeliverMethodHandler when a message has been received.
        /// This is invoked on the MINA dispatcher thread.
        /// </summary>
        /// <param name="message">the unprocessed message</param>
        /// <exception cname="AMQException">if this was not expected</exception>         
        public void UnprocessedMessageReceived(UnprocessedMessage message)
        {
            _channelId2UnprocessedMsgMap[message.ChannelId] = message;
        }

        public void MessageContentHeaderReceived(ushort channelId, ContentHeaderBody contentHeader)                
        {
            UnprocessedMessage msg = (UnprocessedMessage) _channelId2UnprocessedMsgMap[channelId];
            if (msg == null)
            {
                throw new AMQException("Error: received content header without having received a JMSDeliver frame first");
            }
            if (msg.ContentHeader != null)
            {
                throw new AMQException("Error: received duplicate content header or did not receive correct number of content body frames");
            }
            msg.ContentHeader = contentHeader;
            if (contentHeader.BodySize == 0)
            {
                DeliverMessageToAMQSession(channelId, msg);
            }
        }

        public void MessageContentBodyReceived(ushort channelId, ContentBody contentBody)
        {
            UnprocessedMessage msg = (UnprocessedMessage) _channelId2UnprocessedMsgMap[channelId];
            if (msg == null)
            {
                throw new AMQException("Error: received content body without having received a JMSDeliver frame first");
            }
            if (msg.ContentHeader == null)
            {
                _channelId2UnprocessedMsgMap.Remove(channelId);
                throw new AMQException("Error: received content body without having received a ContentHeader frame first");
            }
            try
            {
                msg.ReceiveBody(contentBody);
            }
            catch (UnexpectedBodyReceivedException e)
            {
                _channelId2UnprocessedMsgMap.Remove(channelId);
                throw e;
            }
            if (msg.IsAllBodyDataReceived())
            {
                DeliverMessageToAMQSession(channelId,  msg);
            }
        }

        /// <summary>
        /// Deliver a message to the appropriate session, removing the unprocessed message
        /// from our map
        /// <param name="channelId">the channel id the message should be delivered to</param>
        /// <param name="msg"> the message</param>         
        private void DeliverMessageToAMQSession(ushort channelId, UnprocessedMessage msg)
        {
            AMQSession session = (AMQSession) _channelId2SessionMap[channelId];
            session.MessageReceived(msg);
            _channelId2UnprocessedMsgMap.Remove(channelId);
        }

        /// <summary>
        /// Convenience method that writes a frame to the protocol session. Equivalent
        /// to calling getProtocolSession().write().
        /// </summary>
        /// <param name="frame">the frame to write</param>
        public void WriteFrame(IDataBlock frame)
        {
            _minaProtocolSession.Write(frame);
        }

        public void AddSessionByChannel(ushort channelId, AMQSession session)
        {            
            if (session == null)
            {
                throw new ArgumentNullException("Attempt to register a null session");
            }
            _logger.Debug("Add session with channel id  " + channelId);
            _channelId2SessionMap[channelId] = session;
        }

        public void RemoveSessionByChannel(ushort channelId)
        {            
            _logger.Debug("Removing session with channelId " + channelId);
            _channelId2SessionMap.Remove(channelId);
        }

        /// <summary>
        /// Starts the process of closing a session
        /// </summary>
        /// <param name="session" the AMQSession being closed</param>         
        public void CloseSession(AMQSession session)
        {
            _logger.Debug("closeSession called on protocol session for session " + session.ChannelId);
            ushort channelId = session.ChannelId;
            
            // we need to know when a channel is closing so that we can respond
            // with a channel.close frame when we receive any other type of frame
            // on that channel
            _closingChannels[channelId] = session;

            AMQFrame frame = ChannelCloseBody.CreateAMQFrame(channelId, 200,
                                                             "JMS client closing channel", 0, 0);
            WriteFrame(frame);
        }

        /// <summary>
        /// Called from the ChannelClose handler when a channel close frame is received.
        /// This method decides whether this is a response or an initiation. The latter
        /// case causes the AMQSession to be closed and an exception to be thrown if
        /// appropriate.
        /// </summary>
        /// <param name="channelId">the id of the channel (session)</param>
        /// <returns>true if the client must respond to the server, i.e. if the server
        /// initiated the channel close, false if the channel close is just the server
        /// responding to the client's earlier request to close the channel.</returns>         
        public bool ChannelClosed(ushort channelId, int code, string text)
        {            
            // if this is not a response to an earlier request to close the channel            
            if (!_closingChannels.ContainsKey(channelId))
            {
                _closingChannels.Remove(channelId);
                AMQSession session = (AMQSession) _channelId2SessionMap[channelId];
                session.SessionClosed(new AMQException(_logger, code, text));
                return true;
            }
            else
            {
                return false;
            }
        }

        public AMQConnection AMQConnection        
        {
            get
            {
                return (AMQConnection) _minaProtocolSession.GetAttribute(AMQ_CONNECTION);
            }            
        }

        public void CloseProtocolSession()
        {
            _logger.Debug("Closing protocol session");
            _minaProtocolSession.Close();
        }
    }
}
