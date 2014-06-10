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
    name    = "http_access_refresh"
    comment = "Refresh authentication via shell"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    Password authentication refresh module.  If this module is loaded, it
    will attempt to recover from certain authentication failures by calling
    a shell command to update the password file(s).  The shell command can
    do whatever is needed - e.g. query a credentials database - to get new
    credentials for the specified user.

    The module issues a system command with this syntax:

        command-name mechanism username realm

    E.g.

        http_refresh Digest mufasa testrealm@host.com

    The module does not wait for a response.  The shell command should update
    the web server password file appropriately; Xitami will reload that data
    and use it.  The user may get a message asking them to retry later.
</doc>

<inherit class = "http_access_module_back" />

<import class = "http" />

<method name = "announce">
    smt_log_print (log, "I: X5 password refresh module loaded");
</method>

<method name = "before">
    <doc>
    Check if authentication failed, and if so, send shell command.
    </doc>
    <local>
    char
        *refresh_by = http_config_refresh_by (http_config);
    icl_shortstr_t
        refresh_cmd;
    </local>
    //
    if (strnull (refresh_by))
        ;   //  Do nothing
    else
    if (context->process) {
        //  If there is already a refresh process running, check whether it's
        //  completed successfully or not, and kill it if we think it's blocked.
        if (ipr_process_wait (context->process, FALSE)) {
            //  Allow ten seconds for the process to finish, else kill the
            //  process and signal an internal server error
            if (++context->retries < 5)
                rc = HTTP_ACCESS_RETRY;
            else {
                smt_log_print (context->alert_log,
                    "E: killing unresponsive '%s'", refresh_by);
                ipr_process_destroy (&context->process);
                http_response_set_error (context->response,
                    HTTP_REPLY_INTERNALERROR, "Authentication system did not respond");
            }
        }
        else {
            //  Process finished, so check its exit code
            if (ipr_process_exitcode (context->process)) {
                smt_log_print (context->alert_log,
                    "E: non-zero exit value for '%s' (%d)", refresh_by, ipr_process_exitcode (context->process));
                http_response_set_error (context->response,
                    HTTP_REPLY_INTERNALERROR, "Authentication system failed");
            }
            ipr_process_destroy (&context->process);
        }
    }
    else
    if (context->fail_code == IPR_PASSWORD_NOTFOUND
    ||  context->fail_code == IPR_PASSWORD_EXPIRED) {
        //  Launch a new refresh process
        if (context->digest)
            icl_shortstr_fmt (refresh_cmd, "%s Digest %s %s %s",
                refresh_by, context->fail_user, context->fail_realm, http_config_digest_auth (http_config));
        else
            icl_shortstr_fmt (refresh_cmd, "%s Basic %s %s %s",
                refresh_by, context->fail_user, context->fail_realm, http_config_basic_auth (http_config));

        smt_log_print (context->alert_log, "I: running password refresh '%s'", refresh_cmd);
        context->process = ipr_process_new (
            refresh_cmd,
            NULL,
            http_config_refresh_log (http_config),
            http_config_refresh_log (http_config));
        ipr_process_start (context->process, ".");
        rc = HTTP_ACCESS_RETRY;
    }
</method>

</class>
