<?xml?>
<class
    name      = "amq_match"
    comment   = "Matcher class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements the message matching item.  Each entry in the
match table is a search term along with the set of subscriptions that
have requested it.  The actual matching logic is in amq_match_table.
</doc>

<inherit class = "ipr_hash_str" />
<option name = "bigtable" value = "1" />

<import class = "amq_global" />
<import class = "amq_db" />

<public name = "header">
#include "amq_core.h"
</public>

<context>
    ipr_bits_t
        *bits;                          /*  Set of subscriptions for term    */
</context>

<method name = "new">
    self->bits = ipr_bits_new ();
</method>

<method name = "destroy">
    ipr_bits_destroy (&self->bits);
</method>

<method name = "selftest" />

</class>
