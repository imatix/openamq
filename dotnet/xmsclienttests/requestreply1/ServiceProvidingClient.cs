using System;
using System.Threading;
using IBM.XMS;
using log4net;
using log4net.Config;
using NUnit.Framework;
using OpenAMQ.XMS.Client;

namespace JPMorgan.XMS.RequestReply1
{
    [TestFixture]
    public class ServiceProvidingClient : BaseMessagingTestFixture
    {
        private static ILog _logger = LogManager.GetLogger(typeof(ServiceProvidingClient));

        private int _messageCount;

        private IDestination _responseDest;

        private IMessageProducer _destinationProducer;
        
        private string _serviceName = "ServiceQ1";
        
        private string _selector = null;
        
        [SetUp]
        public override void Init()
        {
            base.Init();
            BasicConfigurator.Configure();
            _logger.Info("Starting...");
                    
            _logger.Info("Service (queue) name is '" + _serviceName + "'...");

            AMQQueue destination = new AMQQueue(_serviceName);
        
            _logger.Info("Message selector is <" + _selector + ">...");

            IMessageConsumer consumer = _session.CreateConsumer(destination,
                                                                100, true, false, _selector);

            consumer.MessageListener = new MessageListener(OnMessage);
        }
        
        [Test]
        public void Test()
        {
            _connection.Start();
            _logger.Info("Waiting...");
            AutoResetEvent evt = new AutoResetEvent(false);
            evt.WaitOne();
            
        }
                    
        public void OnMessage(IMessage message)
        {
            //_logger.info("Got message '" + message + "'");

            ITextMessage tm = (ITextMessage)message;

            try
            {
                IDestination responseDest = tm.JMSReplyTo;
                if (!responseDest.Equals(_responseDest))
                {
                    _responseDest = responseDest;                    
                    _logger.Debug("About to create a producer");
                    _destinationProducer = _session.CreateProducer(responseDest);
                    _destinationProducer.DisableMessageTimestamp = true;
                    _destinationProducer.DeliveryMode = DeliveryMode.NonPersistent;
                    _logger.Debug("After create a producer");                                
                }
            }
            catch (XMSException e)
            {
                _logger.Error("Error creating destination", e);
                throw e;
            }
            _messageCount++;
            if (_messageCount % 1000 == 0)
            {
                _logger.Info("Received message total: " + _messageCount);
                _logger.Info("Sending response to '" + _responseDest + "'");
            }

            try
            {
                String payload = "This is a response: sing together: 'Mahnah mahnah...'" + tm.Text;
                ITextMessage msg = _session.CreateTextMessage(payload);
                if (tm.PropertyExists("timeSent"))
                {
                    _logger.Info("timeSent property set on message");
                    _logger.Info("timeSent value is: " + tm.GetStringProperty("timeSent"));
                    msg.SetStringProperty("timeSent", tm.GetStringProperty("timeSent"));
                }
                _destinationProducer.Send(msg);
                if (_messageCount % 1000 == 0)
                {
                    _logger.Info("Sent response to '" + _responseDest + "'");
                }
            }
            catch (XMSException e)
            {
                _logger.Error("Error sending message: " + e, e);
                throw e;
            }
        }               
    }       
}
