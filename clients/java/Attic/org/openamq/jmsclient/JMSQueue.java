/**
 *  JMSQueue.java - JMS Queue implementation using AMQP/Fast.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2005 JPMorgan
 *  Copyright (c) 2005 iMatix Corporation
 *
 **/

package com.imatix.openamq.jms;

import javax.jms.Queue;
import javax.jms.JMSException;


public class JMSQueue implements Queue { 
    String 
        destName = null;

    public JMSQueue(String destName) {
        this.destName = destName;
    }    
        
    public String getQueueName() throws JMSException {
        return destName;
    } 

    public String toString() {
        return destName;
    }
}
