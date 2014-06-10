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
    name      = "http_access_context"
    comment   = "HTTP access context class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class holds properties for access control.  These properties are
    provided to and modified by the various access modules.  The object is
    for local use by access modules.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <!-- Passed to synchronous portal back-end -->
    <option name = "links" value = "0" />
</inherit>

<import class = "http" />

<context>
    //  This is the thread of the client connection
    smt_thread_t
        *thread;                        //  Client thread in http_agent.smt
    //  This is the IP address of the client (could be a proxy)
    char
        *address;                       //  Dotted numeric notation
    //  The request and response refer to the two main objects holding the parsed
    //  HTTP request, and prepared response data.
    http_request_t
        *request;                       //  HTTP request object
    http_response_t
        *response;                      //  HTTP response object
    //  These properties are set by the authentication code, copying from the
    //  relevant password table.
    char
        *fail_user,                     //  If authentication failed
        *fail_realm,                    //  If known
        *auth_user,                     //  If authentication succeeded
        *auth_group;                    //  User group, or "users"
    int
        fail_code;                      //  IPR_PASSWORD_xxx value
    //  This is true if we did a Digest authentication
    Bool
        digest_auth;                    //  Did we try Digest auth?
    //  Links to the two server password tables
    ipr_password_table_t
        *basic,                         //  Basic authentication data
        *digest;                        //  Digest authentication data
    //  Links to the server's log files
    smt_log_t
        *access_log,                    //  Access log file
        *alert_log,                     //  Alert log file
        *debug_log;                     //  Debug log file
    //  Links to the server's blacklist table
    ipr_dict_table_t
        *blacklist;                     //  Banned IP addresses
    //  Counts the number of retries
    size_t
        retries;                        //  How many retries so far
    //  If an access module needs to run an external process it can put the
    //  handle here, and then recheck that the process is not still running
    //  in a retry loop.
    ipr_process_t
        *process;                       //  External process handle
</context>

<method name = "new">
    <argument name = "server" type = "http_server_t *" />
    //
    self->basic  = ipr_password_table_link (server->basic);
    self->digest = ipr_password_table_link (server->digest);
    self->access_log = smt_log_link (server->access_log);
    self->alert_log  = smt_log_link (server->alert_log);
    self->debug_log  = smt_log_link (server->debug_log);
    self->blacklist  = ipr_dict_table_link (server->blacklist);
</method>

<method name = "destroy">
    smt_thread_unlink (&self->thread);
</method>

<method name = "free">
    icl_mem_strfree (&self->address);
    http_request_unlink  (&self->request);
    http_response_unlink (&self->response);
    ipr_password_table_unlink (&self->basic);
    ipr_password_table_unlink (&self->digest);
    smt_log_unlink  (&self->access_log);
    smt_log_unlink  (&self->alert_log);
    smt_log_unlink  (&self->debug_log);
    icl_mem_strfree (&self->fail_user);
    icl_mem_strfree (&self->fail_realm);
    icl_mem_strfree (&self->auth_user);
    icl_mem_strfree (&self->auth_group);
    ipr_process_destroy (&self->process);
    ipr_dict_table_unlink (&self->blacklist);
</method>

<method name = "set thread" template = "function">
    <argument name = "thread" type = "smt_thread_t *" />
    //
    smt_thread_unlink (&self->thread);
    self->thread = smt_thread_link (thread);
</method>

<method name = "set address" template = "function">
    <argument name = "address" type = "char *" />
    //
    icl_mem_strfree (&self->address);
    self->address = icl_mem_strdup (address);
</method>

<method name = "set request" template = "function">
    <argument name = "request" type = "http_request_t *" />
    //
    http_request_unlink (&self->request);
    self->request = http_request_link (request);
</method>

<method name = "set response" template = "function">
    <argument name = "response" type = "http_response_t *" />
    //
    http_response_unlink (&self->response);
    self->response = http_response_link (response);
</method>

<method name = "set auth" template = "function">
    <doc>
    Stores the user name and group after successful authentication.
    </doc>
    <argument name = "table" type = "ipr_password_table_t *" />
    <argument name = "group" type = "char *" />
    //
    icl_mem_strfree (&self->fail_user);
    icl_mem_strfree (&self->fail_realm);
    icl_mem_strfree (&self->auth_user);
    icl_mem_strfree (&self->auth_group);
    self->auth_user  = icl_mem_strdup (table->username);
    self->auth_group = icl_mem_strdup (group);
    self->fail_code  = table->result;
