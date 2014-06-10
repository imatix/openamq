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
    name      = "icl_list_iter"
    comment   = "Creates a list iterator class"
    version   = "1.1"
    script    = "icl_gen"
    abstract  = "1"
    multiple  = "1"
    >
<doc>
    Implements an list iterator class for an arbitrary class.
</doc>

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

<!-- Generate container for container list -->
<invoke script = "icl_iter_list_head"/>

</class>
