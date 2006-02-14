using System;
using System.Collections;
using jpmorgan.mina.common;
using jpmorgan.mina.filter.codec;
using jpmorgan.mina.transport.socket.networkstream;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.State;
using OpenAMQ.XMS.Client.State.Listener;
using OpenAMQ.XMS.Client.Transport;

namespace OpenAMQ.XMS.Client.Protocol
{
    public class AMQProtocolHandler : HandlerAdapter
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(AMQProtocolHandler));

        /**
         * The connection that this protocol handler is associated with. There is a 1-1
         * mapping between connection instances and protocol handler instances.
         */
        private AMQConnection _connection;

        /**
         * Our wrapper for a protocol session that provides access to session values
         * in a typesafe manner.
         */
        private AMQProtocolSession _protocolSession;

        private readonly AMQStateManager _stateManager = new AMQStateManager();

        //private readonly CopyOnWriteArraySet _frameListeners = new CopyOnWriteArraySet();
        private readonly ArrayList _frameListeners = ArrayList.Synchronized(new ArrayList());

        public AMQProtocolHandler(AMQConnection con)
        {
            _connection = con;
            _frameListeners.Add(_stateManager);
        }

        public override void SessionCreated(jpmorgan.mina.common.ISession session)
        {
            if (session is SocketSession)
            {
                SocketSession socketSession = (SocketSession) session;
                _logger.Info("Setting socket receive buffer size to 8192 bytes");
                socketSession.ReceiveBufferSize = 8192;
            }

            AMQProtocolProvider provider = new AMQProtocolProvider();
            session.FilterChain.AddLast("protocolFilter",
                                        new ProtocolCodecFilter(provider.CodecFactory));
        }

        public override void SessionOpened(jpmorgan.mina.common.ISession session)
        {
            _protocolSession = new AMQProtocolSession(session, _connection);
        }

        public override void SessionClosed(jpmorgan.mina.common.ISession session)
        {
            // we only raise an exception if the close was not initiated by the client
            if (!_connection.Closed)
            {
                _connection.ExceptionReceived(new AMQDisconnectedException("Server closed connection"));
            }

            _logger.Info("Protocol Session closed");
        }

        public override void SessionIdle(jpmorgan.mina.common.ISession session, IdleStatus status)
        {
            _logger.Info("Protocol Session idle");
        }

        public override void ExceptionCaught(jpmorgan.mina.common.ISession session, Exception cause)
        {
            _logger.Error("Exception caught by protocol handler: " + cause, cause);
            _connection.ExceptionReceived(cause);
        }

        public override void MessageReceived(jpmorgan.mina.common.ISession session, object message)
        {                        
            AMQFrame frame = (AMQFrame) message;

            if (frame.BodyFrame is AMQMethodBody)
            {
                if (_logger.IsDebugEnabled)
                {
                    _logger.Debug("Method frame received: " + frame);
                }
                AMQMethodEvent evt = new AMQMethodEvent(frame.Channel, (AMQMethodBody)frame.BodyFrame, _protocolSession);
                try
                {
                    bool wasAnyoneInterested = false;
                    foreach (IAMQMethodListener listener in _frameListeners)
                    {                    
                        wasAnyoneInterested = listener.MethodReceived(evt) || wasAnyoneInterested;
                    }
                    if (!wasAnyoneInterested)
                    {
                        throw new AMQException("AMQMethodEvent " + evt + " was not processed by any listener.");
                    }
                }
                catch (Exception e)
                {
                    foreach (IAMQMethodListener listener in _frameListeners)
                    {
                        listener.Error(e);
                    }
                }
            }
            else if (frame.BodyFrame is ContentHeaderBody)
            {
                _protocolSession.MessageContentHeaderReceived(frame.Channel,
                                                              (ContentHeaderBody) frame.BodyFrame);
            }
            else if (frame.BodyFrame is ContentBody)
            {
                _protocolSession.MessageContentBodyReceived(frame.Channel,
                                                            (ContentBody) frame.BodyFrame);
            }
            //_connection.BytesReceived(_protocolSession.Session.ReadBytes); is this really useful?
        }

        private static int _messagesOut;

        public override void MessageSent(jpmorgan.mina.common.ISession session, object message)
        {
            if (_messagesOut++ % 1000 == 0)
            {
                _logger.Warn("Sent " + _messagesOut + " protocol messages");
            }
            //_connection.bytesSent(_protocolSession.Session.WrittenBytes);
            if (_logger.IsDebugEnabled)
            {
                _logger.Debug("Sent frame " + message);
            }
        }

        public void AddFrameListener(IAMQMethodListener listener)
        {
            _frameListeners.Add(listener);
        }

        public void RemoveFrameListener(IAMQMethodListener listener)
        {
            _frameListeners.Remove(listener);
        }

        public void AttainState(AMQState s)
        {
            _stateManager.AttainState(s);
        }

        ///
        /// Convenience method that writes a frame to the protocol session. Equivalent
        /// to calling getProtocolSession().write().
        ///
        /// <param name="frame">the frame to write</param>         
        public void WriteFrame(IDataBlock frame)
        {
            _protocolSession.WriteFrame(frame);
        }

        /// <summary>
        /// Convenience method that writes a frame to the protocol session and waits for
        /// a particular response. Equivalent to calling getProtocolSession().write() then
        /// waiting for the response.
        /// </summary>
        /// <param name="frame">the frame</param>
        /// <param name="listener">the blocking listener. Note the calling thread will block.</param>         
        public AMQMethodEvent WriteCommandFrameAndWaitForReply(AMQFrame frame,
                                                               BlockingMethodFrameListener listener)            
        {
            _frameListeners.Add(listener);
            _protocolSession.WriteFrame(frame);
            return listener.BlockForFrame();
            // When control resumes before this line, a reply will have been received
            // that matches the criteria defined in the blocking listener
        }

        /// <summary>
        /// Convenience method to register an AMQSession with the protocol handler. Registering
        /// a session with the protocol handler will ensure that messages are delivered to the
        /// consumer(s) on that session.
        /// </summary>
        /// <param name="channelId">the channel id of the session</param>
        /// <param name="session">the session instance.</param>         
        public void AddSessionByChannel(ushort channelId, AMQSession session)
        {
            _protocolSession.AddSessionByChannel(channelId, session);
        }

        /// <summary>
        /// Convenience method to deregister an AMQSession with the protocol handler.
        /// </summary>
        /// <param name="channelId">then channel id of the session</param>         
        public void RemoveSessionByChannel(ushort channelId)
        {
            _protocolSession.RemoveSessionByChannel(channelId);
        }

        public void CloseSession(AMQSession session)
        {
            BlockingMethodFrameListener listener = new SpecificMethodFrameListener(session.ChannelId,
                                                                                   typeof(ChannelCloseOkBody));
            _frameListeners.Add(listener);
            _protocolSession.CloseSession(session);
            _logger.Debug("Blocking for channel close frame for channel " + session.ChannelId);
            listener.BlockForFrame();
            _logger.Debug("Received channel close frame");
            // When control resumes at this point, a reply will have been received that
            // indicates the broker has closed the channel successfully
        }

        public void CloseConnection()
        {
            BlockingMethodFrameListener listener = new ConnectionCloseOkListener();
            _frameListeners.Add(listener);
            _stateManager.ChangeState(AMQState.CONNECTION_CLOSING);

            // TODO: Polish
            AMQFrame frame = ConnectionCloseBody.CreateAMQFrame(0, 200,
                                                                "XMS client is closing the connection.", 0, 0);
            WriteFrame(frame);
            _logger.Debug("Blocking for connection close ok frame");
            listener.BlockForFrame();
            _protocolSession.CloseProtocolSession();
        }

        /**
         * @return the number of bytes read from this protocol session
         */
        public long ReadBytes
        {
            get
            {
                return _protocolSession.Session.ReadBytes;
            }
        }

        /**
         * @return the number of bytes written to this protocol session
         */
        public long WrittenBytes
        {
            get
            {
                return _protocolSession.Session.WrittenBytes;
            }
        }
    }
}
