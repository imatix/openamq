<?xml?>
<class
    name      = "amq_index"
    comment   = "Message matching index class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
    Holds a match index, which is a text string on which AMQ will index and
    match messages.  Each index holds a bindset bitmap that represents all
    bindings that match that index.  Indices can be accessed via a hash
    lookup (by value) or array lookup (by number, for iterating through all
    indices).
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "prefix"    value = "hash" />
    <option name = "hash_type" value = "str" />
    <option name = "hash_size" value = "65535" />
    <option name = "rwlock"    value = "0" />
</inherit>
<inherit class = "icl_array_item">
    <option name = "prefix"    value = "array" />
    <option name = "rwlock"    value = "0" />
</inherit>

<import class = "amq_server_classes" />

<context>
    ipr_bits_t
        *bindset;                       //  Bitmap of bindings that match
</context>

<method name = "new">
    <!-- Always insert new items at the end of the array -->
    <dismiss argument = "index" value = "array->bound" />
    self->bindset = ipr_bits_new ();
</method>

<method name = "destroy">
    ipr_bits_destroy (&self->bindset);
</method>

<method name = "selftest">
    amq_index_hash_t
        *index_hash;
    amq_index_array_t
        *index_array;
    amq_index_t
        *index;

    index_hash  = amq_index_hash_new  ();
    index_array = amq_index_array_new ();

    index = amq_index_new (index_hash, "key1", index_array);
    amq_index_unlink (&index);
    index = amq_index_new (index_hash, "abc", index_array);
    amq_index_unlink (&index);
    index = amq_index_new (index_hash, "0x6262", index_array);
    amq_index_unlink (&index);
    index = amq_index_new (index_hash, "g00df00d", index_array);
    amq_index_unlink (&index);

    assert (index_array->bound == 4);
    amq_index_hash_destroy  (&index_hash);
    amq_index_array_destroy (&index_array);
</method>

</class>
