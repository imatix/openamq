<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This file is licensed under the GPL as follows:

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
    name      = "zyre_uri"
    comment   = "Zyre URI parsing"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<doc>
Does validation and parsing of RestMS URIs for the Zyre server.  Accepts
a URI string, and validates it returning a URI type and a set of component
strings.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "zyre_classes" />

<context>
    icl_shortstr_t
        uri;                            //  URI before parsing
    int
        type;                           //  URI type, after parsing
    icl_shortstr_t
        feed_class,                     //  Parsed feed class
        feed_name,                      //  Parsed feed name
        pipe_class,                     //  Parsed pipe class
        pipe_name,                      //  Parsed pipe name
        address,                        //  Parsed address string
        nozzle;                         //  Parsed message nozzle
    size_t
        index;                          //  Parsed message index
</context>

<public name = "header">
//  These are the types of URI we recognize for processing
#define RESTMS_URI_INVALID             0
#define RESTMS_URI_ROOT                1
#define RESTMS_URI_PIPES               2
#define RESTMS_URI_PIPE                3
#define RESTMS_URI_FEEDS               4
#define RESTMS_URI_FEED                5
#define RESTMS_URI_ADDRESS             6
#define RESTMS_URI_JOIN                7
#define RESTMS_URI_MESSAGE             8
</public>

<method name = "new">
    <argument name = "uri" type = "char *">URI to parse</argument>
    <doc>
    Creates a new object and sets its URI as specified.  The URI may be null.
    </doc>
    //
    if (uri) {
        icl_shortstr_cpy (self->uri, uri);
        self_parse (self);
    }
</method>

<method name = "destroy">
</method>

<method name = "set" template = "function">
    <doc>
    Sets the URI to the specified string, and parses it.
    </doc>
    <argument name = "uri" type = "char *">URI to parse</argument>
    //
    assert (uri);
    icl_shortstr_cpy (self->uri, uri);
    rc = self_parse (self);
</method>

<private name = "header">
//  This regexp matches a join URI with optional feed class (5 items)
//                        ( pipe )/(class )/( addr )@( feed )   /(class )
#define JOIN_REGEXP    "^/([^/@]+)/([^/@]+)/([^/@]+)@([^/@]+)(?:/([^/@]+))?$"

//  This regexp matches an address URI with optional feed class (3 items)
//                        ( addr )@( feed )   /(class )
#define ADDRESS_REGEXP "^/([^/@]+)@([^/@]+)(?:/([^/@]+))?$"

//  This regexp matches a message URI with nozzle and index (4 items)
//                        ( pipe )/(class )/   (nozzle)   /(index)
#define MESSAGE_REGEXP "^/([^/@]+)/([^/@]+)/(?:([^/@]+)(?:/([0-9]+))?)?$"

//  This regexp matches a class name and an option item name
//                        (class )   /( item )
#define ENTITY_REGEXP  "^/([^/@]+)(?:/([^/@]+))?$"
</private>

