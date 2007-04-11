package org.openamq.transacted;

import org.openamq.client.AMQConnection;
import org.openamq.AMQException;
import org.openamq.client.AMQQueue;

import javax.jms.Connection;
import javax.jms.JMSException;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class Pong
{
    public static void main(String[] argv) throws Exception
    {
        Config config = new Config(argv);
        Connection con = config.createConnection();
        con.setClientID("pong");
        new Relay(new AMQQueue("pong"), new AMQQueue("ping"), con,
                  config.isEchoOn(),
                  config.getBatchSize(),
                  config.usePersistentMessages()).start();

    }
}
