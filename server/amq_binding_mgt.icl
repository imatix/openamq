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
    name      = "amq_binding_mgt"
    comment   = "Binding management class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This class shows one binding.
</doc>

<inherit class = "smt_object" />
<!-- any containers must come here -->
<inherit class = "amq_console_object" />
<inherit class = "smt_object_tracker" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "binding" parent = "exchange" label = "Binding">
        <field name = "name" label = "Routing key">
          <get>icl_shortstr_cpy (field_value, self->binding->routing_key);</get>
        </field>
        <field name = "arguments" label = "Binding arguments">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, self->binding->arguments_str);</get>
        </field>
        <field name = "queues" label = "Number of queues" type = "int">
          <rule name = "show on summary" />
          <get>icl_shortstr_fmt (field_value, "%d", amq_queue_set_count (self->binding->queue_set));</get>
        </field>
        <field name = "exclusive" label = "Exclusive binding?" type = "bool">
          <get>icl_shortstr_fmt (field_value, "%d", self->binding->exclusive);</get>
        </field>
    </class>
</data>

<import class = "amq_server_classes" />

<context>
    amq_exchange_t
        *exchange;                          //  Parent exchange for CML hierarchy
    amq_binding_t
        *binding;                           //  Binding we refer to
</context>

<method name = "new">
    <argument name = "exchange" type = "amq_exchange_t *">Parent exchange</argument>
    <argument name = "binding" type = "amq_binding_t *">Parent binding</argument>
    //
    self->exchange = exchange;
    self->binding = binding;
</method>

<method name = "selftest" />

</class>
