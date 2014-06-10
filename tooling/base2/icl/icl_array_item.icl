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
    name      = "icl_array_item"
    comment   = "Implements a dynamic array structure"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    multiple  = "1"
    >
<doc>
    Implements a dynamic array structure
</doc>

<option name = "container" value = "1"/>

<invoke>
.#  Make <option name = "prefix" value = "array" /> if not already defined
.if ! defined (icl->option (name="prefix"))
.   new icl.option
.       option.name = "prefix"
.       option.value = "array"
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
    
<!-- Generate container for array if necessary-->
<invoke script = "icl_array_head" />

<context>
    $(selfname)_$(prefix)_t
        *$(prefix)_head;                //  Array container                    
    qbyte
        $(prefix)_index;                //  Own index into array
</context>

<method name = "new">
    <argument name = "array" type = "$(selfname)_$(prefix)_t *">Array to insert into</argument>
    <argument name = "index" type = "qbyte">Index for new item</argument>
    <doc>
    Initialises a new array item without placing it into any array.
    </doc>
    <header export = "after">
    self->$(prefix)_index = 0;
    self->$(prefix)_head = NULL;
    </header>
    <footer export = "before">
    $(selfname)_$(prefix)_insert (array, index, self);
    </footer>
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

<method name = "show">
    <header>
     if (self->$(prefix)_head)
        container_links++;
    </header>
</method>

</class>
