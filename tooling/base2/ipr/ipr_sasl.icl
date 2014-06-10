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
    name      = "ipr_sasl"
    comment   = "SASL functions"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Provides functions for working with SASL (simple authentication and
    security layer).
</doc>
<inherit class = "icl_base" />

<method name = "plain encode" return = "auth data">
    <doc>
    Formats a new authentication data block for a plain login.  The protocol
    uses a SASL-style binary data block for authentication.  This method is
    a simple way of turning a name and password into such a block.  Note
    that plain authentication data is not encrypted and does not provide
    any degree of confidentiality.  The SASL PLAIN mechanism is defined here:
    http://www.ietf.org/internet-drafts/draft-ietf-sasl-plain-08.txt.
    </doc>
    <argument name = "login"    type = "char *" pass = "in">User login name</argument>
    <argument name = "password" type = "char *" pass = "in">User password</argument>
    <declare name = "auth data" type = "icl_longstr_t *">Authentication data</declare>
    //
    //  PLAIN format is [null]login[null]password
    auth_data = icl_longstr_new (NULL, strlen (login) + strlen (password) + 3);
    auth_data->data [0] = 0;
    strcpy ((char *) auth_data->data + 1, login);
    strcpy ((char *) auth_data->data + 2 + strlen (login), password);

    //  Do NOT include final null in binary block size
    auth_data->cur_size = strlen (login) + strlen (password) + 2;
</method>

<method name = "plain decode" return = "rc">
    <doc>
    To be done.
    </doc>
    <argument name = "auth data" type = "icl_longstr_t *">Authentication data</argument>
    <argument name = "login"    type = "char *">User login name</argument>
    <argument name = "password" type = "char *">User password</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    //
</method>

<method name = "selftest">
    <local>
    icl_longstr_t
        *auth_data;
    </local>
    //
    auth_data = ipr_sasl_plain_encode ("myname", "mypassword");
    assert (auth_data);
    icl_longstr_destroy (&auth_data);
</method>

</class>
