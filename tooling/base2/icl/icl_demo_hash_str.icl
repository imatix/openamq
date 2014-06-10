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
    name      = "icl_demo_hash_str"
    comment   = "Demo of a hash class built using string keys"
    version   = "1.0b0"
    script    = "icl_gen"
    >

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "initial_size" value = "15" />
    <option name = "hash_type" value = "str" />
</inherit>

<context>
    char *name;
    char *address;
</context>

<method name = "new">
    <argument name = "name"    type = "char *" />
    <argument name = "address" type = "char *" />
    self->name    = icl_mem_strdup (name);
    self->address = icl_mem_strdup (address);
</method>

<method name = "destroy">
    icl_mem_free (self->name);
    icl_mem_free (self->address);
</method>

<!-- Tester ------------------------------------------------------------------>

<method name = "selftest" >
    <local>
    $(selfname)_table_t
        *table;
    $(selfname)_t
        *item;
    struct {
        icl_shortstr_t name;
        Bool exists;
    } testset [200];
    int
        testmax = 200,
        testnbr,
        iteration;
    </local>

    table = $(selfname)_table_new ();

    //  Insert some items
    item = $(selfname)_new (table, "0xdeadbeef", "deadbeef", "deadbeef street");
    $(selfname)_unlink (&item);
    item = $(selfname)_new (table, "0xabadcafe", "abadcafe", "deadbeef cafe");
    $(selfname)_unlink (&item);

    //  Retrieve items
    item = $(selfname)_table_search (table, "0xdeadbeef");
    assert (item && streq (item->name, "deadbeef"));
    $(selfname)_unlink (&item);
    item = $(selfname)_table_search (table, "0xabadcafe");
    assert (item && streq (item->name, "abadcafe"));
    $(selfname)_unlink (&item);

    //  Get a non existent item
    item = $(selfname)_table_search (table, "0xf0000000");
    assert (item == NULL);

    //  Delete an item
    item = $(selfname)_table_search (table, "0xdeadbeef");
    $(selfname)_destroy (&item);
    item = $(selfname)_table_search (table, "0xdeadbeef");
    assert (item == NULL);

    //  Check that the queue is robust against random usage
    memset (testset, 0, sizeof (testset));
    for (iteration = 0; iteration < 100000; iteration++) {
        testnbr = randomof (testmax);
        if (testset [testnbr].exists) {
            item = $(selfname)_table_search (table, testset [testnbr].name);
            assert (item);
            $(selfname)_destroy (&item);
            testset [testnbr].exists = FALSE;
        }
        else {
            icl_shortstr_fmt (testset [testnbr].name, "%x-%x", rand (), rand ());
            item = $(selfname)_new (table, testset [testnbr].name, "name", "address");
            $(selfname)_unlink (&item);
            testset [testnbr].exists = TRUE;
        }
    }
    $(selfname)_table_destroy (&table);
</method>

</class>
