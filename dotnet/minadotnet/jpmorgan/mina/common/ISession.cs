using System;
using System.Collections;
using System.Net;
using System.Text;
using Iesi.Collections;

namespace jpmorgan.mina.common
{
    ///
    /// A handle which represents connection between two endpoints regardless of 
    /// transport types.
    /// <p>
    /// {@link IoSession} provides user-defined attributes.  User-defined attributes
    /// are application-specific data which is associated with a session.
    /// It often contains objects that represents the state of a higher-level protocol
    /// and becomes a way to exchange data between filters and handlers.
    /// 
    /// <h3>Adjusting Transport Type Specific Properties</h3>
    /// <p>
    /// You can simply downcast the session to an appropriate subclass.
    /// </p>
    /// 
    /// <h3>Thread Safety</h3>
    /// <p>
    /// {@link IoSession} is thread-safe.  But please note that performing
    /// more than one {@link #write(Object)} calls at the same time will
    /// cause the {@link IoFilter#filterWrite(IoFilter.NextFilter, IoSession, IoFilter.WriteRequest)}
    /// is executed simnutaneously, and therefore you have to make sure the
    /// {@link IoFilter} implementations you're using are thread-safe, too. 
    /// </p>
    public interface ISession
    {
        ///
        /// Returns the {@link ISessionManager} which manages this session.
        ///
        ISessionManager Manager
        {
            get;
        }

        ///
        /// Returns the {@link IoHandler} which handles this session.
        ///
        IHandler Handler
        {
            get;
        }

        ///
        /// Returns the filter chain that only affects this session.
        ///
        IFilterChain FilterChain
        {
            get;
        }

        ///
        /// Writes the specified <code>message</code> to remote peer.  This
        /// operation is asynchronous; {@link IoHandler#messageSent(IoSession, Object)}
        /// will be invoked when the message is actually sent to remote peer.
        /// You can also ait for the returned {@link WriteFuture} if you want
        /// to wait for the session actually closed.
        ///
        WriteFuture Write(object message);

        ///
        /// Closes this session immediately.  This operation is asynthronous.
        /// Wait for the returned {@link CloseFuture} if you want to wait for
        /// the session actually closed.
        ///
        CloseFuture Close();

        ///
        /// Returns an attachment of this session.
        /// This method is identical with <tt>getAttribute( "" )</tt>.
        ///
        object Attachment
        {
            get;
            set;
        }        

        ///
        /// Returns the value of user-defined attribute of this session.
        /// 
        /// @param key the key of the attribute
        /// @return <tt>null</tt> if there is no attribute with the specified key
        ///
        object GetAttribute(string key);

        ///
        /// Sets a user-defined attribute.
        /// 
        /// @param key the key of the attribute
        /// @param value the value of the attribute        
        ///
        void SetAttribute(string key, object value);

        ///
        /// Sets a user defined attribute without a value.  This is useful when
        /// you just want to put a 'mark' attribute.  Its value is set to
        /// {@link Boolean#TRUE}.
        /// 
        /// @param key the key of the attribute
        /// @return The old value of the attribute.  <tt>null</tt> if it is new.       
        void SetAttribute(string key);

        ///
        /// Removes a user-defined attribute with the specified key.
        ///        
        void RemoveAttribute(String key);

        ///
        /// Returns <tt>true</tt> if this session contains the attribute with
        /// the specified <tt>key</tt>.       
        bool ContainsAttribute(string key);

        ///
        /// Returns the set of keys of all user-defined attributes.
        ///
        ICollection AttributeKeys
        {
            get;
        }

        ///
        /// Returns transport type of this session.
        ///
        TransportType TransportType
        {
            get;
        }

        ///
        /// Returns <code>true</code> if this session is connected with remote peer.        
        bool Connected
        {
            get;
        }

        ///
        /// Returns <code>true</tt> if and only if this session is being closed
        /// (but not disconnected yet) or is closed.        
        bool IsClosing();

