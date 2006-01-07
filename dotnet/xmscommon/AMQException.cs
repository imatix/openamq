using System;
using log4net;

namespace OpenAMQ
{

    /// <summary>
    /// The generic AMQ exception.
    /// </summary>
    public class AMQException : Exception
    {
        private int _errorCode;

        public AMQException(string message)
            : base(message)
        {
        }

        public AMQException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        public AMQException(int errorCode, string message)
            : base(message)
        {
            _errorCode = errorCode;
        }

        public AMQException(int errorCode, string message, Exception innerException)
            : base(message, innerException)
        {
            _errorCode = errorCode;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="T:AMQException"/> class, with a logger that will
        /// be used to output log information upon construction. This saves having to log separately.
        /// </summary>
        /// <param name="logger">The logger.</param>
        /// <param name="message">The message.</param>
        public AMQException(ILog logger, string message)
            : base(message)
        {
            logger.Error(message);
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="T:AMQException"/> class, with a logger that will
        /// be used to output log information upon construction. This saves having to log separately.
        /// </summary>
        /// <param name="logger">The logger.</param>
        /// <param name="message">The message.</param>
        /// <param name="innerException">The root cause</param>
        public AMQException(ILog logger, string message, Exception innerException)
            : base(message, innerException)
        {
            logger.Error(message, innerException);
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="T:AMQException"/> class, with a logger that will
        /// be used to output log information upon construction. This saves having to log separately.
        /// </summary>
        /// <param name="logger">The logger.</param>
        /// <param name="message">The message.</param>
        /// <param name="errorCode">The AMQ error code. See RFC 006 for details of error codes</param>
        public AMQException(ILog logger, int errorCode, string message)
            : this(errorCode, message)
        {
            logger.Error(message);
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="T:AMQException"/> class, with a logger that will
        /// be used to output log information upon construction. This saves having to log separately.
        /// </summary>
        /// <param name="logger">The logger.</param>
        /// <param name="message">The message.</param>
        /// <param name="errorCode">The AMQ error code. See RFC 006 for details of error codes</param>
        /// <param name="innerException">The root cause</param>
        public AMQException(ILog logger, int errorCode, string message, Exception innerException)
            : this(errorCode, message, innerException)
        {
            logger.Error(message, innerException);
        }

        /// <summary>
        /// Gets or sets the error code. See RFC 006 for details of error codes.
        /// </summary>
        /// <value>The error code.</value>
        public int ErrorCode
        {
            get
            {
                return _errorCode;
            }
            set
            {
                _errorCode = value;
            }
        }

    }
}
