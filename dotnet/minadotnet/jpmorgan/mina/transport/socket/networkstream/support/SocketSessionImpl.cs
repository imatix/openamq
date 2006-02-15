using System;
using System.Collections;
using System.Net;
using System.Net.Sockets;
using jpmorgan.mina.common;

namespace jpmorgan.mina.transport.socket.networkstream.support
{
    public class SocketSessionImpl : SocketSession
    {
        public const int DEFAULT_RECEIVE_BUFFER_SIZE = 8192;

        private const string ATTACHMENT_PROPERTY_NAME = "__ATTACHMENT";

        private int _receiveBufferSize = DEFAULT_RECEIVE_BUFFER_SIZE;

        private readonly IHandler _handler;

        private readonly Socket _socket;

        private readonly Hashtable _attributeMap = new Hashtable();

        private SocketFilterChain _filterChain;

        private CloseFuture _closeFuture = new CloseFuture();

        private ByteBuffer _buffer;
        
        public SocketSessionImpl(Socket socket, IHandler handler)
        {
            _socket = socket;
            _handler = handler;
            _filterChain = new SocketFilterChain(this);
        }

        #region SocketSession Members

        public Socket Socket
        {
            get
            {
                return _socket;
            }
        }

        public int ReceiveBufferSize
        {
            get
            {
                return _receiveBufferSize;
            }

            set
            {
                _receiveBufferSize = value;
            }
        }

        #endregion

        #region ISession Members

        public jpmorgan.mina.common.ISessionManager Manager
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public IHandler Handler
        {
            get
            {
                return _handler;
            }
        }

        public IFilterChain FilterChain
        {
            get { return _filterChain; }
        }

        public WriteFuture Write(object message)
        {
            WriteFuture future = new WriteFuture();            
            _filterChain.FilterWrite(new WriteRequest(message, future));            
            return future;
        }

        public CloseFuture Close()
        {        
            _filterChain.FilterClose(_closeFuture);
            return _closeFuture;
        }

        public object Attachment
        {
            get
            {
                return _attributeMap[ATTACHMENT_PROPERTY_NAME];
            }
            set
            {
                _attributeMap[ATTACHMENT_PROPERTY_NAME] = value;
            }
        }

        public object GetAttribute(string key)
        {
            return _attributeMap[key];
        }

        public void SetAttribute(string key, object value)
        {
            _attributeMap[key] = value;
        }

        public void SetAttribute(string key)
        {
            _attributeMap[key] = true;
        }

        public void RemoveAttribute(string key)
        {
            _attributeMap.Remove(key);
        }

        public bool ContainsAttribute(string key)
        {
            return _attributeMap.Contains(key);
        }

        public ICollection AttributeKeys
        {
            get
            {
                return _attributeMap.Keys;
            }
        }

        public jpmorgan.mina.common.TransportType TransportType
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public bool Connected
        {
            get
            {
                throw new Exception("The method or operation is not implemented.");
            }
        }

        public bool IsClosing()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public jpmorgan.mina.common.CloseFuture CloseFuture
        {
            get { return _closeFuture; }
        }

        public System.Net.IPEndPoint RemoteEndPoint
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public System.Net.IPEndPoint getLocalEndPoint
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public int GetIdleTime(jpmorgan.mina.common.IdleStatus status)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public long GetIdleTimeInMillis(jpmorgan.mina.common.IdleStatus status)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SetIdleTime(jpmorgan.mina.common.IdleStatus status, int idleTime)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public int WriteTimeout
        {
            get
            {
                throw new Exception("The method or operation is not implemented.");
            }
            set
            {
                throw new Exception("The method or operation is not implemented.");
            }
        }

        public long WriteTimeoutInMillis
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public jpmorgan.mina.common.TrafficMask TrafficMask
        {
            get
            {
                throw new Exception("The method or operation is not implemented.");
            }
            set
            {
                throw new Exception("The method or operation is not implemented.");
            }
        }

        public void SuspendRead()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void SuspendWrite()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void ResumeRead()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void ResumeWrite()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public long ReadBytes
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public long WrittenBytes
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public long WrittenWriteRequests
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public int ScheduledWriteRequests
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public long CreationTime
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public long LastIoTime
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public long LastReadTime
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public long LastWriteTime
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public bool IsIdle(jpmorgan.mina.common.IdleStatus status)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public int GetIdleCount(jpmorgan.mina.common.IdleStatus status)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public long GetLastIdleTime(jpmorgan.mina.common.IdleStatus status)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion

        #region SocketSession Members


        public ByteBuffer Buffer
        {
            get
            {
                return _buffer;
            }
            set
            {
                _buffer = value;
            }
        }

        #endregion

        #region ISession Members


        public IPEndPoint LocalEndPoint
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        #endregion
    }
}
