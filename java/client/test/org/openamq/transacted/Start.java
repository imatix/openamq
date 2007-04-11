package org.openamq.transacted;

import org.openamq.client.AMQConnection;
import org.openamq.AMQException;
import org.openamq.client.AMQQueue;

import javax.jms.Connection;
import javax.jms.JMSException;
import javax.jms.Session;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
public class Start
{
    public static void main(String[] argv) throws Exception
    {
        Connection con = new Config(argv).createConnection();
        AMQQueue ping = new AMQQueue("ping");
        Session session = con.createSession(false, Session.AUTO_ACKNOWLEDGE);
        session.createProducer(ping).send(session.createTextMessage("start"));
        session.close();
        con.close();
    }
}
