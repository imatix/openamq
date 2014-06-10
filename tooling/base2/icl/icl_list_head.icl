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

<inherit name = "icl_object"/>

<context>
    $(childtype)
        $(childname);                   //  Self starts with child object
.if count ? 0
    volatile qbyte
        count;
.endif
</context>

<method name = "new">
    <header export = "after">
    self->$(childname).$(prefix)_next = &self->$(childname);
    self->$(childname).$(prefix)_prev = &self->$(childname);
    self->$(childname).$(prefix)_head = self;
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
    $(childtype)
        *item;
    </local>
    <header>
    <!-- NB we can't just call _purge because of the lock.                  -->
    item = ($(childtype) *) self->$(childname).$(prefix)_next;
    while (item != &self->$(childname)) {
        //  Unlink child without lock (we already have the lock)
        $(selfname)_unhook (item);

.if link_to_child ? child_has_links ? 1
        $(childname)_unlink (&item);
.else
        $(childname)_destroy (&item);
.endif
        item = ($(childtype) *) self->$(childname).$(prefix)_next;
    }
    </header>
</method>

<method name = "isempty" template = "function">
    <doc>
    Returns true if the list is empty, else returns false.
    </doc>
    <!--inherit name = "rdlock"/-->
        rc = (self->$(childname).$(prefix)_next == &self->$(childname));
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
    <header>
        //  If $(prefix)_head is non-null then we are moving within the
        //  list to another so don't need to adjust links. Otherwise it
        //  is illegitimate to move from one list to another in one step,
        //  because we don't have a lock on the other list.
        if (! item->$(prefix)_head) {
.if link_to_child ? child_has_links ? 1
            $(childname)_link (item);
.endif
.if count ? 0
.   if (rwlock ? 0) | (mutex ? 0)
            self->count++;
.   else
            icl_atomic_inc32 (&self->count);
.   endif
.endif
        }
        else
        if (item->$(prefix)_head != self) {
            icl_console_print ("E: $(childname) item inserted into multiple $(prefix) containers");
            assert (item->$(prefix)_head == self);
        }
        $(selfname)_unhook (item);
        $(selfname)_rehook (item, &self->$(childname), ($(childtype) *) self->$(childname).$(prefix)_next);
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
    <header>
        if (self->$(childname).$(prefix)_next == &self->$(childname))
            item = NULL;
        else {
            item = $(selfname)_unhook (($(childtype) *) self->$(childname).$(prefix)_next);
.
.#  If we are linking to the child then links are unchanged, but if we aren't
.#  then this method is creating a new link.
.if (child_has_links ? 1) & ! (link_to_child ? 1)
            $(childname)_link (item);
.endif
.
.if count ? 0
.   if (rwlock ? 0) | (mutex ? 0)
            self->count--;
.   else
            icl_atomic_dec32 (&self->count);
.   endif
.endif
        }
    </header>
</method>

<method name = "queue" template = "function">
    <doc>
    Attaches the item to the end of the specified list.  The item may not
    be in another list of the same class.

    The number of links is automatically adjusted - if the item was already
    in the list, it remains unchanged; otherwise it is incremented.
    </doc>
    <inherit name = "wrlock"/>
    <inherit name = "mutex"/>
    <argument name = "item" type = "$(childtype) *" />
    <header>
        //  If $(prefix)_head is non-null then we are moving within the
        //  list to another so don't need to adjust links. Otherwise it
        //  is illegitimate to move from one list to another in one step,
        //  because we don't have a lock on the other list.
        if (! item->$(prefix)_head) {
.if link_to_child ? child_has_links ? 1
            $(childname)_link (item);
.endif
.if count ? 0
.   if (rwlock ? 0) | (mutex ? 0)
            self->count++;
.   else
            icl_atomic_inc32 (&self->count);
.   endif
.endif
        }
        else
        if (item->$(prefix)_head != self) {
            icl_console_print ("E: $(childname) item inserted into multiple $(prefix) containers");
            assert (item->$(prefix)_head == self);
        }
        $(selfname)_unhook (item);
        $(selfname)_rehook (item, ($(childtype) *) self->$(childname).$(prefix)_prev, &self->$(childname));
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
    <argument name = "item"  type = "$(childtype) *" />
    <argument name = "where" type = "$(childtype) *" />
    <declare  name = "self" type = "$(selftype) *" default = "($(selftype) *) where->$(prefix)_head">The list</declare>
    <header>
        //  If $(prefix)_head is non-null then we are moving within the
        //  list to another so don't need to adjust links. Otherwise it
        //  is illegitimate to move from one list to another in one step,
        //  because we don't have a lock on the other list.
        if (! item->$(prefix)_head) {
.if link_to_child ? child_has_links ? 1
            $(childname)_link (item);
.endif
.if count ? 0
.   if (rwlock ? 0) | (mutex ? 0)
            self->count++;
.   else
            icl_atomic_inc32 (&self->count);
.   endif
.endif
        }
        else
        if (item->$(prefix)_head != self) {
            icl_console_print ("E: $(childname) item inserted into multiple $(prefix) containers");
            assert (item->$(prefix)_head == self);
        }
        $(selfname)_unhook (item);
        $(selfname)_rehook (item, ($(childtype) *) where->$(prefix)_prev, where);
    </header>
</method>

