<?xml?>
<class
    name      = "amq_queue_list"
    comment   = "list of amq_queues"
    version   = "1.0"
    script    = "icl_gen"
    >

<import class = "amq_queue" />

<inherit class = "icl_list_link_class">
    <option name = "object_name" value = "amq_queue" />
</inherit>

<method name = "selftest">
</method>

</class>
