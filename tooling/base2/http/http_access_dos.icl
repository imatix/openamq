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
    name    = "http_access_dos"
    comment = "Guard against basic denial-of-service attacks"
    script  = "smt_object_gen"
    target  = "smt"
    >
<doc>
    Implements connection quotas per IP address.
</doc>

<inherit class = "http_access_module_back" />

<import class = "http" />

<context>
    http_address_table_t
        *address_table;
</context>

<method name = "new">
    self->address_table = http_address_table_new ();
</method>

<method name = "destroy">
    http_address_table_destroy (&self->address_table);
</method>

<method name = "announce">
    smt_log_print (log, "I: X5 DoS detection module loaded");
</method>

<method name = "open">
    //  The http_address class does the interesting work
    //  Culling older threads for the same IP address happens by sending
    //  shutdown events to those threads, rather than by setting a return
    //  code here.
    http_address_thread_open (self->address_table, context);
</method>

<method name = "close">
    http_address_thread_close (self->address_table, context);
</method>

</class>
