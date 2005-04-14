<?xml?>
<class
    name      = "amq_subsc"
    comment   = "Topic subscription class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class holds the subscription for a consumer and can optionally
be backed up onto a persistent database using ipr_db_queue serialisation.
</doc>

<inherit class = "amq_mesgq"    />
<inherit class = "ipr_db_queue" />

<import class = "amq_db" />

<data>
    <!-- reference to message in topic -->
</data>

<private>
#include "amq_server_agent.h"
#define TRACE_DISPATCH                  /*  Trace dispatching progress?      */
#undef  TRACE_DISPATCH
</private>

<context>
    ipr_looseref_t
        *subsc_ref;                     /*  Item in vhost subsc list         */
    amq_consumer_list_t
        *consumers;                     /*  Consumers for this subsc         */
    qbyte
        window;                         /*  Total window availability        */
    qbyte
        last_id;                        /*  Last dispatched message          */
    Bool
        dirty;                          /*  Subscription needs dispatching   */
</context>

<method name = "new">
    self->subsc_ref = ipr_looseref_new (vhost->subsc_refs, self);
</method>

<method name = "destroy">
    ipr_looseref_destroy (&self->subsc_ref);
</method>

<method name = "selftest" />

</class>
