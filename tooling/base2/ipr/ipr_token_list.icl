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
    name      = "ipr_token_list"
    comment   = "Implements the list container for ipr_token"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the list container for ipr_token
</doc>

<inherit class = "icl_list_head" >
    <option name = "prefix"    value = "list"/>
    <option name = "childname" value = "ipr_token" />
    <option name = "childtype" value = "ipr_token_t" />
    <option name = "rwlock"    value = "0" />
    <option name = "count"     value = "1" />
</inherit>

<import class = "ipr_token" />

<method name = "cat" template = "function">
    <doc>
    Concatenates one token list onto another.
    </doc>
    <argument name = "source" type = "$(selftype) *">List to copy from</argument>
    <local>
    ipr_token_t
        *token,
        *copy;
    </local>
    //
    token = ipr_token_list_first (source);
    while (token) {
        copy = ipr_token_new ();
        icl_shortstr_cpy (copy->value, token->value);
        ipr_token_list_queue (self, copy);
        ipr_token_unlink (&copy);
        token = ipr_token_list_next (&token);
    }
</method>

<method name = "merge" template = "function">
    <doc>
    Merges one token list into another, appending only tokens that
    are not already in the first list.  Note: this function will be
    slow for large lists, since it does sequential scans.
    </doc>
    <argument name = "source" type = "$(selftype) *">List to copy from</argument>
    <local>
    ipr_token_t
        *token,
        *copy;
    </local>
    //
    token = ipr_token_list_first (source);
    while (token) {
        copy = ipr_token_list_first (self);
        while (copy) {
            if (streq (token->value, copy->value))
                break;                  //  Token already exists in list
            copy = ipr_token_list_next (&copy);
        }
        //  Append only if the token was not already there
        if (!copy) {
            copy = ipr_token_new ();
            icl_shortstr_cpy (copy->value, token->value);
            ipr_token_list_queue (self, copy);
        }
        ipr_token_unlink (&copy);
        token = ipr_token_list_next (&token);
    }
</method>

<method name = "selftest">
    <local>
    </local>
</method>
</class>
