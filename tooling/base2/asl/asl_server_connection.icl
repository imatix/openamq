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
    name      = "asl_server_connection"
    comment   = "ASL server connection class"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
    This class implements the ASL server connection class.  Connections
    are numbered on a per-server basis and stored in a hash table so we
    can find the connection for a given number.
</doc>

<option name = "links" value = "1" />
<inherit class = "icl_object">
    <option name = "alloc"  value = "cache" />
    <option name = "rwlock" value = "1" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str"   />
</inherit>
<inherit class = "icl_list_item">
    <option name = "prefix" value = "list" />
</inherit>
<inherit class = "icl_init" />
<inherit class = "icl_tracker" />

<import class = "asl" />
<import class = "$(basename)_channel" />
<import class = "$(basename)_method" />

<private>
static volatile qbyte
    s_context_seq_lo,                   //  Next context sequence id
    s_context_seq_hi;                   //  High part (64-bits)
</private>

<context>
    //  References to parent objects
    smt_thread_t
        *thread;                        //  Parent thread

    //  Object properties
    apr_time_t
        started;                        //  Time started
    icl_shortstr_t
        id;                             //  Connection identifier
    $(basename)_channel_table_t
        *channels;                      //  Active channels
    dbyte
        channel_max,                    //  Negotiated channel limit
        heartbeat;                      //  Connection heartbeat
    qbyte
        frame_max;                      //  Negotiated maximum frame size
    Bool
        exception_raised;               //  Indicates exception raised
    dbyte
        reply_code;                     //  Exception error code
    icl_shortstr_t
        reply_text;                     //  Exception error text
    dbyte
        faulting_method_id,             //  Class id of faulting method
        faulting_class_id;              //  Method id of faulting method
    Bool
        nowarning,                      //  Suppress disconnect warnings
        exception,                      //  Indicates connection exception
        authorised;                     //  Connection authorised?
    int
        trace;                          //  Last known trace value

    icl_shortstr_t
        client_address;                 //  IP address:port of client
    icl_shortstr_t
        client_product;                 //  Reported by client
    icl_shortstr_t
        client_version;                 //  Client version
    icl_shortstr_t  
        client_platform;                //  Client OS/language
    icl_shortstr_t
        client_copyright;               //  Client copyright
    icl_shortstr_t
        client_information;             //  Client information
    icl_shortstr_t
        client_instance;                //  Client instance name

    //  If the client connection comes from a server, it will also supply
    //  us with information that lets make a call-back connection to it
    icl_shortstr_t
        client_proxy_name;              //  Server name
    icl_shortstr_t
        client_proxy_host;              //  Server address and port

    //  Statistics
    int64_t
        traffic_in,                     //  Traffic in, in octets
        traffic_out,                    //  Traffic out, in octets
        contents_in,                    //  Contents in
        contents_out;                   //  Contents out
</context>

<method name = "initialise">
    s_context_seq_hi = 0;
    s_context_seq_lo = 1;
</method>

<method name = "new">
    <argument name = "thread" type = "smt_thread_t *">Parent thread</argument>
    <dismiss argument = "key" value = "self->id" />
    <local>
    qbyte
        seq_lo,
        seq_hi;
    </local>
    //
    if (thread)
        self->thread  = smt_thread_link (thread);
    self->started     = smt_time_now ();
    self->channel_max = 1;              //  We only support one channel 
    self->frame_max   = $(basename)_config_frame_max ($(basename)_config);
    self->heartbeat   = $(basename)_config_heartbeat ($(basename)_config);
    self->trace       = $(basename)_config_trace ($(basename)_config);
    self->channels    = $(basename)_channel_table_new ();

    //  Get 64-bit connection id, no wraparound handling
    seq_lo = icl_atomic_inc32 (&s_context_seq_lo);
    if (seq_lo == 0)
        seq_hi = icl_atomic_inc32 (&s_context_seq_hi);
    else
        seq_hi = s_context_seq_hi;
    icl_shortstr_fmt (self->id, "%x-%x", seq_hi, seq_lo);
</method>

<method name = "destroy">
    //  This closes all channels when we call destroy on the connection
    $(basename)_channel_table_destroy (&self->channels);
</method>

<method name = "free">
    //  This ensures the thread is valid until the connection is no longer used
    smt_thread_unlink (&self->thread);
</method>

<method name = "set address" template = "function">
    <doc>
    Sets the connection's address string, which can be used for logging
    and reporting.
    </doc>
    <argument name = "address" type = "char *">Client address</argument>
    <inherit name = "wrlock" />
    //
    icl_shortstr_cpy (self->client_address, address);