<method name = "remove" return = "rc">
    <doc>
    Removes the item from any list it is in.

    If it was in a list, the number of links is decremented.
    </doc>
    <inherit name = "wrlock"/>
    <inherit name = "mutex"/>
    <argument name = "item"  type = "$(childtype) *" />
    <declare  name = "self" type = "$(selftype) *" default = "($(selftype) *) item->$(prefix)_head">The list</declare>
    <declare  name = "rc"    type = "int" default = "0" />
    <footer>
    if (self
    &&  self == item->$(prefix)_head) { //  Catch case where item moved
        $(selfname)_unhook (item);
.if link_to_child ? child_has_links ? 1
        $(childname)_unlink (&item);
.endif
.if count ? 0
.   if (rwlock ? 0) | (mutex ? 0)
        self->count--;
.   else
        icl_atomic_dec32 (&self->count);
.   endif
.endif
    }
    </footer>
</method>

<method name = "first" return = "item">
    <doc>
    Returns the first $(childname) item on the list.  If the list is empty,
    returns null.
    </doc>
    <inherit name = "reject zombie"/>
    <inherit name = "rdlock"/>
    <inherit name = "mutex"/>
    <argument name = "self" type = "$(selftype) *">The list</argument>
    <argument name = "file"   type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line"   type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare name = "item" type = "$(childtype) *" default = "NULL" />
    <header>
        item = ($(childtype) *) self->$(childname).$(prefix)_next;
        if (item == &self->$(childname))
            item = NULL;
.if child_has_links ? 1
        else
            $(childname)_link (item);
.endif
    </header>
</method>

<method name = "last" return = "item">
    <doc>
    Returns the last $(childname) item on the list.  If the list is empty,
    returns null.
    </doc>
    <inherit name = "reject zombie"/>
    <inherit name = "rdlock"/>
    <inherit name = "mutex"/>
    <argument name = "self" type = "$(selftype) *">The list</argument>
    <argument name = "file"   type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line"   type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare name = "item" type = "$(childtype) *" default = "NULL" />
    <header>
        item = ($(childtype) *) self->$(childname).$(prefix)_prev;
        if (item == &self->$(childname))
            item = NULL;
.if child_has_links ? 1
        else
            $(childname)_link (item);
.endif
    </header>
</method>

<method name = "next" return = "next">
    <doc>
    Returns the following item on the $(childname) list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    </doc>
    <inherit  name = "rdlock"/>
    <inherit name = "mutex"/>
    <argument name = "item" type = "$(childtype) *" ref = "1" />
    <argument name = "file"   type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line"   type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "next" type = "$(childtype) *" default = "NULL">The item to return</declare>
    <declare  name = "self" type = "$(selftype) *" default = "($(selftype) *) item->$(prefix)_head">The list</declare>
    <header>
    if (self
    &&  self == item->$(prefix)_head) { //  Catch case where item moved
        next = ($(childtype) *) item->$(prefix)_next;
        if (next == &self->$(childname))
            next = NULL;
.if child_has_links ? 1
        else
            $(childname)_link (next);
.endif
    </header>
    <footer>
    }
.if child_has_links ? 1
    $(childname)_unlink (item_p);
.endif
    </footer>
</method>

<method name = "prev" return = "prev">
    <doc>
    Returns the preceding item on the $(childname) list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    </doc>
    <inherit  name = "rdlock"/>
    <inherit name = "mutex"/>
    <argument name = "item" type = "$(childtype) *" ref = "1" />
    <argument name = "file"   type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line"   type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "prev" type = "$(childtype) *" default = "NULL">The item to return</declare>
    <declare  name = "self" type = "$(selftype) *" default = "($(selftype) *) item->$(prefix)_head">The list</declare>
    <header>
    if (self
    &&  self == item->$(prefix)_head) { //  Catch case where item moved
        prev = ($(childtype) *) item->$(prefix)_prev;
        if (prev == &self->$(childname))
            prev = NULL;
.if child_has_links ? 1
        else
            $(childname)_link (prev);
.endif
    </header>
    <footer>
    }
.if child_has_links ? 1
    $(childname)_unlink (item_p);
.endif
    </footer>
</method>

<method name = "count" return = "count" condition = "count ? 0">
    <doc>
    Return the count of the number of objects in the list.
    </doc>
    <argument name = "self" type = "$(selftype) *">The list</argument>
    <declare name = "count" type = "qbyte" />
    $(selfname:upper)_ASSERT_SANE (self);
.   if (rwlock ? 0) | (mutex ? 0)
    count = self->count;
.   else
    count = icl_atomic_get32 (&self->count);
.   endif
</method>

<method name = "rehook" private = "1" inline = "1">
    <argument name = "item" type = "$(childtype) *"/>
    <argument name = "left" type = "$(childtype) *"/>
    <argument name = "right" type = "$(childtype) *"/>
    <local>
    $(childtype)
        *swap;
    </local>
    <footer>
    swap = ($(childtype) *) left->$(prefix)_next;
    left->$(prefix)_next = item->$(prefix)_next;
    item->$(prefix)_next = swap;

    swap = ($(childtype) *) item->$(prefix)_prev;
    item->$(prefix)_prev = right->$(prefix)_prev;
    right->$(prefix)_prev = swap;

    item->$(prefix)_head = left->$(prefix)_head;
    </footer>
</method>

<method name = "unhook" return = "item" private = "1" inline = "1">
    <argument name = "item" type = "$(childtype) *"/>
    <footer>
    $(base)rehook (item, ($(childtype) *) item->$(prefix)_prev, ($(childtype) *) item->$(prefix)_next);
    item->$(prefix)_head = NULL;
    </footer>
</method>

</class>
