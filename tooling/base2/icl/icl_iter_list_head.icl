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
    name      = "icl_list_head"
    comment   = "Abstract base class for list head container"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
    Abstract base class for list head container
</doc>

<inherit name = "icl_object" />

<public>
//  The iterator type

typedef struct _$(selfname)_iter_t $(selfname)_iter_t;

struct _$(selfname)_iter_t {
    volatile $(selfname)_t *head;
    volatile $(selfname)_iter_t *prev;
    volatile $(selfname)_iter_t *next;
    $(childtype) *item;
.if defined (iter_fields)
    $(iter_fields)
.endif
};
</public>

<context>
    $(selfname)_iter_t
        $(selfname)_iter;                   //  Self starts with child object
.if count ? 0
    volatile qbyte
        count;
.endif
</context>

<method name = "iter new" return = "iter">
    <argument name = "item" type = "$(childtype) *"/>
    <declare name = "iter" type = "$(selfname)_iter_t *"/>
    iter = icl_mem_cache_alloc (s_iter_cache);
    iter->next = iter;
    iter->prev = iter;
    iter->head = NULL;
.if link_to_child ? 1
    iter->item = $(childname)_link (item);
.else
    iter->item = item;
.endif
</method>

<method name = "iter object" return = "item">
    <argument name = "iter" type = "$(selfname)_iter_t *"/>
    <declare name = "item" type = "$(childtype) *"/>
.if link_to_child ? 1
    item = $(childname)_link (iter->item);
.else
    item = iter->item;
.endif
</method>

<method name = "iter destroy">
    <argument name = "iter" type = "$(selfname)_iter_t *" ref = "1"/>
    if (iter) {
        $(selfname)_remove (iter);
.if link_to_child ? 1
        $(childname)_unlink (&iter->item);
.endif
        icl_mem_free (iter);
        *iter_p = NULL;
    }
</method>

<method name = "new">
    <header export = "after">
    self->$(selfname)_iter.next = &self->$(selfname)_iter;
    self->$(selfname)_iter.prev = &self->$(selfname)_iter;
    self->$(selfname)_iter.head = self;
    self->$(selfname)_iter.item = NULL;
.if count ? 0
    self->count = 0;
.endif
    </header>
</method>

<method name = "destroy">
    <doc>
    Destroys the list and unlinks all attached items.
    </doc>
    <local>
    $(selfname)_iter_t
        *item;
    </local>
    <header>
    <!-- NB we can't just call _purge because of the lock.                  -->
    item = ($(selfname)_iter_t *) self->$(selfname)_iter.next;
    while (item != &self->$(selfname)_iter) {
        //  Unlink child without lock (we already have the lock)
        $(selfname)_unhook (item);

        $(selfname)_iter_destroy (&item);
        item = ($(selfname)_iter_t *) self->$(selfname)_iter.next;
    }
    </header>
</method>

<method name = "isempty" template = "function">
    <doc>
    Returns true if the list is empty, else returns false.
    </doc>
    <!--inherit name = "rdlock"/-->
        rc = (self->$(selfname)_iter.next == &self->$(selfname)_iter);
</method>

<method name = "push" template = "function">
    <doc>
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    </doc>
    <inherit name = "wrlock"/>
    <inherit name = "mutex"/>
    <argument name = "item" type = "$(childtype) *" />
    <local>
    $(selfname)_iter_t
        *iter;
    </local>
    <header>
        iter = $(selfname)_iter_new (item);
.if count ? 0
        icl_atomic_inc32 (&self->count);
.endif
        $(selfname)_rehook (iter, &self->$(selfname)_iter, ($(selfname)_iter_t *) self->$(selfname)_iter.next);
    </header>
</method>