</method>

<method name = "set fail" template = "function">
    <doc>
    Stores the user name which tried but failed to authenticated, and its
    realm.
    </doc>
    <argument name = "table" type = "ipr_password_table_t *" />
    //
    icl_mem_strfree (&self->fail_user);
    icl_mem_strfree (&self->fail_realm);
    icl_mem_strfree (&self->auth_user);
    icl_mem_strfree (&self->auth_group);
    self->fail_user  = icl_mem_strdup (table->username);
    self->fail_realm = icl_mem_strdup (table->realm);
    self->fail_code  = table->result;
</method>

<method name = "format" return = "formatted">
    <doc>
    Formats a line of logging information, inserting logging symbols.  Returns
    freshly allocated line with formatted text. The template symbools are:
    * $agent - User-Agent header value
    * $arguments - requested URI arguments
    * $datetime - date/time in NCSA format
    * $day - day as two digits
    * $file - filename to which request was translated
    * $from - client address, as dotted number
    * $hh - hour, using 24-hour clock
    * $method - HTTP method
    * $mm - minutes as two digits
    * $mon - month as two digits
    * $path - requested URI path
    * $query - query string, if any
    * $recd - request size, in bytes
    * $referer - Referer header
    * $request - complete request line
    * $sent - response size, in bytes
    * $ss - seconds as two digits
    * $status - response code, 3 digits
    * $user - user name, if authenticated, else -
    * $year - year as four digits
    * $yy - year as two digits
    * $VAR - environment variable VAR
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <argument name = "format" type = "char *" />
    <declare name = "formatted" type = "char *" default = "NULL" />
    <local>
    static char
        *months = "Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec";
    ipr_dict_table_t
        *table;
    apr_time_t
        apr_time;
    apr_time_exp_t
        time_exp;
    icl_shortstr_t
        time_str;
    </local>
    //
    table = ipr_dict_table_new ();
    //  Import environment
    ipr_dict_table_import (table, environ, TRUE);

    apr_time = apr_time_now ();
    apr_time_exp_lt (&time_exp, apr_time);

    //  Define all variables
    ipr_dict_assume     (table, "agent",        http_request_get_header (self->request, "user-agent"));
    ipr_dict_assume     (table, "arguments",    self->request->request_arguments);
    ipr_dict_assume     (table, "datetime",     ipr_http_time_str (time_str));
    ipr_dict_assume_fmt (table, "day", "%02d",  time_exp.tm_mday);
    ipr_dict_assume     (table, "file",         self->response->filename? self->response->filename: "-");
    ipr_dict_assume     (table, "from",         self->request->address);
    ipr_dict_assume_fmt (table, "hh", "%02d",   time_exp.tm_hour);
    ipr_dict_assume     (table, "method",       self->request->request_method);
    ipr_dict_assume_fmt (table, "mm", "%02d",   time_exp.tm_mon + 1);
    ipr_dict_assume     (table, "mon",          months + time_exp.tm_mon * 4);
    ipr_dict_assume     (table, "path",         self->request->request_path);
    ipr_dict_assume_fmt (table, "recd", "%ld",  self->request->content_length);
    ipr_dict_assume     (table, "referer",      http_request_get_header (self->request, "referer"));
    ipr_dict_assume     (table, "request",      self->request->request_line);
    ipr_dict_assume_fmt (table, "sent", "%ld",  self->response->content_length);
    ipr_dict_assume_fmt (table, "ss", "%02d",   time_exp.tm_sec);
    ipr_dict_assume_fmt (table, "status", "%d", self->response->reply_code);
    ipr_dict_assume     (table, "user",         self->auth_user? self->auth_user: "-");
    ipr_dict_assume_fmt (table, "year", "%04d", time_exp.tm_year + 1900);
    ipr_dict_assume_fmt (table, "yy", "%02d",   time_exp.tm_year);

    formatted = ipr_dict_table_template (table, format);
    ipr_dict_table_destroy (&table);
</method>

<method name = "selftest" />

</class>
