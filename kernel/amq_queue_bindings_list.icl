<?xml?>
<class
    name      = "amq_queue_bindings_list"
    comment   = "list of all bindings for a particular queue"
    version   = "1.0"
    script    = "icl_gen"
    >

<import class = "amq_binding" />

<inherit class = "icl_list_link_class">
    <option name = "object_name" value = "amq_binding" />
</inherit>

<inherit class = "icl_hash_item">
    <option name = "hash_size" value = "65535" />
    <option name = "hash_type" value = "str" />
</inherit>

<method name = "selftest" />

</class>
