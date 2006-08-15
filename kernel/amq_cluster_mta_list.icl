<?xml?>
<class
    name      = "amq_cluster_mta_list"
    comment   = "list of amq_cluster_mtas"
    version   = "1.0"
    script    = "icl_gen"
    >

<import class = "amq_cluster_mta" />

<inherit class = "icl_list_link_class">
    <option name = "object_name" value = "amq_cluster_mta" />
</inherit>

<method name = "selftest">
</method>

</class>
