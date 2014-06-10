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
    name      = "icl_tree_item"
    comment   = "Implements a dynamic tree structure"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    multiple  = "1"
    >
<doc>
    Implements a dynamic tree structure
</doc>

<!--option name = "container" value = "1"/-->

<invoke>
.#  Make <option name = "prefix" value = "tree" /> if not already defined
.if ! defined (icl->option (name="prefix"))
.   new icl.option
.       option.name = "prefix"
.       option.value = "tree"
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
    
<!-- Generate container for tree if necessary-->
<invoke script = "icl_tree_head" />

<context>
    $(selftype)
        *$(prefix)_left,
        *$(prefix)_right,
        *$(prefix)_parent;
    $(selfname)_$(prefix)_colour
        $(prefix)_colour;
    $(selfname)_$(prefix)_t
        *$(prefix)_head;
</context>

<method name = "new">
    self->$(prefix)_left   = self;
    self->$(prefix)_right  = self;
    self->$(prefix)_parent = NULL;
    self->$(prefix)_colour = RED;
</method>

<method name = "remove from all containers">
    <argument name = "self" type = "volatile $(selftype) *">The item</argument>
    <doc>
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    </doc>
    //  Not implemented!!
</method>

<method name = "show">
    <header>
     if (self->$(prefix)_head)
        container_links++;
    </header>
</method>

</class>
