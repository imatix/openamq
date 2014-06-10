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
    name      = "icl_demo_hash_int"
    comment   = "Demo of a hash class built using integer keys"
    version   = "1.0b0"
    script    = "icl_gen"
    >

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "initial_size" value = "15" />
    <option name = "hash_type" value = "int" />
</inherit>

<context>
    int dummy;
</context>

<method name = "new">
    self->dummy = 0;
</method>

<!-- Tester ------------------------------------------------------------------>

<method name = "selftest" >
    <local>
    size_t
        index;
    $(selfname)_table_t *
        table;
    $(selfname)_t
        *item,
        *current_item,
        *next_item;
    int
        count;
    apr_time_t
        start_time,
        end_time;
    int
        key;
    </local>

    table = $(selfname)_table_new ();

    randomize ();

    start_time = apr_time_now ();
    for (count = 0; count < 100000; count++) {
        key = randomof (1000000);
        item = $(selfname)_new (table, key);
        $(selfname)_unlink (&item);
    }
    end_time = apr_time_now ();
    icl_console_print ("Inserting %ld items took %ld microseconds (%ld)",
        (long) count, (long) (end_time - start_time), 
        (long) (table->nbr_items));

    start_time = apr_time_now ();
    for (index = 0; index != table->max_items; index++) {
        current_item = $(selfname)_link (table->table_items [index]);
        while (current_item) {
            next_item = current_item->table_next;
            $(selfname)_remove_from_all_containers (current_item);
            $(selfname)_destroy (&current_item);
            current_item = $(selfname)_link (next_item);
        }
    }
    end_time = apr_time_now ();
    icl_console_print ("Destroying %ld items took %ld microseconds (%ld)",
        (long) count, (long) (end_time - start_time), 
        (long) (table->nbr_items));

    start_time = apr_time_now ();
    for (count = 0; count < 100000; count++) {
        key = randomof (1000000);
        item = $(selfname)_new (table, key);
        $(selfname)_unlink (&item);
    }
    end_time = apr_time_now ();
    icl_console_print ("Inserting %ld items took %ld microseconds (%ld)",
        (long) count, (long) (end_time - start_time), 
        (long) (table->nbr_items)); 

    $(selfname)_table_destroy (&table);
</method>

</class>