</method>

<method name = "set trace" template = "function">
    <doc>
    Sets the connection's trace level, used to determine the logging done
    at the connection thread level (in the server agent).
    </doc>
    <argument name = "trace" type = "int">Desired trace level</argument>
    <inherit name = "wrlock" />
    //
    $(basename)_agent_set_trace (self->thread, trace);
    self->trace = trace;                //  Last known value
</method>

<method name = "kill" template = "function">
    <doc>
    Kills the connection by sending a connection-kill method to the
    agent.
    </doc>
    <inherit name = "rdlock" />
    //
    $(basename)_agent_kill_connection (self->thread);
</method>

<method name = "ready" template = "function">
    <inherit name = "rdlock" />
    rc = TRUE;
</method>

<method name = "raise exception" template = "function">
    <doc>
    Raise a connection exception.  This method is for internal use by 
    the connection class or server agent only.  External callers should 
    call the "error" method instead.
    </doc>
    <argument name = "reply code" type = "dbyte" >Error code</argument>
    <argument name = "reply text" type = "char *">Error text</argument>
    <argument name = "faulting method id" type = "dbyte">Faulting method id</argument>
    <argument name = "faulting class id" type = "dbyte">Faulting class id</argument>
    //
    //  TODO Find a way to assert this is not being called from the wrong place
    self->exception_raised  = TRUE;
    self->reply_code = reply_code;
    icl_shortstr_cpy (self->reply_text, reply_text);
    self->faulting_class_id = faulting_class_id;
    self->faulting_method_id = faulting_method_id;
</method>

<method name = "error" template = "function">
    <doc>
    Raise a connection exception.  This method is for external use only,
    internal callers should call the "raise exception" method instead.
    </doc>
    <argument name = "reply code" type = "dbyte" >Error code</argument>
    <argument name = "reply text" type = "char *">Error text</argument>
    <argument name = "faulting method id" type = "dbyte">Faulting method id</argument>
    <argument name = "faulting class id" type = "dbyte">Faulting class id</argument>
    //
    $(basename)_agent_connection_error (self->thread, reply_code, reply_text,
        faulting_method_id, faulting_class_id);
</method>

<method name = "exception raised" template = "function">
    <doc>
    Returns TRUE if a connection exception has been raised.
    </doc>
    <inherit name = "rdlock" />
    rc = self->exception_raised;
</method>

<method name = "start ok" template = "function">
    <doc>
    Implements the ASL Connection.Start-Ok protocol method.
    </doc>
    <argument name = "method" type = "$(basename)_connection_start_ok_t *" />
    <inherit name = "wrlock" />
    <local>
    asl_field_list_t
        *fields;                        //  Decoded responses
    </local>
    //
    self->authorised = FALSE;
    fields = asl_field_list_new (method->client_properties);
    if (fields) {
        asl_field_list_cpy (fields, self->client_product,     "product");
        asl_field_list_cpy (fields, self->client_version,     "version");
        asl_field_list_cpy (fields, self->client_platform,    "platform");
        asl_field_list_cpy (fields, self->client_copyright,   "copyright");
        asl_field_list_cpy (fields, self->client_information, "information");
        //  Optionally, we could handle the following cases:
        //  1. duplicate connection instance name
        //  2. empty connection instance name
        asl_field_list_cpy (fields, self->client_instance,    "instance");
        asl_field_list_cpy (fields, self->client_proxy_name,  "proxy name");
        asl_field_list_cpy (fields, self->client_proxy_host,  "proxy host");
        asl_field_list_destroy (&fields);
    }
</method>

<method name = "tune ok" template = "function">
    <doc>
    Implements the ASL Connection.Tune-Ok protocol method.
    </doc>
    <argument name = "method" type = "$(basename)_connection_tune_ok_t *" />
    <inherit name = "wrlock" />
    //
    self->heartbeat = method->heartbeat;
    self->frame_max = method->frame_max;
    if (self->frame_max < ASL_FRAME_MIN_SIZE
    &&  self->frame_max > 0)
        self->frame_max = ASL_FRAME_MIN_SIZE;
</method>

<method name = "open" template = "function">
    <doc>
    Implements the ASL Connection.Open protocol method.
    </doc>
    <argument name = "method" type = "$(basename)_connection_open_t *" />
    <inherit name = "wrlock" />
</method>

<method name = "selftest" />

</class>
