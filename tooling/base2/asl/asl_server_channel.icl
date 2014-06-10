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
    name      = "asl_server_channel"
    comment   = "ASL server channel class"
    version   = "1.0"
    target    = "smt"
    abstract  = "1"
    >
<doc>
    This class implements the ASL server channel class.  Server channels
    are asynchronous objects.
</doc>

<inherit class = "smt_object" />
<inherit class = "asl_channel" />

<private>
static volatile qbyte
    s_channel_id_seq_lo,                //  Next channel id
    s_channel_id_seq_hi;                //  High part (64-bits)
</private>

<context>
    $(basename)_connection_t
        *connection;                    //  Parent connection
    dbyte
        number;                         //  Channel number
    icl_shortstr_t
        id;                             //  Unique channel id (per broker)
    volatile qbyte
        credit;                         //  Current channel credit
</context>

<method name = "initialise">
    s_channel_id_seq_hi = 0;
    s_channel_id_seq_lo = 1;
</method>

<method name = "new">
    <argument name = "connection" type = "$(basename)_connection_t *">Parent connection</argument>
    <argument name = "number" type = "dbyte">Channel number</argument>
    <dismiss argument = "key" value = "number" />
    <local>
    qbyte
        seq_lo,
        seq_hi;
    </local>
    //
    self->connection = $(basename)_connection_link (connection);
    self->number = number;

    //  Get 64-bit channel id, no wraparound handling
    seq_lo = icl_atomic_inc32 (&s_channel_id_seq_lo);
    if (seq_lo == 0)
        seq_hi = icl_atomic_inc32 (&s_channel_id_seq_hi);
    else
        seq_hi = s_channel_id_seq_hi;
    icl_shortstr_fmt (self->id, "%x-%x", seq_hi, seq_lo);
</method>

<method name = "destroy">
    <action>
    $(basename)_connection_unlink (&self->connection);
    </action>
</method>

<method name = "spend" template = "function">
    icl_atomic_dec32 ((volatile qbyte *) &self->credit);
</method>

<method name = "earn" template = "function">
    icl_atomic_inc32 ((volatile qbyte *) &self->credit);
</method>

<method name = "error" template = "function">
    <doc>
    Raises a channel exception.
    </doc>
    <argument name = "reply code" type = "dbyte" >Error code</argument>
    <argument name = "reply text" type = "char *">Error text</argument>
    <argument name = "faulting class id" type = "dbyte" >Faulting class id</argument>
    <argument name = "faulting method id" type = "dbyte" >Faulting method id</argument>
    //
    $(basename)_agent_channel_error (
        self->connection->thread, self, reply_code, reply_text,
        faulting_class_id, faulting_method_id);
</method>

</class>