<method name = "parse" template = "function">
    <doc>
    Parses the URI into its components, and returns the URI type, if it
    could be parsed.
    root        empty or /
    feeds       /{feed-class}
    feed        /{feed-class}/{feed-name}
    pipes       /{pipe-class}
    pipe        /{pipe-class}/{pipe-name}
    address     /{address-string}@{feed-name}[/{feed-class}]
    join        /{pipe-class}/{pipe-name}/{address-string}@{feed-name}[/{feed-class}]
    message     /{pipe-class}/{pipe-name}/[{nozzle}[/{index}]]
    </doc>
    <local>
    ipr_regexp_t
        *regexp = NULL;                 //  The compiled regexp
    char
        *part1 = NULL,                  //  We have up to 4 path segments
        *part2 = NULL,
        *part3 = NULL,
        *part4 = NULL,
        *part5 = NULL;
    </local>
    //
    rc = RESTMS_URI_INVALID;            //  Guilty until proven innocent
    strclr (self->feed_class);
    strclr (self->feed_name);
    strclr (self->pipe_class);
    strclr (self->pipe_name);
    strclr (self->address);
    strclr (self->nozzle);

    if (streq (self->uri, "") || streq (self->uri, "/"))
        rc = RESTMS_URI_ROOT;

    if (rc == RESTMS_URI_INVALID) {
        //  Try address URI with optional feed class
        //  /{address-string}@{feed-name}[/{feed-class}]
        regexp = ipr_regexp_new (ADDRESS_REGEXP);
        if (ipr_regexp_match (regexp, self->uri, &part1, &part2, &part3) == 3) {
            if (strnull (part3) || zyre_uri_is_feed_class (part3)) {
                rc = RESTMS_URI_ADDRESS;
                icl_shortstr_cpy (self->address,    part1);
                icl_shortstr_cpy (self->feed_name,  part2);
                icl_shortstr_cpy (self->feed_class, part3);
            }
        }
        ipr_regexp_destroy (&regexp);
    }
    if (rc == RESTMS_URI_INVALID) {
        //  Try join URI with optional feed class
        //  /{pipe-class}/{pipe-name}/{address-string}@{feed-name}[/{feed-class}]
        regexp = ipr_regexp_new (JOIN_REGEXP);
        if (ipr_regexp_match (regexp, self->uri, &part1, &part2, &part3, &part4, &part5) == 5) {
            if (zyre_uri_is_pipe_class (part1)
            && (strnull (part5) || zyre_uri_is_feed_class (part5))) {
                rc = RESTMS_URI_JOIN;
                icl_shortstr_cpy (self->pipe_class, part1);
                icl_shortstr_cpy (self->pipe_name,  part2);
                icl_shortstr_cpy (self->address,    part3);
                icl_shortstr_cpy (self->feed_name,  part4);
                icl_shortstr_cpy (self->feed_class, part5);
            }
        }
        ipr_regexp_destroy (&regexp);
    }
    if (rc == RESTMS_URI_INVALID) {
        //  Try message URI with optional nozzle and index
        //  /{pipe-class}/{pipe-name}/[{nozzle}[/{index}]]
        regexp = ipr_regexp_new (MESSAGE_REGEXP);
        if (ipr_regexp_match (regexp, self->uri, &part1, &part2, &part3, &part4) == 4) {
            if (zyre_uri_is_pipe_class (part1)) {
                rc = RESTMS_URI_MESSAGE;
                icl_shortstr_cpy (self->pipe_class, part1);
                icl_shortstr_cpy (self->pipe_name,  part2);
                icl_shortstr_cpy (self->nozzle,     part3);
                self->index = atoi (part4);
            }
        }
        ipr_regexp_destroy (&regexp);
    }
    if (rc == RESTMS_URI_INVALID) {
        //  Try remaining URIs, class name and optional item name
        //  /{class-name}[/{item-name}]
        regexp = ipr_regexp_new (ENTITY_REGEXP);
        if (ipr_regexp_match (regexp, self->uri, &part1, &part2) == 2) {
            if (zyre_uri_is_pipe_class (part1)) {
                icl_shortstr_cpy (self->pipe_class, part1);
                if (strused (part2)) {
                    rc = RESTMS_URI_PIPE;
                    icl_shortstr_cpy (self->pipe_name, part2);
                }
                else
                    rc = RESTMS_URI_PIPES;
            }
            else
            if (zyre_uri_is_feed_class (part1)) {
                icl_shortstr_cpy (self->feed_class, part1);
                if (strused (part2)) {
                    rc = RESTMS_URI_FEED;
                    icl_shortstr_cpy (self->feed_name, part2);
                }
                else
                    rc = RESTMS_URI_FEEDS;
            }
        }
        ipr_regexp_destroy (&regexp);
    }
    icl_mem_free (part1);
    icl_mem_free (part2);
    icl_mem_free (part3);
    icl_mem_free (part4);
    icl_mem_free (part5);
    self->type = rc;
