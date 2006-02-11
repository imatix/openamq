using System;
using System.Threading;
using log4net;
using IBM.XMS;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Message;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State.Listener;
using JPMorgan.Collections;

namespace OpenAMQ.XMS.Client
{
    public class BasicMessageConsumer : Closeable, IMessageConsumer
    {
        private static readonly ILog _logger = LogManager.GetLogger(typeof(BasicMessageConsumer));

        private string _messageSelector;

        private bool _noLocal;

        private AMQDestination _destination;

        private MessageListener _messageListener;

        /// <summary>
        /// The consumer tag allows us to close the consumer by sending a jmsCancel method to the
        /// broker
        /// </summary>
        private string _consumerTag;

        /// <summary>
        /// We need to know the channel id when constructing frames
        /// </summary>
        private ushort _channelId;

        /// <summary>
        /// Protects the setting of a messageListener
        /// </summary>
        private readonly object _syncLock = new object();

        /// <summary>
        /// When true indicates that either a message listener is set or that
        /// a blocking receive call is in progress
        /// </summary>
        private bool _receiving;

        /// <summary>
        /// Used in the blocking receive methods to receive a message from
        /// the Session thread. Argument true indicates we want strict FIFO semantics
        /// </summary>
        private readonly SynchronousQueue _synchronousQueue = new SynchronousQueue(true);

        private MessageFactoryRegistry _messageFactory;

        private AMQSession _session;

        private AMQProtocolHandler _protocolHandler;

        public BasicMessageConsumer(ushort channelId, AMQDestination destination, string messageSelector, bool noLocal,
                                    MessageFactoryRegistry messageFactory, AMQSession session, AMQProtocolHandler protocolHandler)
        {
            _channelId = channelId;
            _messageSelector = messageSelector;
            _noLocal = noLocal;
            _destination = destination;
            _messageFactory = messageFactory;
            _session = session;
            _protocolHandler = protocolHandler;
        }

        #region IMessageConsumer Members

        public MessageListener MessageListener
        {
            get
            {
                return _messageListener;
            }
            set
            {
                CheckNotClosed();

                lock (_syncLock)
                {
                    // If someone is already receiving
                    if (_messageListener != null && _receiving)
                    {
                        throw new IllegalStateException("Another thread is already receiving...");
                    }

                    _messageListener = value;

                    _receiving = (_messageListener != null);

                    if (_receiving)
                    {
                        _logger.Debug("Message listener set for destination " + _destination);
                    }
                }
            }
        }

        public string MessageSelector
        {
            get
            {
                return _messageSelector;
            }
        }

        public IMessage Receive(long delay)
        {
            CheckNotClosed();

            lock (_syncLock)
            {
                // If someone is already receiving
                if (_receiving)
                {
                    throw new IllegalStateException("Another thread is already receiving (possibly asynchronously)...");
                }

                _receiving = true;
            }

            try
            {
                object o;
                if (delay > 0)
                {
                    //o = _synchronousQueue.Poll(l, TimeUnit.MILLISECONDS);
                    throw new NotImplementedException("Need to implement synchronousQueue.Poll(timeout");
                }
                else
                {
                    o = _synchronousQueue.DequeueBlocking();
                }
                return ReturnMessageOrThrow(o);
            }            
            finally
            {
                lock (_syncLock)
                {
                    _receiving = false;
                }
            }
        }

        public IMessage Receive()
        {
            return Receive(0);
        }

        public IMessage ReceiveNoWait()
        {
            CheckNotClosed();

            lock (_syncLock)
            {
                // If someone is already receiving
                if (_receiving)
                {
                    throw new IllegalStateException("Another thread is already receiving (possibly asynchronously)...");
                }

                _receiving = true;
            }

            try
            {
                object o = _synchronousQueue.Dequeue();
                return ReturnMessageOrThrow(o);
            }
            finally
            {
                lock (_syncLock)
                {
                    _receiving = false;
                }
            }
        }

        #endregion

