<?xml?>
<class
    name      = "amq_exchange_list"
    comment   = "list of amq_exchanges"
    version   = "1.0"
    script    = "icl_gen"
    >

<import class = "amq_exchange" />

<inherit class = "icl_list_link_class">
    <option name = "object_name" value = "amq_exchange" />
</inherit>

<method name = "selftest">
</method>

</class>
