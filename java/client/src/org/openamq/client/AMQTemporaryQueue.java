/**
 * AMQTemporaryQueue
 * Copyright (C) 2001-2005, J.P. Morgan Chase & Co. Incorporated.
 * all Rights Reserved.
 * The information contained herein is proprietary and confidential, and may 
 * not be duplicated or redistributed in any form without express written
 * consent of J.P. MorganChase & Co. Incorporated.
 * 
 * Created on Jan 13, 2006
 */
package org.openamq.client;

import javax.jms.JMSException;
import javax.jms.TemporaryQueue;

/**
 * AMQ implementation of a TemporaryQueue.
 */
final class AMQTemporaryQueue extends AMQQueue implements TemporaryQueue {

    /**
     * Create a new instance of an AMQTemporaryQueue
     */
    public AMQTemporaryQueue() {
        super("TempQueue" + Long.toString(System.currentTimeMillis()),
                null, true, true);
    }

    /**
     * @see javax.jms.TemporaryQueue#delete()
     */
    public void delete() throws JMSException {
        throw new UnsupportedOperationException("Delete not supported, " +
            "will auto-delete when connection closed");
    }
    
}
