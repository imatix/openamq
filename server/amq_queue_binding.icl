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
    name      = "amq_queue_binding"
    comment   = "Queue binding management class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This class shows one queue binding, navigable from the queue object.
</doc>

<inherit class = "smt_object" />
<!-- any containers must come here -->
<inherit class = "amq_console_object" />
<inherit class = "smt_object_tracker" />

<!-- Console definitions for this object -->
<data name = "cml">
    <class name = "queue_binding" parent = "queue" label = "Queue binding">
        <field name = "name" label = "Routing key">
          <get>icl_shortstr_cpy (field_value, self->routing_key);</get>
        </field>
        <field name = "exchange_name" label = "Exchange name">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, self->exchange_name);</get>
        </field>
        <field name = "exchange_type" label = "Exchange type">
          <rule name = "show on summary" />
          <get>icl_shortstr_cpy (field_value, self->exchange_type);</get>
        </field>
        <field name = "binding_args" label = "Binding arguments">
          <get>icl_shortstr_cpy (field_value, self->binding_args);</get>
        </field>
        <field name = "exclusive" label = "Exclusive binding?" type = "bool">
          <get>icl_shortstr_fmt (field_value, "%d", self->exclusive);</get>
        </field>
    </class>
</data>

<import class = "amq_server_classes" />

<context>
    amq_queue_t
        *queue;
    amq_exchange_t
        *exchange;
    amq_binding_t
        *binding;
    icl_shortstr_t
        routing_key,
        exchange_name,
        exchange_type,
        binding_args;
    Bool
        exclusive;
</context>

<method name = "new">
    <argument name = "queue" type = "amq_queue_t *">Parent queue</argument>
    <argument name = "exchange" type = "amq_exchange_t *">Parent exchange</argument>
    <argument name = "binding" type = "amq_binding_t *">Parent binding</argument>
    <local>
    asl_field_list_t
        *field_list;
    </local>
    //
    //  Don't link to these objects because it makes deconstruction impossible
    self->queue    = queue;
    self->exchange = exchange;
    self->binding  = binding;

    icl_shortstr_cpy (self->exchange_name, exchange->name);
    icl_shortstr_cpy (self->exchange_type, amq_exchange_type_name (exchange->type));
    icl_shortstr_cpy (self->routing_key,   binding->routing_key);

    // Convert binding arguments to human readable string
    field_list = asl_field_list_new (binding->arguments);
    assert (field_list);
    asl_field_list_dump (field_list, self->binding_args);
    asl_field_list_destroy (&field_list);
    self->exclusive = binding->exclusive;
</method>

<method name = "selftest" />

</class>
