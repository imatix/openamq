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
    name      = "amq_sequence"
    comment   = "Sequence checking for messages"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
Tracks the timestamp for each of a set of senders.  Given a new timestamp and
a sender, verifies that the timestamp is equal to or more recent than the last
known timestamp for the sender.
</doc>

<inherit class = "icl_object">
    <option name = "alloc"  value = "cache" />
    <option name = "rwlock" value = "0" />
    <option name = "links"  value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
    <option name = "rwlock"    value = "0" />
</inherit>

<context>
    apr_time_t
        timestamp;                      //  Last known time for sender
</context>

<method name = "new" >
    <argument name = "timestamp" type = "apr_time_t" />
    //
    self->timestamp = timestamp;
</method>

<method name = "check" return = "rc">
    <doc>
    Checks the provided sender id and timestamp, returns 0 if ok, and -1 if
    the timestamp is out of sequence.
    </doc>
    <argument name = "table" type = "amq_sequence_table_t *" />
    <argument name = "sender id" type = "char *" />
    <argument name = "timestamp" type = "apr_time_t" />
    <declare name = "rc" type = "int" default = "0" />
    <local>
    amq_sequence_t
        *entry;
    </local>
    //
    entry = amq_sequence_table_search (table, sender_id);
    if (entry) {
        if (timestamp < entry->timestamp)
            rc = -1;
    }
    else
        entry = amq_sequence_new (table, sender_id, timestamp);
    entry->timestamp = timestamp;
</method>

<method name = "selftest">
    amq_sequence_table_t
        *sequences;

    sequences = amq_sequence_table_new ();
    assert (amq_sequence_check (sequences, "sender", 0) == 0);
    assert (amq_sequence_check (sequences, "sender", 1) == 0);
    assert (amq_sequence_check (sequences, "sender", 0) == -1);
    assert (amq_sequence_check (sequences, "sender", 1) == 0);
    assert (amq_sequence_check (sequences, "sender", 1) == 0);
    amq_sequence_table_destroy (&sequences);
</method>

</class>
