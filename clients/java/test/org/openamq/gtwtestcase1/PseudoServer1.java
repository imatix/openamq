package org.openamq.gtwtestcase1;

//import org.openamq.rawclient.Connection;
//import org.openamq.rawclient.ConnectionFactory;
//import org.openamq.rawclient.QueueWriter;
//import org.openamq.rawclient.Message;
//import org.openamq.rawclient.ClientConfig;
import org.openamq.rawclient.*;

import org.apache.log4j.*;

/**
 * @author Mark Atwell (mark.atwell@jpmorgan.com)
 */
public class PseudoServer1
{
	private static final Logger logger = Logger.getLogger(PseudoServer1.class);

    public static void main(String[] args)
    {
        String host = ClientConfig.getDefaultInstance().getHost();
        int port = ClientConfig.getDefaultInstance().getPort();
	  String user = ClientConfig.getDefaultInstance().getUser();
	  String password = ClientConfig.getDefaultInstance().getPassword();

        try
        {
            Connection con = ConnectionFactory.createConnection(host, port, user, password, 0);

		for(int i = 0; i < args.length; i++)
		{
			String queueName = args[i];

			logger.info("Trying to add queue '" + queueName + "'...");

			// Due to a bug I can't name temporary queues. So mine will have to be
			// non-remp for the time being (hence false).
			QueueReader qr = con.createQueueReader(queueName,false,true,new MessageReceivedCallback()
      	      {
            	    public void onMessage(Message m) throws AMQClientException
                	    {
                  	  System.out.println("Message received: " + String.valueOf(m));
                      }
	            });
		}

//            QueueWriter qw = con.createQueueWriter("test", false, 0);
 //           Message msg = new Message("pies");
 //           qw.write(msg);

            logger.info("Completed init.");

            con.readMessage();


            logger.info("Exiting...");

            System.exit(1);
        }
        catch (Throwable t)
        {
            System.err.println("Fatal error: " + t);
            t.printStackTrace();
        }

    }
}
