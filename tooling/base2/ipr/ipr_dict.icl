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
    name      = "ipr_dict"
    comment   = "A simple hashed dictionary"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
Implements a symbol-table dictionary.  The dictionary holds string
values indexed by string keys.  The string values are duplicated.
All access to the dictionary is unlocked, and dictionary items do
not use links, so the dictionary should not be used from multiple
threads in parallel.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "links" value = "0" />
</inherit>
<!-- Dictionary organised as random-access hash table -->
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>
<!-- Dictionary organised as serial-access list -->
<inherit class = "icl_list_item">
    <option name = "count" value = "1" />
</inherit>

<import class = "ipr_regexp" />

<context>
    char
        *value;                         //  Text value for item
</context>

<method name = "new">
    <argument name = "value" type = "char *" />
    self->value = icl_mem_strdup (value);
</method>

<method name = "destroy">
    icl_mem_free (self->value);
</method>

<method name = "update" template = "function">
    <argument name = "value" type = "char *" />
    icl_mem_free (self->value);
    self->value = icl_mem_strdup (value);
</method>

<method name = "assume" return = "rc">
    <doc>
    Create or update symbol with specified value.  Returns TRUE if the symbol
    was created, FALSE if it already existed.
    </doc>
    <argument name = "table" type = "ipr_dict_table_t *">Symbol table</argument>
    <argument name = "name"  type = "char *">Symbol name</argument>
    <argument name = "value" type = "char *">Symbol value</argument>
    <declare name = "rc" type = "Bool" default = "FALSE" />
    <local>
    ipr_dict_t
        *item;                          //  Inserted item
    </local>
    item = ipr_dict_table_search (table, name);
    if (item) {
        if (value && strneq (value, item->value))
            ipr_dict_update (item, value);
    }
    else {
        ipr_dict_new (table, name, value);
        rc = TRUE;
    }
</method>

<method name = "assume_fmt">
    <doc>
    Create or update symbol with formatted value. Allows printf-style
    formatting to be done on a value prior to inserting it into the
    symbol table.  That the formatted value may not be larger than a
    shortstr, or it will be truncated.
    </doc>
    <argument name = "table"  type = "ipr_dict_table_t *">Symbol table</argument>
    <argument name = "name"   type = "char *">Symbol name</argument>
    <argument name = "format" type = "char *">Format specifier</argument>
    <argument name = "args"   type = "...">Variable arguments</argument>
    <local>
    icl_shortstr_t
        value;
    </local>
    //
    apr_vsnprintf (value, ICL_SHORTSTR_MAX, format, args);
    self_assume (table, name, value);
</method>

<method name = "selftest">
    //  selftest is in ipr_dict_table.icl
</method>

</class>
