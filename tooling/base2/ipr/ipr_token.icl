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
    name      = "ipr_token"
    comment   = "String tokenisation class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
Provides string tokenisation functions.  Use this to split a string
into a list of words (delimited by spaces) so that each word can be
manipulated separately.
</doc>

<option name = "links" value = "1"/>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_list_item" />

<import class = "icl_shortstr" />
<import class = "ipr_str" />

<context>
    icl_shortstr_t
        value;                          //  Item value
</context>

<method name = "split" return = "tokens">
    <doc>
    Splits a string into a list of tokens delimited by whitespace.  Returns a new
    ipr_token_list item, which the caller must destroy.
    </doc>
    <argument name = "string" type = "char *">String to parse</argument>
    <declare name = "tokens" type = "ipr_token_list_t *">Returned list</declare>
    <local>
    char
        *word_start,
        *word_end;
    ipr_token_t
        *token;
    </local>
    //
    tokens = ipr_token_list_new ();
    word_start = string;
    while (*word_start) {
        while (isspace (*word_start))
            word_start++;
        word_end = word_start;
        while (*word_end && !isspace (*word_end))
            word_end++;

        if (word_end > word_start) {
            token = ipr_token_new ();
            icl_shortstr_ncpy (token->value, word_start, word_end - word_start);
            ipr_token_list_queue (tokens, token);
            ipr_token_unlink (&token);
        }
        word_start = word_end;
    }
</method>

<method name = "split rich" return = "tokens">
    <doc>
    Splits a string into a list of tokens delimited by a set of user-specified
    delimiters.  Returns a new ipr_token_list item, which the caller must destroy.
    </doc>
    <argument name = "string" type = "char *">String to parse</argument>
    <argument name = "delims" type = "char *">Delimiters</argument>
    <declare name = "tokens" type = "ipr_token_list_t *">Returned list</declare>
    <local>
    char
        *word_start,
        *word_end;
    ipr_token_t
        *token;
    </local>
    //
    tokens = ipr_token_list_new ();
    word_start = string;
    while (*word_start) {
        while (strchr (delims, *word_start))
            word_start++;
        word_end = word_start;
        while (*word_end && !strchr (delims, *word_end))
            word_end++;

        if (word_end > word_start) {
            token = ipr_token_new ();
            icl_shortstr_ncpy (token->value, word_start, word_end - word_start);
            ipr_token_list_queue (tokens, token);
            ipr_token_unlink (&token);
        }
        word_start = word_end;
    }
</method>

<method name = "selftest">
    <local>
    ipr_token_list_t
        *tokens;
    </local>
    //
    tokens = ipr_token_split ("this is a word");
    assert (ipr_token_list_count (tokens) == 4);
    ipr_token_list_destroy (&tokens);

    tokens = ipr_token_split ("  this  is  a  word  ");
    assert (ipr_token_list_count (tokens) == 4);
    ipr_token_list_destroy (&tokens);

    tokens = ipr_token_split ("  ");
    assert (ipr_token_list_count (tokens) == 0);
    ipr_token_list_destroy (&tokens);

    tokens = ipr_token_split ("  this-is-a-word  ");
    assert (ipr_token_list_count (tokens) == 1);
    ipr_token_list_destroy (&tokens);

    tokens = ipr_token_split ("this-is-a-word");
    assert (ipr_token_list_count (tokens) == 1);
    ipr_token_list_destroy (&tokens);

    tokens = ipr_token_split_rich ("this is a word", " ");
    assert (ipr_token_list_count (tokens) == 4);
    ipr_token_list_destroy (&tokens);

    tokens = ipr_token_split_rich ("this  is;a  word", ";-");
    assert (ipr_token_list_count (tokens) == 2);
    ipr_token_list_destroy (&tokens);
</method>

</class>
