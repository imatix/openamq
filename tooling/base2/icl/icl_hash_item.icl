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
    name      = "icl_hash_item"
    comment   = "Hash abstract base class definition"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    multiple  = "1"
    >
<doc>
    Base class for hash objects.
</doc>

<option name = "container" value = "1"/>

<invoke>
.#  Make <option name = "prefix" value = "table" /> if not already defined
.if ! defined (icl->option (name="prefix"))
.   new icl.option
.       option.name = "prefix"
.       option.value = "table"
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

<!-- Generate container for hash table if necessary -->
<invoke script = "icl_hash_head" />

<context>
    $(selfname)_$(prefix)_t
        *$(prefix)_head;                //  Hash table container
    $(selftype)
        *$(prefix)_next;                //  Next item in the hash slot
    qbyte
        $(prefix)_index;                //  Index of item in table
    $(key_decl_t)
        key;                            //  Item's original key
</context>

<method name = "new">
    <argument name = "table" type = "$(selfname)_$(prefix)_t *" >Table to insert into</argument>
    <argument name = "key"   type = "$(key_arg_t)">Hash key</argument>
    <doc>
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    </doc>
    <header export = "after">
    self->$(prefix)_head = NULL;
    self->$(prefix)_index = 0;          //  Will be set by container
    </header>
    <footer export = "before">
    if (table && self && $(selfname)_$(prefix)_insert (table, key, self))
        $(selfname)_destroy (&self);
    </footer>
</method>

<method name = "remove from all containers">
    <argument name = "self" type = "$(selftype) *">The item</argument>
    <doc>
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    </doc>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
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
