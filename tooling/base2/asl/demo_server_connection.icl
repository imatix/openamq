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
    name      = "demo_server_connection"
    comment   = "Demo server connection class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the ASL connection class for the demo server.
</doc>

<inherit class = "asl_server_connection" />
<option name = "basename" value = "demo_server" />

<method name = "start ok" template = "function">
    if (s_auth_plain (self, method))
        self->authorised = TRUE;
    else
        self_raise_exception (self, ASL_ACCESS_REFUSED, 
            "Invalid authentication data",
            DEMO_SERVER_CONNECTION, DEMO_SERVER_CONNECTION_START_OK);
</method>

<private name = "header">
static int
    s_auth_plain ($(selftype) *self, demo_server_connection_start_ok_t *method);
static uint
    s_collect_plain_token (byte *data, char *token, uint token_end);
</private>

<private name = "body">
//  Sets and returns connection group, zero indicating an error

static int s_auth_plain (
    $(selftype) *self,
    demo_server_connection_start_ok_t *method)
{
    icl_shortstr_t
        login,
        password;
    uint
        token_null;

    //  method->response holds PLAIN data in format "[NULL]login[NULL]password"
    token_null = s_collect_plain_token (
        method->response->data, password, method->response->cur_size);
    if (token_null) 
        s_collect_plain_token (method->response->data, login, token_null);

    if (strnull (login) || strnull (password)) {
        self_raise_exception (self, ASL_SYNTAX_ERROR, 
            "Missing authentication data",
            DEMO_SERVER_CONNECTION, DEMO_SERVER_CONNECTION_START_OK);
        return (0);
    }
    if (streq (login, "guest") && streq (password, "guest"))
        return (TRUE);
    else
        return (FALSE);
}

//  Collects a token, returns offset of null octet at start
static uint
s_collect_plain_token (byte *data, char *token, uint token_end)
{
    uint
        token_null,
        token_size;

    //  Find start of token, scanning back from known end
    token_null = --token_end;
    while (token_null && data [token_null])
        token_null--;
        
    strclr (token);
    //  Token start must point to a null octet before the string
    token_size = token_end - token_null;
    if (token_size > ICL_SHORTSTR_MAX)
        smt_log_print (demo_broker->alert_log,
            "W: client used over-long authentication value - rejected");
    else {
        memcpy (token, data + token_null + 1, token_size);
        token [token_size] = 0;
    }
    return (token_null);
}
</private>

</class>
