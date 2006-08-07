/*---------------------------------------------------------------------------
    AMQException.java

    Copyright (c) 2005-2006 iMatix Corporation
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.
    
    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
    
    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 *---------------------------------------------------------------------------*/

package org.openamq;

/**
 * AMQP Exception.
 */
public class AMQException extends Exception
{
    /**
     * Create a new AMQException with the given message.
     *
     * @param message exception description.
     */
    public AMQException(String message)
    {
        super(message);
    }

    /**
     * Create a new AMQException with the given message and cause.
     *
     * @param message exception description.
     * @param t underlying exception cause.
     */
    public AMQException(String msg, Throwable t)
    {
        super(msg, t);
    }
}