<method name = "pop" return = "item">
    <doc>
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    </doc>
    <inherit name = "reject zombie"/>
    <inherit name = "wrlock"/>
    <inherit name = "mutex"/>
    <argument name = "self" type = "$(selftype) *">The list</argument>
    <declare name = "item"  type = "$(childtype) *" default = "NULL"/>
    <local>
    $(selfname)_iter_t
        *iter = NULL;
    </local>
    <header>
        if (self->$(selfname)_iter.next != &self->$(selfname)_iter) {
            iter = $(selfname)_unhook (($(selfname)_iter_t *) self->$(selfname)_iter.next);
.if count ? 0
            icl_atomic_dec32 (&self->count);
.endif
            item = iter->item;
            iter->item = NULL;          //  So don't destroy item
            $(selfname)_iter_destroy (&iter);
        }
    </header>
</method>

<method name = "queue" template = "function">
    <doc>
    Attaches the item to the end of the specified list. If the item
    was on another list, it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    </doc>
    <inherit name = "wrlock"/>
    <inherit name = "mutex"/>
    <argument name = "item" type = "$(childtype) *" />
    <local>
    $(selfname)_iter_t
        *iter;
    </local>
    <header>
        iter = $(selfname)_iter_new (item);
.if count ? 0
        icl_atomic_inc32 (&self->count);
.endif
        $(selfname)_rehook (iter, ($(selfname)_iter_t *) self->$(selfname)_iter.prev, &self->$(selfname)_iter);
    </header>
</method>

<method name = "relink before" template = "function">
    <doc>
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    </doc>
    <inherit name = "wrlock"/>
    <inherit name = "mutex"/>
    <argument name = "self" condition = "0">Disable argument 'self'</argument>
    <argument name = "item"  type = "$(selfname)_iter_t *" />
    <argument name = "where" type = "$(selfname)_iter_t *" />
    <declare  name = "self" type = "$(selftype) *" default = "($(selfname)_t *) where->head">The list</declare>
    <header>
        //  If head is non-null then we are moving within the
        //  list to another so don't need to adjust links. Otherwise it
        //  is illegitimate to move from one list to another in one step,
        //  because we don't have a lock on the other list.
        if (! item->head) {
.if count ? 0
            icl_atomic_inc32 (&self->count);
.endif
        }
        else
        if (item->head != self) {
            icl_console_print ("E: $(childname) item inserted into multiple $(prefix) containers");
            assert (item->head == self);
        }
        $(selfname)_unhook (item);
        $(selfname)_rehook (item, ($(selfname)_iter_t *) where->prev, where);
    </header>
</method>

<method name = "remove" return = "rc">
    <doc>
    Removes the $(selfname)_iter item from any list it is in.

    If it was in a list, the number of links is decremented.
    </doc>
    <inherit name = "wrlock"/>
    <inherit name = "mutex"/>
    <argument name = "item"  type = "$(selfname)_iter_t *" />
    <declare  name = "self" type = "$(selftype) *" default = "($(selftype) *) item->head">The list</declare>
    <declare  name = "rc"    type = "int" default = "0" />
    <footer>
    if (self
    &&  self == item->head) { //  Catch case where item moved
        $(selfname)_unhook (item);
.if count ? 0
        icl_atomic_dec32 (&self->count);
.endif
    }
    </footer>
</method>

<method name = "first" return = "item">
    <doc>
    Returns the first $(selfname)_iter item on the list.  If the list is empty,
    returns null.
    </doc>
    <inherit name = "reject zombie"/>
    <inherit name = "rdlock"/>
    <inherit name = "mutex"/>
    <argument name = "self" type = "$(selftype) *">The list</argument>
    <argument name = "file"   type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line"   type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare name = "item" type = "$(selfname)_iter_t *" default = "NULL" />
    <header>
        item = ($(selfname)_iter_t *) self->$(selfname)_iter.next;
        if (item == &self->$(selfname)_iter)
            item = NULL;
    </header>
</method>

