package org.openamq.transacted;

import org.openamq.client.AMQSession;

import javax.jms.MessageProducer;
import javax.jms.MessageConsumer;
import javax.jms.Session;
import javax.jms.Destination;
import javax.jms.Connection;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.TextMessage;
import javax.jms.DeliveryMode;

/**
 * @author Gordon Sim (gordon.r.sim@jpmorgan.com)
 */
class Relay implements Runnable
{
    private final Connection _con;
    private final Session _session;
    private final MessageConsumer _src;
    private final MessageProducer _dest;
    private final int _batch;
    private final boolean _echo;
    private int _counter;
    private long start;
    private boolean _running;

    Relay(Destination src, Destination dest, Connection con) throws JMSException
    {
        this(src, dest, con, false, 100, true);
    }

    Relay(Destination src, Destination dest, Connection con, boolean echo, int batch, boolean persistent) throws JMSException
    {
        _echo = echo;
        _batch = batch;
        _con = con;
        _session = con.createSession(true, AMQSession.NO_ACKNOWLEDGE);
        _src = _session.createConsumer(src);
        _dest = _session.createProducer(dest);
        _dest.setDeliveryMode(persistent ? DeliveryMode.PERSISTENT : DeliveryMode.NON_PERSISTENT);

    }

    public void run()
    {
        start = System.currentTimeMillis();
        try{
            while(true) relay();
        }
        catch(JMSException e)
        {
            e.printStackTrace();
        }
        try
        {
            _session.close();
        }
        catch (JMSException e)
        {
            e.printStackTrace();
        }
    }

    void relay() throws JMSException
    {
        _dest.send(relay(_src.receive()));
        _session.commit();
    }

    Message relay(Message in) throws JMSException
    {
        if(!_running)
        {
            System.out.println(_con.getClientID() + " started.");
            _running = true;
        }
        if(++_counter % _batch == 0)
        {
            long time = System.currentTimeMillis() - start;
            System.out.println(_batch + " iterations performed in " + time + " ms");
            try
            {
                Thread.sleep(100);
            }
            catch (InterruptedException e)
            {
                e.printStackTrace();
            }
            start = System.currentTimeMillis();
        }
        if(_echo)
        {
            System.out.println("Received: " + ((TextMessage) in).getText());
        }
        return _session.createTextMessage(_con.getClientID() + _counter);
    }

    void start() throws InterruptedException, JMSException
    {
        Thread runner = new Thread(this);
        runner.start();
        _con.start();
        System.out.println(_con.getClientID() + " waiting...");
        runner.join();
        _con.close();
    }
}
