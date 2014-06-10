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
    name      = "http_address"
    comment   = "HTTP IP address class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Stores data about a known IP address, used for DoS detection and other
    administration.  The address holds a looseref list of http_agent threads.
</doc>
<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "links" value = "0" />
    <option name = "mutex" value = "1" />
</inherit>
<!-- Password table organised as random-access hash table -->
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>

<import class = "http" />

<context>
    ipr_looseref_list_t
        *thread_list;                   //  List of threads active on address
    size_t
        thread_quota;                   //  Remaining quota for threads
    /*
    - number of hits
    - first date/time
    - last date/time
    - threads open
    - list of threads
    - thread quota
    - throttle quota
        b/s
    */
</context>

<method name = "new">
    self->thread_list = ipr_looseref_list_new ();
    self->thread_quota = http_config_address_quota (http_config);
    //todo: if address is on whitelist then set to infinity
</method>

<method name = "destroy">
    <local>
    smt_thread_t
        *thread;
    </local>
    //
    while ((thread = (smt_thread_t *) ipr_looseref_pop (self->thread_list))) {
    icl_console_print ("end thread=%pp", thread);
        smt_thread_shut_down (thread);
        smt_thread_unlink (&thread);
    }
    ipr_looseref_list_destroy (&self->thread_list);
</method>

<method name = "thread open" return = "rc">
    <doc>
    Adds an http_agent thread to the address's list of threads and culls
    the oldest thread if the address has gone over quota.
    </doc>
    <argument name = "table" type = "http_address_table_t *" />
    <argument name = "context" type = "http_access_context_t *" />
    <declare name = "rc" type = "int" default = "0" />
    <local>
    http_address_t
        *self;
    smt_thread_t
        *thread;
    </local>
    //
    assert (table);
    assert (context);
    assert (context->address);
    assert (context->thread);

    self = http_address_table_search (table, context->address);
    if (!self) {
        self = http_address_new (table, context->address);
        //  This can return null if two threads create the same address at
        //  the same time... so let's go find the actual address object
        if (!self) {
            self = http_address_table_search (table, context->address);
            assert (self);
        }
    }
    assert (self);

    self_lock (self);
    ipr_looseref_queue (self->thread_list, smt_thread_link (context->thread));
    //  Cull oldest thread if quota is non-zero but would become zero
    //  I.e. if it's exactly one... a quota of zero means "unlimited".
    if ( self->thread_quota
    && --self->thread_quota == 0) {
        //  Find oldest looseref entry
        thread = (smt_thread_t *) ipr_looseref_pop (self->thread_list);
        assert (thread);
        smt_thread_shut_down (thread);
        smt_thread_unlink (&thread);
        self->thread_quota++;
    }
    self_unlock (self);
</method>

<method name = "thread close" return = "rc">
    <doc>
    Removes an http_agent thread from the address's list of threads.  The
    thread will be missing from the table, if it was already culled.
    </doc>
    <argument name = "table" type = "http_address_table_t *" />
    <argument name = "context" type = "http_access_context_t *" />
    <declare name = "rc" type = "int" default = "0" />
    <local>
    ipr_looseref_t
        *looseref;
    http_address_t
        *self;
    smt_thread_t
        *thread;
    </local>
    //
    assert (table);
    assert (context);
    assert (context->address);
    assert (context->thread);

    self = http_address_table_search (table, context->address);
    assert (self);
    self_lock (self);

    //  Find looseref entry for context->thread
    looseref = ipr_looseref_list_first (self->thread_list);
    while (looseref
    && (smt_thread_t *) looseref->object != context->thread)
        looseref = ipr_looseref_list_next (&looseref);

    if (looseref) {
        //  Now kill the thread and get back our quota
        thread = (smt_thread_t *) looseref->object;
        ipr_looseref_destroy (&looseref);
        assert (thread);
        smt_thread_shut_down (thread);
        smt_thread_unlink (&thread);
        self->thread_quota++;
    }
    self_unlock (self);
</method>

<method name = "selftest" />

</class>
