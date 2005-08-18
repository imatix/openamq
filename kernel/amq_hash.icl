<?xml?>
<class
    name      = "amq_hash"
    comment   = "A loose reference hash"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
Implements a loose-reference hash with no locking, for use
in lock-free agents (exchanges, queues).
</doc>

<inherit class = "icl_object">
    <option name = "alloc"  value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str"   />
    <option name = "hash_size" value = "65535" />
    <option name = "rwlock"    value = "0" />
</inherit>

<context>
    void *data;
</context>

<method name = "new" >
    <argument name = "data" type = "void *" />
    //
    self->data = data;
</method>

<method name = "selftest" />

</class>