</method>

<method name = "is pipe class" return = "rc">
    <doc>
    Returns TRUE if the supplied string is a pipe class name.
    </doc>
    <argument name = "string" type = "char *">String to check</argument>
    <declare name = "rc" type = "int" default = "0" />
    //
    assert (string);
    if (streq (string, "pipe")
    ||  streq (string, "stream"))
        rc = TRUE;
    else
        rc = FALSE;
</method>

<method name = "is feed class" return = "rc">
    <doc>
    Returns TRUE if the supplied string is a feed class name.
    </doc>
    <argument name = "string" type = "char *">String to check</argument>
    <declare name = "rc" type = "int" default = "0" />
    //
    assert (string);
    if (streq (string, "fanout")
    ||  streq (string, "direct")
    ||  streq (string, "topic")
    ||  streq (string, "headers")
    ||  streq (string, "system")
    ||  streq (string, "service")
    ||  streq (string, "rotator"))
        rc = TRUE;
    else
        rc = FALSE;
</method>

<method name = "selftest">
    <local>
    zyre_uri_t
        *uri;
    </local>
    //
    assert (zyre_uri_is_pipe_class ("pipe"));
    assert (!zyre_uri_is_pipe_class ("piped"));
    assert (zyre_uri_is_feed_class ("topic"));
    assert (!zyre_uri_is_feed_class (""));

    uri = zyre_uri_new (NULL);

    assert (RESTMS_URI_ROOT    == zyre_uri_set (uri, ""));
    assert (RESTMS_URI_ROOT    == zyre_uri_set (uri, "/"));
    assert (RESTMS_URI_FEEDS   == zyre_uri_set (uri, "/direct"));
    assert (RESTMS_URI_FEED    == zyre_uri_set (uri, "/direct/amq.direct"));
    assert (RESTMS_URI_PIPES   == zyre_uri_set (uri, "/pipe"));
    assert (RESTMS_URI_PIPE    == zyre_uri_set (uri, "/pipe/my.pipe.0001"));
    assert (RESTMS_URI_ADDRESS == zyre_uri_set (uri, "/hello@amq.direct"));
    assert (RESTMS_URI_ADDRESS == zyre_uri_set (uri, "/hello@amq.direct/direct"));
    assert (RESTMS_URI_JOIN    == zyre_uri_set (uri, "/pipe/my.pipe.0001/hello@amq.direct"));
    assert (RESTMS_URI_JOIN    == zyre_uri_set (uri, "/pipe/my.pipe.0001/hello@amq.direct/direct"));
    assert (RESTMS_URI_MESSAGE == zyre_uri_set (uri, "/pipe/my.pipe.0001/1-100"));
    assert (RESTMS_URI_MESSAGE == zyre_uri_set (uri, "/pipe/my.pipe.0001/"));

    //  Invalid URIs
    assert (RESTMS_URI_INVALID == zyre_uri_set (uri, "//"));
    assert (RESTMS_URI_INVALID == zyre_uri_set (uri, "/direcx"));
    assert (RESTMS_URI_INVALID == zyre_uri_set (uri, "/direct/"));
    assert (RESTMS_URI_INVALID == zyre_uri_set (uri, "/pipe/"));
    assert (RESTMS_URI_INVALID == zyre_uri_set (uri, "/pope/"));
    assert (RESTMS_URI_INVALID == zyre_uri_set (uri, "/hello@amq.direct@beastie"));
    assert (RESTMS_URI_INVALID == zyre_uri_set (uri, "/hello@amq.direct/direcx"));
    assert (RESTMS_URI_INVALID == zyre_uri_set (uri, "/pipe/my.pipe.0001/hello@amq.direct/direcx"));
    assert (RESTMS_URI_INVALID == zyre_uri_set (uri, "/pipe/hello@amq.direct/direcx"));

    zyre_uri_destroy (&uri);
</method>

</class>
