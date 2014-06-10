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
    name      = "ipr_index"
    comment   = "Provides object indexation functions"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
This class provides a mechanism for indexing objects.  An index is a number
from 0 to IPR_INDEX_MAX - 1. The class keeps indexes low by hashing across
a variable part of the index rather than across its full size.  Does not
support the retrieval of objects by their reference, only by index.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1" />
</inherit>
<inherit class = "icl_tracker" />

<import class = "ipr_bits" />

<public>
#define IPR_INDEX_MAX   IPR_BITS_SIZE_BITS
</public>

<context>
    int   size;                         //  Number of index entries
    void *data [IPR_INDEX_MAX];
</context>

<method name = "insert" template = "function">
    <doc>
    Inserts a new object into the index.  Returns the resulting index
    value, 1 and upwards.  If the same object is inserted twice, will
    create two index entries.  A return value of zero indicates that the
    table was full.
    </doc>
    <argument name = "object" type = "void *">Object to index</argument>
    <inherit name = "wrlock"/>
    <local>
    unsigned int
        hash_range,                     //  Size of hash range
        hash_index;                     //  Calculated index
    </local>
    assert (self->size < IPR_INDEX_MAX);
    hash_range = self->size * 2;
    if (hash_range > IPR_INDEX_MAX)
        hash_range = IPR_INDEX_MAX;
    else
    if (hash_range < 16)
        hash_range = 16;                //  Minimum hash range

    hash_index = (((unsigned long) object) % (hash_range - 1)) + 1;
    while (self->data [hash_index]) {
        hash_index++;
        if (hash_index == IPR_INDEX_MAX)
            hash_index = 0;
    }
    if (hash_index) {
        self->data [hash_index] = object;
        self->size++;
    }
    rc = hash_index;
</method>

<method name = "delete" template = "function">
    <doc>
    Deletes the specified index entry.  Note that you must specify
    the index value, not the object address.
    </doc>
    <argument name = "index" type = "int">Index entry to delete</argument>
    <inherit name = "wrlock"/>
    assert (index > 0 && index < IPR_INDEX_MAX);
    if (self->size > 0) {
        self->data [index] = 0;
        self->size--;
    }
</method>

<method name = "selftest">
    <local>
    ipr_index_t
        *index_table;
    char
        *value1 = "abc",
        *value2 = "abc",
        *value3 = "abc";
    int
        index;
    </local>

    index_table = ipr_index_new ();
    ipr_index_insert (index_table, value1);
    ipr_index_insert (index_table, value2);
    ipr_index_insert (index_table, value3);
    ipr_index_insert (index_table, value1);
    ipr_index_insert (index_table, value2);
    ipr_index_insert (index_table, value3);

    //  Fill the table half-full
    for (index = 0; index < IPR_INDEX_MAX / 2; index++)
        ipr_index_insert (index_table, value3);
    //  Insert a limit pointer
    index = ipr_index_insert (index_table, (void*) ((uint) (IPR_INDEX_MAX - 1)));
    ipr_index_delete (index_table, index);

    //  Clean-up
    ipr_index_destroy (&index_table);
</method>

</class>

