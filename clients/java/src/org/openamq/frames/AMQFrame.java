/**
 *  AMQFrame.java - Regular frames super class.
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
import org.openamq.AMQException;

public abstract class AMQFrame extends AMQFramable {
    short
        type = -1;                           /*                                   */

    /**
     *
     * Default constructor
     *
     **/
    public AMQFrame() {
        type = -1;
    }
    
    /**
     *
     * Decoder constructor
     *
     **/
    public AMQFrame(DataInputStream dis) throws AMQException {
        decode(dis);
    }
    
}