        /// <summary>
        /// We can get back either a Message or an exception from the queue. This method examines the argument and deals
        /// with it by throwing it (if an exception) or returning it (in any other case).
        /// </summary>
        /// <param name="o">the object off the queue</param>
        /// <returns> a message only if o is a Message</returns>
        /// <exception>JMSException if the argument is a throwable. If it is a XMSException it is rethrown as is, but if not
        /// a XMSException is created with the linked exception set appropriately</exception>
        private IMessage ReturnMessageOrThrow(object o)                
        {
            // errors are passed via the queue too since there is no way of interrupting the poll() via the API.
            if (o is Exception)
            {
                XMSException e = new XMSException("Message consumer forcibly closed due to error: " + o);
                if (o is Exception)
                {
                    e.LinkedException = ((Exception) o);
                }
                throw e;
            }
            else
            {
                return (IMessage) o;
            }
        }

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

        public override void Close()
        {
            lock (_closingLock)
            {
                Interlocked.Exchange(ref _closed, CLOSED);

                AMQFrame cancelFrame = BasicCancelBody.CreateAMQFrame(_channelId, _consumerTag);

                try
                {
                    _protocolHandler.WriteCommandFrameAndWaitForReply(cancelFrame,
                                                                      new SpecificMethodFrameListener(_channelId,
                                                                                                      typeof(BasicCancelOkBody)));
                }
                catch (AMQException e)
                {
                    _logger.Error("Error closing consumer: " + e, e);
                    throw new XMSException("Error closing consumer: " + e);
                }
                finally
                {
                    DeregisterConsumer();
                }
            }
        }

        /// <summary>
        /// Called from the AMQSession when a message has arrived for this consumer. This methods handles both the case
        /// of a message listener or a synchronous receive() caller.
        /// </summary>
        /// <param name="messageFrame">the raw unprocessed mesage</param>
        /// <param name="acknowledgeMode">the acknowledge mode requested for this message</param>
        /// <param name="channelId">channel on which this message was sent</param>       
        internal void NotifyMessage(UnprocessedMessage messageFrame, AcknowledgeMode acknowledgeMode, ushort channelId)
        {
            if (_logger.IsDebugEnabled)
            {
                _logger.Debug("notifyMessage called with message number " + messageFrame.DeliverBody.DeliveryTag);
            }
            try
            {
                AbstractXMSMessage jmsMessage = _messageFactory.CreateMessage(messageFrame.DeliverBody.DeliveryTag,
                                                                              messageFrame.DeliverBody.Redelivered,
                                                                              messageFrame.ContentHeader,
                                                                              messageFrame.Bodies);

                /*if (acknowledgeMode == AcknowledgeMode.PreAcknowledge)
                {
                    _session.sendAcknowledgement(messageFrame.deliverBody.deliveryTag);
                }*/
                if (acknowledgeMode == AcknowledgeMode.ClientAcknowledge)
                {
                    // we set the session so that when the user calls acknowledge() it can call the method on session
                    // to send out the appropriate frame
                    jmsMessage.Session = _session;
                }

                lock (_syncLock)
                {
                    if (_messageListener != null)
                    {
                        _messageListener.Invoke(jmsMessage);
                    }
                    else
                    {
                        _synchronousQueue.Enqueue(jmsMessage);
                    }
                }
                if (acknowledgeMode == AcknowledgeMode.AutoAcknowledge)
                {
                    _session.SendAcknowledgement(messageFrame.DeliverBody.DeliveryTag);
                }
            }
            catch (Exception e)
            {
                _logger.Error("Caught exception (dump follows) - ignoring...", e);
            }
        }

        internal void NotifyError(Exception cause)
        {
            lock (_syncLock)
            {
                Interlocked.Exchange(ref _closed, CLOSED);

                // we have no way of propagating the exception to a message listener - a JMS limitation - so we
                // deal with the case where we have a synchronous receive() waiting for a message to arrive
                if (_messageListener == null)
                {
                    // offer only succeeds if there is a thread waiting for an item from the queue
                    if (_synchronousQueue.EnqueueNoThrow(cause))
                    {
                        _logger.Debug("Passed exception to synchronous queue for propagation to receive()");
                    }
                }
                DeregisterConsumer();
            }
        }

        /// <summary>
        /// Perform cleanup to deregister this consumer. This occurs when closing the consumer in both the clean
        /// case and in the case of an error occurring.
        /// </summary>
        internal void DeregisterConsumer()
        {
            _session.DeregisterConsumer(_consumerTag);
        }

        public string ConsumerTag
        {
            get
            {
                return _consumerTag;
            }
            set
            {
                _consumerTag = value;
            }
        }
    }
}
