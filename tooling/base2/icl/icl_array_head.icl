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
    name      = "icl_array_head"
    comment   = "Abstract base class for array head container"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
    Abstract base class for array head container
</doc>

<inherit name = "icl_object"/>
<import class = "icl_longstr"/>

<private>
//  We increase the array in blocks of this many entries
#define ARRAY_EXTENT        1024
//  This is the largest array we allow (for sanity)
#define ARRAY_MAX_INDEX     1024*1024*128
</private>

<context>
    qbyte
        bound;                          //  Top bound, last + 1
    qbyte
        size;                           //  Size of array, nbr entries
    $(childtype)
        **data;                         //  Array data block
</context>

<method name = "destroy">
    <doc>
    Destroys the array and unlinks all attached items.
    </doc>
    s_$(selfname)_reset (self);
</method>

<method name = "reset" template = "function">
    <doc>
    Clears the array and frees all allocated memory.
    </doc>
    <inherit name = "wrlock"/>
    s_$(selfname)_reset (self);
</method>

<method name = "insert" template = "function">
    <doc>
    Insert a new item into the array or replace the existing item at
    same index.  The first valid index is zero.

    The number of links is automatically adjusted - if the item was previously
    in an array, it remains unchanged; otherwise it is incremented.
    </doc>
    <inherit name = "wrlock"/>
    <argument name = "index" type = "qbyte">Array index</argument>
    <argument name = "item"  type = "$(childtype) *">Item to insert</argument>
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
    s_$(selfname)_insert (self, item, index);
.if link_to_child ? 1
    if (item->$(prefix)_head)
        $(childname)_link (item);
.endif
</method>

<method name = "remove" return ="rc">
    <doc>
    Remove a specified item from an array.  If the item is not in any array,
    this function does nothing.

    If the item was in an array, the number of links is decremented.
    </doc>
    <inherit name = "wrlock"/>
    <argument name = "item"  type = "$(childtype) *">Item to remove</argument>
    <declare  name = "self" type = "$(selftype) *" default = "item->$(prefix)_head">The list</declare>
    <declare  name = "rc"    type = "int" default = "0" />
    if (self
    &&  self == item->$(prefix)_head) { //  Catch case where item moved
        s_$(selfname)_remove (item);
.if link_to_child ? 1
        $(childname)_unlink (&item);
.endif
    }
</method>

<method name = "fetch" return = "item">
    <doc>
    Fetch the item at specified index, if any.  If no item is defined at
    the specified index, returns NULL.

    If an item is found, its link count is incremented.
    </doc>
    <inherit name = "rdlock"/>
    <argument name = "self"  type = "$(selftype) *">The array</argument>
    <argument name = "index" type = "qbyte">Array index</argument>
    <declare name = "item" type = "$(childtype) *" default = "NULL">Item fetched</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    if (index < self->bound) {
        item = self->data [index];
.if link_to_child ? 1
        $(childname)_link (item);
.endif
    }
</method>

<method name = "ubound" return = "index">
    <doc>
    Returns the upper bound of the array, that is one more than the index
    of the last item in the array.
    </doc>
    <argument name = "self" type = "$(selftype) *">The array</argument>
    <declare  name = "index" type = "qbyte">The result</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    index = icl_atomic_get32 (&self->bound);
</method>

<private name = "header">
static void
    s_$(selfname)_reset  ($(selftype) *self);
static void
    s_$(selfname)_insert ($(selftype) *self, $(childtype) *item, qbyte index);
static void
    s_$(selfname)_remove ($(childtype) *item);
</private>

<private name = "footer">
static void
s_$(selfname)_reset ($(selftype) *self)
{
    qbyte
        index;
    $(childtype)
        *item;

    if (self->data) {
        for (index = 0; index < self->bound; index++) {
            item = self->data [index];
            if (item) {
                item->$(prefix)_head = NULL;
                item->$(prefix)_index = 0;
.if link_to_child ? 1
                $(childname)_unlink (&item);
.else
                $(childname)_destroy (&item);
.endif
            }
        }
        icl_mem_free (self->data);
        self->data = NULL;
    }
    self->bound = 0;
    self->size  = 0;
}

static void
s_$(selfname)_insert ($(selftype) *self, $(childtype) *item, qbyte index)
{
    size_t
        old_size,
        extent;
    $(childtype)
        *old_item;

    assert (item);
    assert (index < ARRAY_MAX_INDEX);

    if (index >= self->size) {
        old_size = sizeof ($(childtype) *) * self->size;
        extent   = sizeof ($(childtype) *) * ARRAY_EXTENT;
        self->size += ARRAY_EXTENT;
        self->data = icl_mem_realloc (self->data, old_size + extent);
        memset ((byte *) self->data + old_size, 0, extent);
    }
    //  Kick-out any previous occupant of this slot
    old_item = self->data [index];
    if (old_item) {
        old_item->$(prefix)_head = NULL;
        old_item->$(prefix)_index = 0;
        $(childname)_destroy (&old_item);
    }
    self->data [index] = item;
    if (self->bound <= index)
        self->bound = index + 1;

    item->$(prefix)_head = self;
    item->$(prefix)_index = index;
}

static void
s_$(selfname)_remove ($(childtype) *item)
{
    $(selfname)_t
        *self = item->$(prefix)_head;
    qbyte
        index = item->$(prefix)_index;

    if (self) {
        assert (index < self->size);
        assert (self-> data [index] == item);

        if (self->data) {
            item->$(prefix)_head = NULL;
            item->$(prefix)_index = 0;
            self->data [index] = NULL;
            while (self->bound > 0 && self->data [self->bound - 1] == NULL)
                self->bound--;
        }
    }
}
</private>

</class>

