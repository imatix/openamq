<?xml?>
<!--
    Copyright (c) 2009 iMatix Corporation

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
    name      = "amq_exchange_regexp"
    comment   = "regexp exchange class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
This class implements the regexp exchange, which routes messages
based on the routing_key matched against a regular expression.
Max different routing keys per exchange is limited by size of
amq_index_hash table.
</doc>

<inherit class = "amq_exchange_base" />

<context>
    amq_index_hash_t
        *index_hash;                    //  Access by text key
    amq_index_array_t
        *index_array;                   //  Access by number 0..n-1
</context>

<method name = "new">
    self->index_hash  = amq_index_hash_new  ();
    self->index_array = amq_index_array_new ();
</method>

<method name = "destroy">
    amq_index_hash_destroy  (&self->index_hash);
    amq_index_array_destroy (&self->index_array);
</method>

<method name = "compile">
    <doc>
    Compiles a regexp binding, based on the routing key, which is a
    Perl 5 compatible regexp - see http://www.pcre.org. An empty
    routing key is valid, and is treated as matching nothing.
    </doc>
    <local>
    ipr_regexp_t
        *regexp;                        //  Regular expression object
    uint
        index_nbr;                      //  Index number in self->index_array
    amq_index_t
        *index;                         //  Index reference from index_array
    </local>
    //
    if (amq_server_config_debug_route (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "X: compile  %s: wildcard=%s", self->exchange->name, binding->routing_key);

    //  We scan all indices to see which ones match our regexp
    regexp = ipr_regexp_new (NULL);
    ipr_regexp_compile (regexp, binding->routing_key, FALSE);

    for (index_nbr = 0; index_nbr < self->index_array->bound; index_nbr++) {
        index = amq_index_array_fetch (self->index_array, index_nbr);
        if (index) {
            if (ipr_regexp_match (regexp, index->key, NULL)) {
                if (amq_server_config_debug_route (amq_server_config))
                    smt_log_print (amq_broker->debug_log,
                        "X: index    %s: wildcard=%s routing_key=%s",
                        self->exchange->name, binding->routing_key, index->key);

                //  Cross-reference binding and index
                ipr_bits_set (index->bindset, binding->index);
                ipr_looseref_queue (binding->index_list, index);
            }
            amq_index_unlink (&index);
        }
    }
    ipr_regexp_destroy (&regexp);
</method>

<method name = "publish">
    <local>
    amq_index_t
        *index;                         //  Index object
    amq_binding_t
        *binding;                       //  Auxiliary variable
    ipr_regexp_t
        *regexp;                        //  Regular expression object
    int
        binding_nbr;                    //  Binding number, 1..n from bindset
    </local>
    //
    //  Check if routing_key is already indexed, else reindex bindings on it
    index = amq_index_hash_search (self->index_hash, routing_key);
    if (index == NULL) {
        if (amq_server_config_debug_route (amq_server_config))
            smt_log_print (amq_broker->debug_log,
                "X: newre   %s: regexp=%s", self->exchange->name, routing_key);

        //  Create new index and recompile all bindings for it
        index = amq_index_new (self->index_hash, routing_key, self->index_array);
        binding = amq_binding_list_first (self->exchange->binding_list);

        while (binding) {
            regexp = ipr_regexp_new (NULL);
            ipr_regexp_compile (regexp, binding->routing_key, FALSE);
            if (ipr_regexp_match (regexp, routing_key, NULL)) {
                if (amq_server_config_debug_route (amq_server_config))
                    smt_log_print (amq_broker->debug_log,
                        "X: index    %s: routing_key=%s wildcard=%s",
                        self->exchange->name, routing_key, binding->routing_key);

                //  Cross-reference binding and index
                ipr_bits_set (index->bindset, binding->index);
                ipr_looseref_queue (binding->index_list, index);
            }
            ipr_regexp_destroy (&regexp);
            binding = amq_binding_list_next (&binding);
        }
        if (binding)                    //  If we stopped before the end of the list
            amq_binding_unlink (&binding);
    }
    if (amq_server_config_debug_route (amq_server_config))
        smt_log_print (amq_broker->debug_log,
            "X: route    %s: routing_key=%s", self->exchange->name, routing_key);

    assert (index);

    //  Collect all queues to publish to
    binding_nbr = ipr_bits_first (index->bindset);
    while (binding_nbr >= 0) {
        binding = self->exchange->binding_index->data [binding_nbr];
        assert (binding);
        if (amq_server_config_debug_route (amq_server_config))
            smt_log_print (amq_broker->debug_log,
                "X: hit      %s: wildcard=%s", self->exchange->name, binding->routing_key);
        set_size = amq_binding_collect (binding, self->exchange->queue_set, set_size);
        binding_nbr = ipr_bits_next (index->bindset, binding_nbr);
    }
    amq_index_unlink (&index);
    //  The queue_set is processed in the footer of this function in
    //  amq_exchange_base.icl, the same way for all exchanges
</method>

</class>
