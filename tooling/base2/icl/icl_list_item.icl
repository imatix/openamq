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
    name      = "icl_list_item"
    comment   = "Implements a doubly-linked list structure"
    version   = "1.1"
    script    = "icl_gen"
    abstract  = "1"
    multiple  = "1"
    >
<doc>
    Implements a doubly-linked list structure
</doc>

<option name = "container" value = "1"/>

<invoke>
.#  Make <option name = "prefix" value = "list" /> if not already defined
.if ! defined (icl->option (name="prefix"))
.   new icl.option
.       option.name = "prefix"
.       option.value = "list"
.       preproc_option (option)
.   endnew
.endif
.
.#  Give object links unless explicitly requested otherwise
.if !defined (links)
.   new icl.option
.       option.name = "links"
.       option.value = "1"
.       preproc_option (option)
.   endnew
.endif
</invoke>
    
<!-- Generate container for list -->
<invoke script = "icl_list_head"/>

<!-- Get list management functions -->
<context>
    volatile $(selfname)_$(prefix)_t *$(prefix)_head;
    volatile $(selftype) *$(prefix)_prev;
    volatile $(selftype) *$(prefix)_next;
</context>

<method name = "new">
    <doc>
    Initialises the new item to be a valid list item.
    </doc>
    <header export = "after">
    self->$(prefix)_next = self;
    self->$(prefix)_prev = self;
    self->$(prefix)_head = NULL;
    </header>
</method>

<method name = "remove from all containers">
    <argument name = "self" type = "$(selftype) *">The item</argument>
    <doc>
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    </doc>
    <header>
    $(selfname)_$(prefix)_remove (self);
    </header>
</method>

<method name = "show" condition = "alloc ? &quot;plain&quot; <> &quot;direct&quot;">
    <header>
     if (self->$(prefix)_head)
        container_links++;
    </header>
</method>

</class>
