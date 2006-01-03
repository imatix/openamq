using System;
using System.Collections.Generic;
using System.Text;

namespace jpmorgan.mina.common
{
    /// Handles all protocol events fired by MINA.
    /// There are 6 event handler methods, and they are all invoked by MINA
    /// automatically.
    public interface IHandler
    {
        ///
        /// Invoked when the session is created.  Initialize default socket
        /// parameters and user-defined attributes here.
        ///
        void SessionCreated(ISession session);
        
        ///
        /// Invoked when the connection is opened.  This method is not invoked if the
        /// transport type is UDP.
        ///
        void SessionOpened(ISession session);

        ///
        /// Invoked when the connection is closed.  This method is not invoked if the
        /// transport type is UDP.
        ///
        void SessionClosed(ISession session);

        ///
        /// Invoked when the connection is idle.  Refer to {@link IdleStatus}.  This
        /// method is not invoked if the transport type is UDP.
        ///
        void SessionIdle(ISession session, IdleStatus status);

        ///
        /// Invoked when any exception is thrown by user {@link IoHandler}
        /// implementation or by MINA.  If <code>cause</code> is instanceof
        /// {@link IOException}, MINA will close the connection automatically.
        ///
        void ExceptionCaught(ISession session, Exception cause);

        ///
        /// Invoked when protocol message is received.  Implement your protocol flow
        /// here.
        ///
        void MessageReceived(ISession session, object message);

        ///
        /// Invoked when protocol message that user requested by
        /// {@link IoSession#write(Object)} is sent out actually.
        ///
        void MessageSent(ISession session, object message);
    }
}
