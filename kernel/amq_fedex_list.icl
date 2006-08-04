<?xml?>
<class
    name      = "amq_fedex_list"
    comment   = "list of amq_fedexes"
    version   = "1.0"
    script    = "icl_gen"
    >

<import class = "amq_fedex" />

<inherit class = "icl_list_link_class">
    <option name = "object_name" value = "amq_fedex" />
</inherit>

<method name = "selftest">
</method>

</class>
