using System;
using System.Collections;
using System.IO;
using System.Threading;
using jpmorgan.mina.common;
using jpmorgan.mina.common.support;
using log4net;
using IBM.XMS;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State;
using OpenAMQ.XMS.Client.State.Listener;

namespace OpenAMQ.XMS.Client
{
    public class AMQConnection : Closeable, IConnection 
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(AMQConnection));

        private readonly IdFactory _idFactory = new IdFactory();

        private TransportConnection _transportConnection;

        /// <summary>
        /// A channel is roughly analogous to a session. The server can negotiate the maximum number of channels
        /// per session and we must prevent the client from opening too many. Zero means unlimited.
        /// </summary>
        private ushort _maximumChannelCount;

        /// <summary>
        /// The maximum size of frame supported by the server
        /// </summary>
        private uint _maximumFrameSize;

        private AMQProtocolHandler _protocolHandler;

        /// <summary>
        /// Maps from session id (Integer) to AMQSession instance
        /// </summary>
        private readonly LinkedHashtable _sessions = new LinkedHashtable();

        private string _clientName;

        /// <summary>
        /// The host to which the connection is connected
        /// </summary>
        private string _host;

        /// <summary>
        /// The port on which the connection is made
        /// </summary>
        private int _port;

        /// <summary>
        /// The user name to use for authentication
        /// </summary>
        private string _username;

        /// <summary>
        /// The password to use for authentication
        /// </summary>
        private string _password;

        /// <summary>
        /// The virtual path to connect to on the AMQ server
        /// </summary>
        private string _virtualPath;

        private ExceptionListener _exceptionListener;

        private IConnectionListener _connectionListener;

        /// <summary>
        /// Whether this connection is started, i.e. whether messages are flowing to consumers. It has no meaning for
        /// message publication.
        /// </summary>
        private bool _started;

        public AMQConnection(string host, int port, string username, string password,
                             string clientName, string virtualPath)
        {
            _clientName = clientName;
            _host = host;
            _port = port;
            _username = username;
            _password = password;
            _virtualPath = virtualPath;
            
            _transportConnection = new TransportConnection(this);
            _protocolHandler = _transportConnection.Connect();
            // this blocks until the connection has been set up
            _protocolHandler.AttainState(AMQState.CONNECTION_OPEN);
        }

        #region IConnection Members

        public string ClientID
        {
            get
            {
                CheckNotClosed();
                return _clientName;
            }
            set
            {
                CheckNotClosed();
                _clientName = value;
            }
        }

        public override void Close()
        {
            // atomically set to closed and check the previous value was NOT CLOSED
            if (Interlocked.Exchange(ref _closed, CLOSED) == NOT_CLOSED)            
            {
                try
                {
                    CloseAllSessions(null);
                    _protocolHandler.CloseConnection();
                }
                catch (AMQException e)
                {
                    throw new XMSException("Error closing connection: " + e);
                }
            }
        }

        public IBM.XMS.ISession CreateSession(bool transacted, AcknowledgeMode acknowledgeMode)
        {
            CheckNotClosed();
            if (ChannelLimitReached())
            {
                throw new ChannelLimitReachedException(_maximumChannelCount);
            }
            else
            {
                // TODO: check thread safety
                ushort channelId = _idFactory.ChannelId;
                AMQFrame frame = ChannelOpenBody.CreateAMQFrame(channelId, 100,
                                                                null);

                if (_logger.IsDebugEnabled)
                {
                    _logger.Debug("Write channel open frame for channel id " + channelId);
                }

                // we must create the session and register it before actually sending the frame to the server to
                // open it, so that there is no window where we could receive data on the channel and not be set up to
                // handle it appropriately.
                AMQSession session = new AMQSession(this, channelId, transacted, acknowledgeMode);
                _protocolHandler.AddSessionByChannel(channelId, session);
                RegisterSession(channelId, session);

                try
                {
                    _protocolHandler.WriteCommandFrameAndWaitForReply(frame,
                                                                      new SpecificMethodFrameListener(channelId,
                                                                                                      typeof(ChannelOpenOkBody)));
                }
                catch (Exception e)
                {
                    _protocolHandler.RemoveSessionByChannel(channelId);
                    DeregisterSession(channelId);
                    throw new XMSException("Error creating session: " + e);
                }
                return session;
            }
        }

        public ExceptionListener ExceptionListener
        {
            get
            {
                CheckNotClosed();
                return _exceptionListener;
            }
            set
            {
                CheckNotClosed();
                _exceptionListener = value;
            }
        }

        public IConnectionMetaData MetaData
        {
            get
            {
                CheckNotClosed();
                // TODO implement
                throw new Exception("Not implemented");                
            }
        }

        /// <summary>
        /// Start the connection, i.e. start flowing messages. Note that this method must be called only from a single thread
        /// and is not thread safe (which is legal according to the JMS specification).
        /// @throws JMSException
        /// </summary>
        public void Start()
        {
            CheckNotClosed();
            if (!_started)
            {
                foreach (LinkedHashtable.LinkedDictionaryEntry lde in _sessions)
                {                   
                    AMQSession s = (AMQSession)lde.value;
                    s.Start();
                }
                _started = true;
            }
        }

        public void Stop()
        {
            CheckNotClosed();
            throw new Exception("Not implemented");
        }

        #endregion

        #region IPropertyContext Members

        public bool GetBooleanProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public byte GetByteProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public byte[] GetBytesProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public char GetCharProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public double GetDoubleProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public float GetFloatProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public int GetIntProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public long GetLongProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public object GetObjectProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public short GetShortProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public short GetSignedByteProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public string GetStringProperty(string property_name)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public bool PropertyExists(string propertyName)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetBooleanProperty(string property_name, bool value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetByteProperty(string property_name, byte value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetBytesProperty(string property_name, byte[] value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetCharProperty(string property_name, char value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetDoubleProperty(string property_name, double value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetFloatProperty(string property_name, float value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetIntProperty(string property_name, int value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetLongProperty(string property_name, long value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetObjectProperty(string property_name, object value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetShortProperty(string property_name, short value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetSignedByteProperty(string property_name, short value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetStringProperty(string property_name, string value)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion

        #region IDisposable Members

        public void Dispose()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion

        private bool ChannelLimitReached()
        {
            return _maximumChannelCount != 0 && _sessions.Count == _maximumChannelCount;
        }

        /// <summary>
        /// Close all the sessions, either due to normal connection closure or due to an error occurring.
        /// @param cause if not null, the error that is causing this shutdown
        /// </summary>
        private void CloseAllSessions(Exception cause)
        {
            foreach (LinkedHashtable.LinkedDictionaryEntry lde in _sessions)
            {
                AMQSession session = (AMQSession) lde.value;
                if (cause != null)
                {
                    session.Closed(cause);
                }
                else
                {
                    try
                    {
                        session.Close();
                    }
                    catch (XMSException e)
                    {
                        _logger.Error("Error closing session: " + e);
                    }
                }
            }
        }

        internal IdFactory IdFactory
        {
            get
            {
                return _idFactory;
            }
        }

        public ushort MaximumChannelCount
        {
            get
            {
                CheckNotClosed();
                return _maximumChannelCount;
            }

            set
            {
                CheckNotClosed();
                _maximumChannelCount = value;
            }
        }

        public uint MaximumFrameSize
        {
            get
            {
                return _maximumFrameSize;
            }

            set
            {
                _maximumFrameSize = value;
            }
        }

        public IDictionary Sessions
        {
            get
            {
                return _sessions;
            }
        }

        public string Host
        {
            get
            {
                return _host;
            }
        }

        public int Port
        {
            get
            {
                return _port;
            }
        }

        public string Username
        {
            get
            {
                return _username;
            }
        }

        public string Password
        {
            get
            {
                return _password;
            }
        }

        public string VirtualPath
        {
            get
            {
                return _virtualPath;
            }
        }

        public AMQProtocolHandler ProtocolHandler
        {
            get
            {
                return _protocolHandler;
            }
        }

        /// <summary>
        /// Invoked by the AMQProtocolSession when a protocol session exception has occurred.
        /// This method sends the exception to a JMS exception listener, if configured, and
        /// propagates the exception to sessions, which in turn will propagate to consumers.
        /// This allows synchronous consumers to have exceptions thrown to them.
        /// </summary>
        /// <param name="cause">the exception</param>        
        public void ExceptionReceived(Exception cause)
        {
            XMSException xe = null;

            if (_exceptionListener != null)
            {
                if (cause is XMSException)
                {
                    xe = (XMSException) cause;
                }
                else
                {
                    xe = new XMSException("Exception thrown against " + ToString() + ": " + cause);                    
                    xe.LinkedException = cause;
                }
                // in the case of an IOException, MINA has closed the protocol session so we set _closed to true
                // so that any generic client code that tries to close the connection will not mess up this error
                // handling sequence
                if (cause is IOException)
                {
                    Interlocked.Exchange(ref _closed, CLOSED);
                }
                _exceptionListener.Invoke(xe);
            }
            if (xe == null || !(cause is AMQUndeliveredException))
            {
                CloseAllSessions(cause);
            }
        }

        internal void RegisterSession(int channelId, AMQSession session)
        {
            _sessions[channelId] = session;
        }

        internal void DeregisterSession(int channelId)
        {
            _sessions.Remove(channelId);
        }
    }
}
