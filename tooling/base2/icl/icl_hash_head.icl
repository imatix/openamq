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
    name      = "icl_hash_head"
    comment   = "Abstract base class for hash table container"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
    Abstract base class for hash table container
</doc>

<inherit name = "icl_object"/>

<invoke>
if hash_type = "int"
    key_arg_t     = "int"
    key_destroy_c = "item->key = 0"
    key_copy_c    = "item->key = key"
elsif hash_type = "lstr"
    key_arg_t     = "char *"
    key_destroy_c = "icl_mem_strfree (&item->key)"
    key_copy_c    = "item->key = icl_mem_strdup (key)"
elsif hash_type = "str"
    key_arg_t     = "char *"
    key_destroy_c = "icl_shortstr_cpy (item->key, \"\")"
    key_copy_c    = "icl_shortstr_cpy (item->key, key)"
endif
</invoke>

<public>
#define $(selfname:upper)_INITIAL_SIZE  $(hash_size?255)
#define $(selfname:upper)_LOAD_FACTOR   $(load_factor?75)
#define $(selfname:upper)_GROWTH_FACTOR $(growth_factor?200)

typedef void ($(selfname:c)_callback_fn) ($(childtype) *item, void *argument);
</public>

<context>
    size_t
        nbr_items;
    size_t
        max_items;
    $(childtype)
        **$(prefix)_items;
</context>

<method name = "new">
    self->max_items = $(selfname:upper)_INITIAL_SIZE;
    self->$(prefix)_items = icl_mem_alloc (sizeof ($(childtype)*) *
        $(selfname:upper)_INITIAL_SIZE);
    assert (self->$(prefix)_items);
    memset (self->$(prefix)_items, 0, sizeof ($(childtype)*) *
        $(selfname:upper)_INITIAL_SIZE);
</method>

<method name = "destroy">
    s_$(selfname)_purge (self);
    if (self->$(prefix)_items)
        icl_mem_free (self->$(prefix)_items);
</method>

<method name = "insert" template = "function">
    <doc>
    Insert a new item into the hash table if possible. If successful,
    the number of links to the item is automatically incremented.
    Trying to insert a duplicate key will result in nothing being inserted.
    </doc>
    <inherit name = "wrlock"/>
    <argument name = "key"  type = "$(key_arg_t)">Hash key</argument>
    <argument name = "item" type = "$(childtype) *">Item to insert</argument>
    //
    if (item->$(prefix)_head) {
        if (item->$(prefix)_head != self) {
            icl_console_print ("E: $(childname) item inserted into multiple $(prefix) containers");
            assert (item->$(prefix)_head == self);
        }
        s_$(selfname)_remove (item);
.if link_to_child ? 1
        $(childname)_unlink (&item);
.endif
    }
    rc = s_$(selfname)_insert (self, item, key);
.if link_to_child ? 1
    if (item->$(prefix)_head)
        $(childname)_link (item);
.endif
</method>

<method name = "remove" return ="rc">
    <doc>
    Remove an item from a hash table.  If the item was not in any hash
    table, this function does nothing.
    </doc>
    <inherit name = "wrlock"/>
    <argument name = "item" type = "$(childtype) *">Item to remove</argument>
    <declare  name = "self" type = "$(selftype) *" default = "item->$(prefix)_head">The list</declare>
    <declare  name = "rc"   type = "int" default = "0" />
    if (self
    &&  self == item->$(prefix)_head) { //  Catch case where item moved
        s_$(selfname)_remove (item);
.if link_to_child ? 1
        $(childname)_unlink (&item);
.endif
    }
</method>

<method name = "search" return = "item">
    <doc>
    Find object by key in the hash table. Returns reference to object
    if found, else null.
    </doc>
    <inherit  name = "rdlock"/>
    <argument name = "self" type = "$(selftype) *">Table to search</argument>
    <argument name = "key"  type = "$(key_arg_t)">Hash key</argument>
    <declare  name = "item" type = "$(childtype) *" default = "NULL"/>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    if (key)
        item = s_$(selfname)_lookup (self, key);
.if link_to_child ? 1
    if (item)
        $(childname)_link (item);
.endif
</method>

<method name = "apply">
    <doc>
    This method iterates through all the items in the hash and calls the
    callback function for each of them.
    </doc>
    <inherit name = "wrlock"/>
    <argument name = "self"     type = "$(selftype) *">Table to iterate</argument>
    <argument name = "callback" type = "$(selfname:c)_callback_fn *" />
    <argument name = "argument" type = "void *">Arbitrary argument pointer</argument>
    <local>
    size_t
        pos;
    $(childtype)
        *item;
    </local>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    for (pos = 0; pos != self->max_items; pos ++) {
        item = self->$(prefix)_items [pos];
        while (item) {
            callback (item, argument);
            item = item->$(prefix)_next;
        }
    }
</method>

<private name = "header">
//  Hash support functions
static inline unsigned int
    s_key_hash ($(key_arg_t) key, size_t hash_size);
static inline Bool
    s_key_equal ($(key_arg_t) key1, $(key_arg_t) key2);

