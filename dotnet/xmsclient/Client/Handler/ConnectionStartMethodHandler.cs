using System;
using System.Text;
using log4net;
using OpenAMQ.Framing;
using OpenAMQ.XMS.Client.Protocol;
using OpenAMQ.XMS.Client.State;

namespace OpenAMQ.XMS.Client.Handler
{
    public class ConnectionStartMethodHandler : IStateAwareMethodListener
    {

        private static readonly ILog _log = LogManager.GetLogger(typeof(ConnectionStartMethodHandler));
        
        private static readonly ConnectionStartMethodHandler _instance = new ConnectionStartMethodHandler();
        
        public static ConnectionStartMethodHandler GetInstance()
        {
            return _instance;
        }

        private ConnectionStartMethodHandler()
        {       
        }
        
        public void MethodReceived(AMQStateManager stateManager, AMQMethodEvent evt)
        {
            ConnectionStartBody body = (ConnectionStartBody) evt.Method;
            
            try
            {
                if (body.Mechanisms == null)
                {
                    throw new AMQException("mechanism not specified in ConnectionStart method frame");
                }
                string allMechanisms = Encoding.ASCII.GetString(body.Mechanisms);
                string[] mechanisms = allMechanisms.Split(' ');
                string selectedMechanism = null;
                foreach (string mechanism in mechanisms)
                {
                    if (mechanism.Equals("PLAIN"))
                    {
                        selectedMechanism = mechanism;
                        break;
                    }
                }
        
                if (selectedMechanism == null)
                {
                    throw new AMQException("No supported security mechanism found, passed: " + mechanisms);
                }
                
                FieldTable response = new FieldTable();
                AMQProtocolSession ps = evt.ProtocolSession;
                response["login"] = ps.Username;
                response["password"] = ps.Password;

                if (body.Locales == null)
                {
                    throw new AMQException("Locales is not defined in Connection Start method");
                }
                string allLocales = Encoding.ASCII.GetString(body.Locales);
                string[] locales = allLocales.Split(new char[] { ' ' });
                string selectedLocale;
                if (locales != null && locales.Length > 0)
                {
                    selectedLocale = locales[0];
                }
                else
                {
                    throw new AMQException("No locales sent from server, passed: " + locales);
                }

                byte[] encodedResponse = response.GetDataAsBytes();
                stateManager.ChangeState(AMQState.CONNECTION_NOT_TUNED);
                FieldTable clientProperties = new FieldTable();
                clientProperties["product"] = "OpenAMQ JMS";
                clientProperties["version"] = "1.0";
                clientProperties["platform"] = GetFullSystemInfo();
                clientProperties["copyright"] = "(C) JP Morgan Chase 2006";
                ps.WriteFrame(ConnectionStartOkBody.CreateAMQFrame(evt.ChannelId, clientProperties, selectedMechanism,
                                                                   encodedResponse, selectedLocale));
            }
            catch (Exception e)
            {
                throw new AMQException(_log, "Unable to decode data: " + e, e);
            }
        }

        private string GetFullSystemInfo()
        {
            /*StringBuffer fullSystemInfo = new StringBuffer();
            fullSystemInfo.append(System.getProperty("java.runtime.name"));
            fullSystemInfo.append(", " + System.getProperty("java.runtime.version"));
            fullSystemInfo.append(", " + System.getProperty("java.vendor"));
            fullSystemInfo.append(", " + System.getProperty("os.arch"));
            fullSystemInfo.append(", " + System.getProperty("os.name"));
            fullSystemInfo.append(", " + System.getProperty("os.version"));
            fullSystemInfo.append(", " + System.getProperty("sun.os.patch.level"));*/
            // TODO: add in details here
            return ".NET 1.1 Client";
        }
    }
}
