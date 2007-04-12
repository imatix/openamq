package org.openamq.client.protocol;

import org.apache.log4j.Logger;
import org.openamq.client.AMQConnection;

import javax.jms.JMSException;

public abstract class FailoverSupport
{
    private static final Logger _log = Logger.getLogger(FailoverSupport.class);

    public Object execute(AMQConnection con) throws JMSException
    {
        // we wait until we are not in the middle of failover before acquiring the mutex and then proceeding.
        // any method that can portentially block for any reasons should use this class so that deadlock will not
        // occur. The FailoverException is propagated by the AMQProtocolHandler to any listeners (e.g. frame listeners)
        // that might be causing a block. When that happens, the exception is caught here and the mutex is released
        // before waiting for the failover to complete (either successfully or unsuccessfully).
        while (true)
        {
            try
            {
                con.blockUntilNotFailingOver();
            }
            catch (InterruptedException e)
            {
                _log.info("Interrupted: " + e, e);
                return null;
            }
            synchronized (con.getFailoverMutex())
            {
                try
                {
                    return operation();
                }
                catch (FailoverException e)
                {
                    _log.info("Failover exception caught during operation");
                }
            }
        }
    }


    protected abstract Object operation() throws JMSException;
}
