/*---------------------------------------------------------------------------
    AMQConstant.java

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

import java.net.*;

import org.openamq.framing.*;
import org.openamq.transport.*;
import org.openamq.protocol.*;

/**
 * AMQP implementation constants.
 */
public class AMQConstant {
    public static final int
        /**
         * Default port.
         */
        DEFAULT_PORT = 5672,
        /**
         * Heartbeat latency tolerance in seconds.
         */
        LATENCY = 5,

        REPLY_SUCCESS = 200,
        CONTENT_TOO_LARGE = 311,
        INVALID_PATH = 402,
        ACCESS_REFUSED = 403,
        NOT_FOUND = 404,
        RESOURCE_LOCKED = 405,
        FRAME_ERROR = 501,
        SYNTAX_ERROR = 502,
        COMMAND_INVALID = 503,
        CHANNEL_ERROR = 504,
        RESOURCE_ERROR = 506,
        NOT_ALLOWED = 530,
        NOT_IMPLEMENTED = 540,
        INTERNAL_ERROR = 541;

    public static final String
        /**
         * Plain authentication mechanism.
         */
        PLAIN_AUTH_MECHANISM = "PLAIN",

        /**
         * Default host and port.
         */
        DEFAULT_HOST = "localhost : " + DEFAULT_PORT,
        /**
         * Default virtual host.
         */
        DEFAULT_VHOST = "/";
}