//  Table functions to be called once locks are in place
static int
    s_$(selfname)_insert ($(selftype) *self, $(childtype) *item, $(key_arg_t) key);
static $(childtype) *
    s_$(selfname)_lookup ($(selftype) *self, $(key_arg_t) key);
static void
    s_$(selfname)_remove ($(childtype) *item);
static void
    s_$(selfname)_purge ($(selftype) *self);
</private>

<private name = "footer">
static inline unsigned int
s_key_hash ($(key_arg_t) key, size_t hash_size)
{
.if hash_type = "int"
    return ((uint) (((size_t) key) % hash_size));
.elsif hash_type = "lstr" | hash_type = "str"
    uint
        key_hash = 0;

    //  Torek hashing function
    while (*key) {
        key_hash *= 33;
        key_hash += *key;
        key++;
    }
    key_hash %= hash_size;
    return (key_hash);
.endif
}

static inline Bool
s_key_equal ($(key_arg_t) key1, $(key_arg_t) key2)
{
.if hash_type = "int"
    return (key1 == key2);
.elsif hash_type = "lstr" | hash_type = "str"
    return (streq (key1, key2));
.endif
}

static int
s_$(selfname)_insert ($(selftype) *self, $(childtype) *item, $(key_arg_t) key)
{
    qbyte
        index,
        new_index;
    $(childtype)
        *current_item,
        *next_item;
    int
        rc = 0;

    if (self->nbr_items >= self->max_items * $(selfname:upper)_LOAD_FACTOR / 100) {

        //  Load factor of hash table was exceeded
        //  We need to increase its size by GROWTH_FACTOR and rebuild it
        $(childtype)
            **new_items;
        size_t
            new_max_items;

        //  Create new hash table
        new_max_items = self->max_items * $(selfname:upper)_GROWTH_FACTOR / 100;
        new_items = icl_mem_alloc (sizeof ($(childtype)*) * new_max_items);
        assert (new_items);
        memset (new_items, 0, sizeof ($(childtype)*) * new_max_items);

        //  Move all items to the new hash table
        for (index = 0; index != self->max_items; index++) {
            current_item = self->$(prefix)_items [index];
            while (current_item) {
                next_item = current_item->$(prefix)_next;
                new_index = s_key_hash (current_item->key, new_max_items);
                current_item->$(prefix)_index = new_index;
                current_item->$(prefix)_next = new_items [new_index];
                new_items [new_index] = current_item;
                current_item = next_item;
            }
        }

        //  Destroy old hash table
        icl_mem_free (self->$(prefix)_items);
        self->$(prefix)_items = new_items;
        self->max_items = new_max_items;
    }

    //  Insert new item itself
    index = s_key_hash (key, self->max_items);
    current_item = self->$(prefix)_items [index];
    while (current_item) {
        if (s_key_equal (current_item->key, key)) {
            //  Attempted duplicate insertion
            rc = -1;
            break;
        }
        current_item = current_item->$(prefix)_next;
    }
    if (!current_item) {
        item->$(prefix)_head = self;
        item->$(prefix)_index = index;
        item->$(prefix)_next = self->$(prefix)_items [index];
        self->$(prefix)_items [index] = item;
        $(key_copy_c);
        self->nbr_items++;
    }
    return rc;
}

static $(childtype) *
s_$(selfname)_lookup ($(selftype) *self, $(key_arg_t) key)
{
    qbyte
        index;
    $(childtype)
        *item;

    index = s_key_hash (key, self->max_items);
    item = self->$(prefix)_items [index];

    while (item) {
        if (s_key_equal (item->key, key))
            break;
        item = item->$(prefix)_next;
    }
    return item;
}

static void
s_$(selfname)_remove ($(childtype) *item)
{
    $(childtype)
        *current_item,
        **prev_item;

    prev_item = &(item->$(prefix)_head->$(prefix)_items [item->$(prefix)_index]);
    current_item = item->$(prefix)_head->$(prefix)_items [item->$(prefix)_index];

    while (current_item) {
        if (current_item == item)
            break;
        prev_item = &(current_item->$(prefix)_next);
        current_item = current_item->$(prefix)_next;
    }
    assert (current_item == item);

    if (item) {
        item->$(prefix)_head->nbr_items--;
        *prev_item = item->$(prefix)_next;
        item->$(prefix)_head = NULL;
        item->$(prefix)_index = 0;
        item->$(prefix)_next = NULL;
        $(key_destroy_c);
    }
}

static void
s_$(selfname)_purge ($(selftype) *self)
{
    uint
        table_idx;
    $(childtype)
        *current_item,
        *next_item;

    for (table_idx = 0; table_idx < self->max_items; table_idx++) {
        current_item = self->$(prefix)_items [table_idx];
        while (current_item) {
            next_item = current_item->$(prefix)_next;
            s_$(selfname)_remove (current_item);
.if link_to_child ? 1
            $(childname)_unlink (&current_item);
.else
            $(childname)_destroy (&current_item);
.endif
            current_item = next_item;
        }
    }
}
</private>

<method name = "selftest">

</method>
</class>
