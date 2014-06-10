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
    name      = "ipr_symbol"
    comment   = "A simple hashed symbol table"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
Implements a symbol table that holds arbitrary data, keyed by name.
Can be used directly, or inherited to create specific symbol table
structures.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str"   />
</inherit>
<import class = "ipr_regexp" />

<context>
    void
        *data;                          //  Data value for item
</context>

<method name = "new">
    <argument name = "data" type = "void *">Data for symbol</argument>
    self->data = data;
</method>

<method name = "destroy">
</method>

<method name = "assume">
    <doc>
    Create or update symbol with specified value.
    </doc>
    <argument name = "table" type = "ipr_symbol_table_t *">Symbol table</argument>
    <argument name = "name"  type = "char *">Symbol name</argument>
    <argument name = "data"  type = "void *">Data for symbol</argument>
    <local>
    ipr_symbol_t
        *item;                          //  Inserted item
    </local>
    item = ipr_symbol_table_search (table, name);
    if (item)
        item->data = data;
    else
        item = ipr_symbol_new (table, name, data);

    ipr_symbol_unlink (&item);
</method>

<method name = "selftest">
    <local>
    ipr_symbol_table_t *
        table;
    ipr_symbol_t *
        item;
    </local>

    table = ipr_symbol_table_new ();

    //  Insert some items                                                      
    item = ipr_symbol_new (table, "0xdeadbeef", "deadbeef");
    ipr_symbol_unlink (&item);
    item = ipr_symbol_new (table, "0xabadcafe", "abadcafe");
    ipr_symbol_unlink (&item);

    //  Retrieve items                                                         
    item = ipr_symbol_table_search (table, "0xdeadbeef");
    assert (item && streq (item->data, "deadbeef"));
    ipr_symbol_unlink (&item);
    item = ipr_symbol_table_search (table, "0xabadcafe");
    assert (item && streq (item->data, "abadcafe"));
    ipr_symbol_unlink (&item);

    //  Get a non existent item                                                
    item = ipr_symbol_table_search (table, "0xf0000000");
    assert (item == NULL);

    //  Delete an item                                                         
    item = ipr_symbol_table_search (table, "0xdeadbeef");
    ipr_symbol_destroy (&item);
    item = ipr_symbol_table_search (table, "0xdeadbeef");
    assert (item == NULL);

    ipr_symbol_table_destroy (&table);
</method>

</class>
