<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

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
    name    = "amq_exchange_fanout"
    comment = "Fanout exchange class"
    version = "1.0"
    script  = "icl_gen"
    >
<doc>
This class implements the fanout exchange.  All messages are
routed to all bound queues and exchanges.
</doc>

<inherit class = "amq_exchange_base" />

<method name = "compile">
    //  Until we actually do something here, stop the compiler complaining
    //  about self not being used in this method...
    if (self);
</method>

<method name = "publish">
    <local>
    amq_binding_t
        *binding;                       //  Auxiliary variable
    </local>
    //
    //  First binding is the only binding
    binding = amq_binding_list_first (self->exchange->binding_list);
    if (binding) {
        delivered += amq_binding_publish (binding, channel, method);
        amq_binding_unlink (&binding);
    }
</method>

</class>
