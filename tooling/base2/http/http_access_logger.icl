<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

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
 -->
<class
    name    = "http_access_logger"
    comment = "HTTP access logger module"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    Logs the result of access control.
</doc>

<inherit class = "http_access_module_back" />

<import class = "http" />

<method name = "announce">
    smt_log_print (log, "I: X5 access logging module loaded");
</method>

<method name = "after">
    <doc>
    Logs the HTTP transaction in the configured log format.
    </doc>
    <local>
    char
        *format;
    icl_shortstr_t
        time_str;
    </local>
    //
    format = http_config_access_log_format (http_config);
    if (streq (format, "CLF"))
        smt_log_print (context->access_log,
            "%s -%s [%s] \\"%s\\" %d %ld",
            context->request->address,
            context->auth_user? context->auth_user: "-",
            ipr_http_time_str (time_str),
            context->request->request_line,
            context->response->reply_code,
            context->response->content_length);
    else
    if (streq (format, "CLFX"))
        smt_log_print (context->access_log,
            "%s - %s [%s] \\"%s\\" %d %ld \\"%s\\" \\"%s\\"",
            context->request->address,
            context->auth_user? context->auth_user: "-",
            ipr_http_time_str (time_str),
            context->request->request_line,
            context->response->reply_code,
            context->response->content_length,
            http_request_get_header (context->request, "referer"),
            http_request_get_header (context->request, "user-agent"));
    else {
        char
            *formatted = http_access_context_format (context, format);
        smt_log_print (context->access_log, "%s", formatted);
        icl_mem_free (formatted);
    }
    switch (context->fail_code) {
        case IPR_PASSWORD_OK:
        case IPR_PASSWORD_CHALLENGE:
        case IPR_PASSWORD_TRYAGAIN:
            break;                      //  Ignore, not worth logging
        case IPR_PASSWORD_NOTFOUND:
            smt_log_print (context->alert_log,
                "W: (%s) authentication failed: inexistent user '%s'",
                context->request->address, context->fail_user);
            break;
        case IPR_PASSWORD_EXPIRED:
            smt_log_print (context->alert_log,
                "W: (%s) authentication failed: user '%s' password expired",
                context->request->address, context->fail_user);
            break;
        case IPR_PASSWORD_INCORRECT:
            smt_log_print (context->alert_log,
                "W: (%s) authentication failed: incorrect password for user '%s'",
                context->request->address, context->fail_user);
            break;
        case IPR_PASSWORD_BADTYPE:
            smt_log_print (context->alert_log,
                "W: (%s) authentication failed: wrong type of authentication",
                context->request->address);
            break;
        case IPR_PASSWORD_SYNTAX:
            smt_log_print (context->alert_log,
                "W: (%s) authentication failed: syntax error in credentials",
                context->request->address);
            break;
        case IPR_PASSWORD_HOSTILE:
            smt_log_print (context->alert_log,
                "W: (%s) authentication failed: possible hostile action",
                context->request->address);
            break;
    }
</method>

</class>
