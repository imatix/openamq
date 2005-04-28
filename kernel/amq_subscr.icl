<?xml?>
<class
    name      = "amq_subscr"
    comment   = "Subscription class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
Defines a subscription class.  Each subscription maps to a unique consumer
which maps to a queue and destination of type AMQP_SERVICE_SUBSCR.  The
subscription currently implements topic name hierarchies, i.e. matching
on the topic name with the * and # wildcards (meaning a single and multiple
levels respectively).
</doc>

<inherit class = "ipr_list_item" />
<import class = "amq_global" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<context>
    /*  References to parent objects                                         */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */
    amq_consumer_t
        *consumer;                      /*  Consumer for this subscription   */
    qbyte
        client_id;                      /*  Parent client record             */
    ipr_shortstr_t
        dest_name;                      /*  Topic name pattern               */
    Bool
        no_local;                       /*  Don't deliver own msgs to self   */
    int
        index;                          /*  Index in vhost->subscr_index     */
    ipr_looseref_list_t
        *matches;                       /*  Matches for this subscription    */
</context>

<method name = "new">
    <doc>
    Creates a new subscription as specified.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *"      >Consumer object</argument>
    <argument name = "command"  type = "amq_handle_consume_t *">Passed command</argument>

    self->consumer  = consumer;
    self->vhost     = consumer->handle->vhost;
    self->client_id = consumer->handle->client_id;
    self->no_local  = consumer->no_local;
    self->index     = ipr_index_insert (self->vhost->subscr_index, self);
    self->matches   = ipr_looseref_list_new ();

    /*  Topic name is formed by pasting prefix and suffix                    */
    ipr_shortstr_fmt (self->dest_name,
        "%s%s", consumer->handle->dest_name, command->dest_name);

    /*  Parse the topic name into the match table                            */
    amq_match_table_parse_topic (
        self->vhost->match_topics,      /*  Match table to populate          */
        self->vhost->dest_list,         /*  List of known destinations       */
        self->dest_name,                /*  This destination specifier       */
        self);                          /*  Subscription to register         */

    /*  Attach subscription to vhost list                                    */
    amq_subscr_list_queue (self->vhost->subscr_list, self);
</method>

<method name = "destroy">
   <local>
    ipr_looseref_t
        *match_ref;                     /*  Match table entry                */
    </local>

    /*  Invalidate any matches for this subscription                         */
    match_ref = ipr_looseref_list_first (self->matches);
    while (match_ref) {
        ipr_bits_clear (((amq_match_t *) match_ref->object)->bits, self->index);
        match_ref = ipr_looseref_list_next (self->matches, match_ref);
    }
    ipr_index_delete (self->vhost->subscr_index, self->index);
    ipr_looseref_list_destroy (&self->matches);
</method>

<method name = "selftest">
</method>

</class>
