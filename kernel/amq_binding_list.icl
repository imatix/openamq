<?xml?>
<class
    name      = "amq_binding_list"
    comment   = "list of bindings"
    version   = "1.0"
    script    = "icl_gen"
    >

<import class = "amq_binding" />

<inherit class = "icl_list_link_class">
    <option name = "object_name" value = "amq_binding" />
</inherit>

<method name = "selftest">
</method>

</class>
