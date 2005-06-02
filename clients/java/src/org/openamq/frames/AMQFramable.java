/**
 *  AMQFramable.java - Super class for network-serializable objects.
 *
 *  Generated from amq_frames.xml by java/amq_frames.gsl using GSL/4.
 *  DO NOT MODIFY THIS FILE.
 *
 *  For documentation and updates see http://www.imatix.com.
 *  Copyright (c) 2004 JPMorgan
 *  Copyright (c) 1991-2004 iMatix Corporation
 *
 **/
 
package org.openamq.frames;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import org.openamq.AMQException;

public abstract class AMQFramable {

    /**
     *
     * Default constructor
     *
     **/
    public AMQFramable() {
    }
    
    /**
     *
     * Decoder constructor
     *
     **/
    public AMQFramable(DataInputStream dis) throws AMQException {
        decode(dis);
    }
    
    /**
     *
     * Encoder method
     *
     **/
    abstract public long encode(DataOutputStream os);
    
    /**
     *
     * Decoder method
     *
     **/
    abstract public void decode(DataInputStream dis) throws AMQException;
    
    /**
     *
     * Frame size method
     *
     **/
    abstract public long size();
    
    /**
     *
     * Frame dumping method
     *
     **/
    abstract public void dump();
}