        ///
        /// Returns the {@link CloseFuture} of this session.  This method returns
        /// the same instance whenever user calls it.         
        CloseFuture CloseFuture
        {
            get;
        }

        ///
        /// Returns the socket address of remote peer.          
        IPEndPoint RemoteEndPoint
        {
            get;
        }

        ///
        /// Returns the socket address of local machine which is associated with this
        /// session.         
        IPEndPoint LocalEndPoint
        {
            get;
        }

        ///
        /// Returns idle time for the specified type of idleness in seconds.
        int GetIdleTime(IdleStatus status);

        ///
        /// Returns idle time for the specified type of idleness in milliseconds.        
        long GetIdleTimeInMillis(IdleStatus status);

        ///
        /// Sets idle time for the specified type of idleness in seconds.
        ///
        void SetIdleTime(IdleStatus status, int idleTime);

        ///
        /// Returns write timeout in seconds.        
        int WriteTimeout
        {
            get;
            set;
        }

        ///
        /// Returns write timeout in milliseconds.         
        long WriteTimeoutInMillis
        {
            get;
        }        

        ///
        /// Returns the current {@link TrafficMask} of this session.         
        TrafficMask TrafficMask
        {
            get;
            set;
        }        

        ///
        /// A shortcut method for {@link #setTrafficMask(TrafficMask)} that
        /// suspends read operations for this session.         
        void SuspendRead();

        ///
        /// A shortcut method for {@link #setTrafficMask(TrafficMask)} that
        /// suspends write operations for this session.        
        void SuspendWrite();

        ///
        /// A shortcut method for {@link #setTrafficMask(TrafficMask)} that
        /// resumes read operations for this session.         
        void ResumeRead();

        ///
        /// A shortcut method for {@link #setTrafficMask(TrafficMask)} that
        /// resumes write operations for this session.        
        void ResumeWrite();

        ///
        /// Returns the total number of bytes which were read from this session.        
        long ReadBytes
        {
            get;
        }

        ///
        /// Returns the total number of bytes which were written to this session.         
        long WrittenBytes
        {
            get;
        }

        ///
        /// Returns the total number of write requests which were written to this session.         
        long WrittenWriteRequests
        {
            get;
        }

        ///
        /// Returns the number of write requests which are scheduled to be written
        /// to this session.         
        int ScheduledWriteRequests
        {
            get;
        }

        ///
        /// Returns the time in millis when this session is created.         
        long CreationTime
        {
            get;
        }

        ///
        /// Returns the time in millis when I/O occurred lastly.         
        long LastIoTime
        {
            get;
        }

        ///
        /// Returns the time in millis when read operation occurred lastly.         
        long LastReadTime
        {
            get;
        }

        ///
        /// Returns the time in millis when write operation occurred lastly.        
        long LastWriteTime
        {
            get;
        }

        ///
        /// Returns <code>true</code> if this session is idle for the specified 
        /// {@link IdleStatus}.         
        bool IsIdle(IdleStatus status);

        ///
        /// Returns the number of the fired continuous <tt>sessionIdle</tt> events
        /// for the specified {@link IdleStatus}.
        /// <p>
        /// If <tt>sessionIdle</tt> event is fired first after some time after I/O,
        /// <tt>idleCount</tt> becomes <tt>1</tt>.  <tt>idleCount</tt> resets to
        /// <tt>0</tt> if any I/O occurs again, otherwise it increases to
        /// <tt>2</tt> and so on if <tt>sessionIdle</tt> event is fired again without
        /// any I/O between two (or more) <tt>sessionIdle</tt> events.         
        int GetIdleCount(IdleStatus status);

        ///
        /// Returns the time in millis when the last <tt>sessionIdle</tt> event
        /// is fired for the specified {@link IdleStatus}.         
        long GetLastIdleTime(IdleStatus status);
    }
}
