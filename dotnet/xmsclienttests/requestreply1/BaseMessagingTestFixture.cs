using System;
using System.Net;
using IBM.XMS;
using jpmorgan.mina.common;
using log4net;
using log4net.Config;
using NUnit.Framework;
using OpenAMQ.XMS.Client;
using IMessageProducer=OpenAMQ.XMS.IMessageProducer;

namespace JPMorgan.XMS.RequestReply1
{   
    public class BaseMessagingTestFixture
    {
        private static ILog _logger = LogManager.GetLogger(typeof(BaseMessagingTestFixture));

        protected string _host = "localhost";

        protected int _port = 5672;

        protected string _username = "guest";

        protected string _password = "guest";

        protected string _vpath = "/test";

        protected AMQConnection _connection;

        protected OpenAMQ.XMS.ISession _session;
        
        [SetUp]
        public virtual void Init()
        {            
            try
            {
                string clientID = String.Format("{0}{1:G}", Dns.GetHostName(), DateTime.Now.Ticks);
                _connection = new AMQConnection(_host, _port, _username, _password, clientID, _vpath);
                _session = (OpenAMQ.XMS.ISession) _connection.CreateSession(false, AcknowledgeMode.AutoAcknowledge);                
           }
            catch (XMSException e)
            {
                _logger.Error("Error initialisng test fixture: " + e, e);
                throw e;
            }
        }
    }
}
