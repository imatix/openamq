using System;
using System.IO;
using System.Net;
using System.Text;
using System.Threading;
using IBM.XMS;
using log4net;
using log4net.Appender;
using log4net.Config;
using log4net.Layout;
using log4net.spi;
using OpenAMQ;
using OpenAMQ.XMS.Client;
using NUnit.Framework;

namespace JPMorgan.XMS.RequestReply1
{
    [TestFixture]
    public class ServiceRequestingClient : BaseMessagingTestFixture
    {
        private static ILog _logger = LogManager.GetLogger(typeof(ServiceRequestingClient));
        
        private const string MESSAGE_DATA_BYTES = "jfd ghljgl hjvhlj cvhvjf ldhfsj lhfdsjf hldsjfk hdslkfj hsdflk  ";

        private static string MESSAGE_DATA = GetData(1024);

        private static ILog _log = LogManager.GetLogger(typeof(ServiceRequestingClient));        

        private long _averageLatency;

        static String GetData(int size)
        {
            StringBuilder buf = new StringBuilder(size);
            int count = 0;
            while(count < size + MESSAGE_DATA_BYTES.Length)
            {
                buf.Append(MESSAGE_DATA_BYTES);
                count += MESSAGE_DATA_BYTES.Length;
            }
            if(count < size)
            {
                buf.Append(MESSAGE_DATA_BYTES, 0, size - count);
            }

            return buf.ToString();
        }
        
        private int _expectedMessageCount;

        private int _actualMessageCount;

        private long _startTime;        
        
        private string _commandQueueName = "ServiceQ1";
        
        private int _messageCount = 10;

        private AMQQueue _tempDestination;

        private IMessageProducer _producer;
        
        [SetUp]
        public override void Init()
        {
            DOMConfigurator.Configure(new FileInfo("log4net.config"));
            base.Init();
            
            try
            {                
                AMQQueue destination = new AMQQueue(_commandQueueName);
                _producer = (IMessageProducer) _session.CreateProducer(destination);
                _producer.DisableMessageTimestamp = true;
                _producer.DeliveryMode = DeliveryMode.NonPersistent;
                _expectedMessageCount = _messageCount;
                
                _tempDestination = new AMQQueue("TempResponse" +
                                                String.Format("{0}",DateTime.Now.Ticks), true);
                
            }
            catch (XMSException e)
            {
                _log.Error("Error: " + e, e);
            }
        }
        
        [Test]
        public void SendMessages()
        {
            IMessageConsumer messageConsumer = (IMessageConsumer) _session.CreateConsumer(_tempDestination, 100, true,
                                                                                          true, null);

            _startTime = DateTime.Now.Ticks;

            messageConsumer.MessageListener = new MessageListener(OnMessage);
            _connection.Start();
            for (int i = 0; i < _expectedMessageCount; i++)
            {
                ITextMessage msg = _session.CreateTextMessage(MESSAGE_DATA + i);
                msg.JMSReplyTo = _tempDestination;
                if (i % 1000 == 0)
                {
                    long timeNow = DateTime.Now.Ticks;
                    msg.SetStringProperty("timeSent", String.Format("{0:G}", timeNow));
                }
                _producer.Send(msg);
            }
            _log.Info("Finished sending " + _expectedMessageCount + " messages");
            AutoResetEvent evt = new AutoResetEvent(false);
            evt.WaitOne();
        }
        
        public void OnMessage(IMessage m)
        {
            if (_log.IsDebugEnabled)
            {
                _log.Debug("Message received: " + m);
            }
            try
            {
                if (m.PropertyExists("timeSent"))
                {
                    long timeSent = Int64.Parse(m.GetStringProperty("timeSent"));
                    long now = DateTime.Now.Ticks;
                    if (_averageLatency == 0)
                    {
                        _averageLatency = now - timeSent;
                        _log.Info("Latency " + _averageLatency);
                    }
                    else
                    {
                        _log.Info("Individual latency: " + (now-timeSent));
                        _averageLatency = (_averageLatency + (now - timeSent))/2;
                        _log.Info("Average latency now: " + _averageLatency*1.0/TimeSpan.TicksPerMillisecond);
                    }
                }
            }
            catch (XMSException e)
            {
                _log.Error("Error getting latency data: " + e, e);
            }
            _actualMessageCount++;
            if (_actualMessageCount%1000 == 0)
            {
                _log.Info("Received message count: " + _actualMessageCount);
            }            

            if (_actualMessageCount == _expectedMessageCount)
            {
                long timeTaken = (DateTime.Now.Ticks - _startTime)/TimeSpan.TicksPerMillisecond;                
                _log.Info("Total time taken to receive " + _expectedMessageCount+ " messages was " +
                                   timeTaken*1.0/TimeSpan.TicksPerMillisecond + "ms, equivalent to " +
                                   (_expectedMessageCount*1.0/timeTaken*1000) + " messages per second");
                try
                {
                    _connection.Close();
                    _log.Info("Connection closed");
                }
                catch (XMSException e)
                {
                    _log.Error("Error closing connection: " + e, e);
                }
            }
        }
        
        public static void Main(String[] args)
        {
            ServiceRequestingClient c = new ServiceRequestingClient();
            c.Init();
            c.SendMessages();
        }
    }
}
