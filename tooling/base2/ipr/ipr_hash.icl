<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

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
    name      = "ipr_hash"
    comment   = "A loose reference hash"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
Implements a loose-reference hash with no locking, for use in lock-free agents.
The hash is indexed by a string key, and holds a void pointer reference.  Hash
items are reference counted, so use unlink and destroy appropriately.
</doc>

<inherit class = "icl_object">
    <option name = "alloc"  value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
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

<method name = "insert">
    <doc>
    Creates a new hash for the specified data value, does not return
    a hash item.  Asserts that the specified key does not already
    exist.
    </doc>
    <argument name = "table" type = "ipr_hash_table_t *" />
    <argument name = "key" type = "char *" />
    <argument name = "data" type = "void *" />
    <local>
    ipr_hash_t
        *hash;
    </local>
    //
    assert (!ipr_hash_table_search (table, key));
    hash = ipr_hash_new (table, key, data);
    ipr_hash_unlink (&hash);
</method>

<method name = "lookup" return = "data">
    <doc>
    Searches a hash table for a key value and returns the data reference if
    found, else null.
    </doc>
    <argument name = "table" type = "ipr_hash_table_t *" />
    <argument name = "key" type = "char *" />
    <declare name = "data" type = "void *" default = "NULL" />
    <local>
    ipr_hash_t
        *hash;
    </local>
    hash = ipr_hash_table_search (table, key);
    if (hash) {
        data = hash->data;
        ipr_hash_unlink (&hash);
    }
</method>

<method name = "delete">
    <doc>
    Deletes the hash for the specified data value, does not return
    a hash item.  Asserts that the specified key existed.
    </doc>
    <argument name = "table" type = "ipr_hash_table_t *" />
    <argument name = "key" type = "char *" />
    <local>
    ipr_hash_t
        *hash;
    </local>
    //
    hash = ipr_hash_table_search (table, key);
    assert (hash);
    ipr_hash_destroy (&hash);
</method>

<method name = "selftest">
    ipr_hash_table_t
        *table;
    ipr_hash_t
        *hash;
    char
        *some_data = "hello";

    table = ipr_hash_table_new ();
    hash = ipr_hash_new (table, "tmp_000000", some_data);
    ipr_hash_unlink (&hash);

    assert (ipr_hash_lookup (table, "tmp_000000") == some_data);
    hash = ipr_hash_table_search (table, "tmp_000000");
    assert (hash);
    ipr_hash_destroy (&hash);
    ipr_hash_table_destroy (&table);
</method>

</class>
