<?xml?>
<class
    name      = "amq_hitset"
    comment   = "Holds a match hitset"
    version   = "1.1"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
    Holds a hitset, which is the result of a match field name and
    value.  The hitset is an array of all matching indices plus
    upper/lower limits on this array.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "nullify" value = "0" />
</inherit>

<public name = "include">
#include "amq_server_classes.h"
</public>

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
            if (amq_server_config_trace_route (amq_server_config))
                icl_console_print ("X: route    header=%s binding=%d", index_key, item_nbr);
            if (item_nbr < self->lowest)
                self->lowest = item_nbr;
            if (item_nbr > self->highest) {
                //  We don't nullify this object so initialise new hit_count
                self->hit_count [item_nbr] = 0;
                self->highest = item_nbr;
            }
            self->hit_count [item_nbr]++;
            item_nbr = ipr_bits_next (index->bindset);
        }
        amq_index_unlink (&index);
    }
</method>

<method name = "selftest">
</method>

</class>

