<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

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
    name      = "amq_hitset"
    comment   = "Holds a match hitset"
    version   = "1.1"
    script    = "icl_gen"
    >
<doc>
    Holds a hitset, which is the result of a match field name and value.  
    The hitset is an array of all matching indices plus upper/lower 
    limits on this array.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "nullify" value = "0" />
</inherit>

<import class = "amq_server_classes" />
<import class = "amq_index_hash" />

<context>
    int
        lowest,                         //  Lowest found item
        highest;                        //  Highest found item
    int
        hit_count [IPR_INDEX_MAX];      //  Hit count per item
</context>

<method name = "new">
    self->lowest  = IPR_INDEX_MAX;
    self->highest = -1;
</method>

<method name = "collect" template = "function">
    <argument name = "index hash" type = "amq_index_hash_t *">Search this hash</argument>
    <argument name = "index key"  type = "char *">For this term</argument>
    <local>
    amq_index_t
        *index;                         //  Index item                       
    int
        item_nbr;
    </local>
    index = amq_index_hash_search (index_hash, index_key);
    if (index) {
        item_nbr = ipr_bits_first (index->bindset);
        while (item_nbr >= 0) {
            if (amq_server_config_debug_route (amq_server_config))
                smt_log_print (amq_broker->debug_log,
                    "X: route    header=%s binding=%d", index_key, item_nbr);
            if (item_nbr < self->lowest)
                self->lowest = item_nbr;
            //  We don't nullify this object so initialise new hit counts
            while (self->highest < item_nbr)
                self->hit_count [++self->highest] = 0;

            self->hit_count [item_nbr]++;
            item_nbr = ipr_bits_next (index->bindset, item_nbr);
        }
        amq_index_unlink (&index);
    }
</method>

<method name = "selftest">
</method>

</class>