<method name = "last" return = "item">
    <doc>
    Returns the last $(selfname)_iter item on the list.  If the list is empty,
    returns null.
    </doc>
    <inherit name = "reject zombie"/>
    <inherit name = "rdlock"/>
    <inherit name = "mutex"/>
    <argument name = "self" type = "$(selftype) *">The list</argument>
    <argument name = "file"   type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line"   type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare name = "item" type = "$(selfname)_iter_t *" default = "NULL" />
    <header>
        item = ($(selfname)_iter_t *) self->$(selfname)_iter.prev;
        if (item == &self->$(selfname)_iter)
            item = NULL;
    </header>
</method>

<method name = "next" return = "next">
    <doc>
    Returns the following item on the $(selfname)_iter list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    </doc>
    <inherit  name = "rdlock"/>
    <inherit name = "mutex"/>
    <argument name = "item" type = "$(selfname)_iter_t *" ref = "1" />
    <argument name = "file"   type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line"   type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "next" type = "$(selfname)_iter_t *" default = "NULL">The item to return</declare>
    <declare  name = "self" type = "$(selftype) *" default = "($(selftype) *) item->head">The list</declare>
    <header>
    if (self
    &&  self == item->head) { //  Catch case where item moved
        next = ($(selfname)_iter_t *) item->next;
        if (next == &self->$(selfname)_iter)
            next = NULL;
    </header>
    <footer>
    }
    </footer>
</method>

<method name = "prev" return = "prev">
    <doc>
    Returns the preceding item on the $(selfname)_iter list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    </doc>
    <inherit  name = "rdlock"/>
    <inherit name = "mutex"/>
    <argument name = "item" type = "$(selfname)_iter_t *" ref = "1" />
    <argument name = "file"   type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line"   type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "prev" type = "$(selfname)_iter_t *" default = "NULL">The item to return</declare>
    <declare  name = "self" type = "$(selftype) *" default = "($(selftype) *) item->head">The list</declare>
    <header>
    if (self
    &&  self == item->head) { //  Catch case where item moved
        prev = ($(selfname)_iter_t *) item->prev;
        if (prev == &self->$(selfname)_iter)
            prev = NULL;
    </header>
    <footer>
    }
    </footer>
</method>

<method name = "count" return = "count" condition = "count ? 0">
    <doc>
    Return the count of the number of objects in the list.
    </doc>
    <argument name = "self" type = "$(selftype) *">The list</argument>
    <declare name = "count" type = "qbyte" />
    //
    $(selfname:upper)_ASSERT_SANE (self);
    icl_atomic_set32 (&count, self->count);
</method>

<method name = "rehook" private = "1" inline = "1">
    <argument name = "item" type = "$(selfname)_iter_t *"/>
    <argument name = "left" type = "$(selfname)_iter_t *"/>
    <argument name = "right" type = "$(selfname)_iter_t *"/>
    <local>
    $(selfname)_iter_t
        *swap;
    </local>
    <footer>
    swap = ($(selfname)_iter_t *) left->next;
    left->next = item->next;
    item->next = swap;

    swap = ($(selfname)_iter_t *) item->prev;
    item->prev = right->prev;
    right->prev = swap;

    item->head = left->head;
    </footer>
</method>

<method name = "unhook" return = "item" private = "1" inline = "1">
    <argument name = "item" type = "$(selfname)_iter_t *"/>
    <footer>
    $(base)rehook (item, ($(selfname)_iter_t *) item->prev, ($(selfname)_iter_t *) item->next);
    item->head = NULL;
    </footer>
</method>

<private name = "header">
static icl_cache_t
    *s_iter_cache = NULL;
</private>

<method name = "cache initialise">
    <doc>
    Initialise the cache and register purge method with the meta-cache.
    </doc>
    s_iter_cache = icl_cache_get (sizeof ($(selfname)_iter_t));
</method>

<method name = "cache purge">
    icl_mem_cache_purge (s_iter_cache);
</method>

</class>
