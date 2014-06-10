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
    name      = "icl_stack_item"
    comment   = "Implements a doubly-linked list structure"
    version   = "1.1"
    script    = "icl_gen"
    abstract  = "1"
    multiple  = "1"
    >
<doc>
    Implements a singly-linked stack with atomic thread-safe operations.
</doc>

<option name = "container" value = "1"/>

<invoke>
.#  Make <option name = "prefix" value = "stack" /> if not already defined
.if ! defined (icl->option (name="prefix"))
.   new icl.option
.       option.name = "prefix"
.       option.value = "stack"
.       preproc_option (option)
.   endnew
.endif
.#  Make <option name = "links" value = "1" /> if not already defined
.if ! defined (icl->option (name="links"))
.   new icl.option
.       option.name = "links"
.       option.value = "$(links?1)"
.       preproc_option (option)
.   endnew
.endif
</invoke>
    
<!-- Generate container for list -->
<invoke script = "icl_stack_head"/>

<!-- Get list management functions -->
<context>
    volatile $(selfname)_$(prefix)_t *$(prefix)_head;
    volatile $(selftype) *$(prefix)_next;
</context>

<method name = "new">
    <doc>
    Initialises the new item to be in no stack.
    </doc>
    <header export = "after">
    self->$(prefix)_next = NULL;
    self->$(prefix)_head = NULL;
    </header>
</method>

<method name = "remove from all containers">
    <argument name = "self" type = "volatile $(selftype) *">The item</argument>
    <doc>
    This is just a stub.  Objects cannot be removed from a stack except by
    being popped.
    </doc>
</method>

<method name = "show">
    <header>
     if (self->$(prefix)_head)
        container_links++;
    </header>
</method>

</class>
