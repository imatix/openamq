package org.openamq;

import java.net.*;

import org.openamq.framing.*;
import org.openamq.transport.*;
import org.openamq.protocol.*;

public class AMQConstant {
    public static final String
        PLAIN_AUTH_MECHANISM = "PLAIN";
    public static final int
        DEFAULT_PORT = 5672,
        DEFAULT_PREFETCH = 1,

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
}