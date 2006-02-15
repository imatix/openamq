using System;
using IBM.XMS;
using log4net;
using OpenAMQ.XMS.Client;

namespace JPMorgan.XMS.RequestReply1
{
    public class ServiceProvidingClient
    {
        private static ILog _logger = LogManager.GetLogger(typeof(ServiceProvidingClient));

        private static int _messageCount;

        private static IDestination _responseDest;

        private static IMessageProducer _destinationProducer;

        private static OpenAMQ.XMS.ISession _session;
        
        static void Main(string[] args)
        {
            _logger.Info("Starting...");

            if (args.Length < 5)
            {
                Console.Out.WriteLine("Usage: brokerDetails username password virtual-path serviceQueue [selector]");
                return;
            }        
            try
            {
                //IPEndPoint address = IPAddress.Loopback.
                int port = Int32.Parse(args[1]);
                AMQConnection con = new AMQConnection(args[0], port, args[2], args[3], "laptop", args[4]);                
                _session = (OpenAMQ.XMS.ISession) con.CreateSession(false, AcknowledgeMode.AutoAcknowledge);

                String serviceName = args[5];

                _logger.Info("Service (queue) name is '" + serviceName + "'...");

                AMQQueue destination = new AMQQueue(serviceName);

                String selector = (args.Length >= 7 && args[6].Length > 1) ? args[6] : null;

                _logger.Info("Message selector is <" + selector + ">...");

                IMessageConsumer consumer = _session.CreateConsumer(destination,
                                                                    100, true, false, selector);

                consumer.MessageListener = OnMessage;
                con.Start();
                Console.Out.WriteLine("Waiting...");
            }
            catch (Exception e)
            {
                Console.Error.WriteLine("Fatal error: " + e);
                Console.Error.WriteLine(e.StackTrace);
            }
        }

        public static void OnMessage(IMessage message)
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
                    _logger.Info("timeSent value is: " + tm.GetLongProperty("timeSent"));
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
            }
        }               
    }       
}
