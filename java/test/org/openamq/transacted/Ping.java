package org.openamq.transacted;

import org.openamq.client.AMQConnection;
import org.openamq.AMQException;
import org.openamq.client.AMQQueue;

import javax.jms.Connection;
import javax.jms.JMSException;
import java.util.Arrays;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class Ping
{
    public static void main(String[] argv) throws Exception
    {
        Config config = new Config(argv);
        Connection con = config.createConnection();
        con.setClientID("ping");
        new Relay(new AMQQueue("ping"), new AMQQueue("pong"), con,
                  config.isEchoOn(),
                  config.getBatchSize(),
                  config.usePersistentMessages()).start();
    }
}
