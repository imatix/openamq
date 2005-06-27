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

<inherit class = "icl_object">
    <option name = "cache"  value = "1" />
    <option name = "rwlock" value = "1" />
</inherit>
<inherit class = "ipr_list_item" />
<import class = "amq_global" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<context>
    // TODO list of consumers, not just one
    //      in handle, find existing subscr by comparing selector & topic_exp
    //      only create new subscr if necessary...
    // TODO: per vhost, number of each class of object
    
    /*  References to parent objects                                         */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */
    amq_consumer_t
        *consumer;                      /*  Consumer for this subscription   */

    ipr_shortstr_t
        topic_re;                       /*  Topic regular expression         */
    ipr_longstr_t
        *selector;                      /*  Original selector for subscr     */
    int
        index;                          /*  Index in vhost->subscr_index     */
    ipr_looseref_list_t
        *matches;                       /*  Matches for this subscription    */
    int
        field_count;                    /*  How many fields wanted?          */
</context>

<method name = "new">
    <doc>
    Creates a new subscription as specified.
    </doc>
    <argument name = "consumer" type = "amq_consumer_t *"      >Consumer object</argument>
    <argument name = "command"  type = "amq_handle_consume_t *">Passed command</argument>

    self->consumer = consumer;
    self->vhost    = consumer->handle->vhost;
    self->index    = ipr_index_insert (self->vhost->subscr_index, self);
    self->matches  = ipr_looseref_list_new ();
    self->selector = ipr_longstr_new (command->selector->data, command->selector->cur_size);

    /*  Take topic selector regexp from command dest_name                    */
    amq_match_topic_re (self->topic_re, command->dest_name);
    icl_console_print ("NEW SUBSCRIPTION: dest=%s", command->dest_name);

    /*  Parse the topic name into the match table                            */
    amq_match_table_parse_topic (
        self->vhost->match_topics,      /*  Match table to populate          */
        self);                          /*  Subscription to register         */

    /*  Parse the field specifiers into the match table                      */
    amq_match_table_parse_fields (
        self->vhost->match_fields,      /*  Match table to populate          */
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
        match_ref = ipr_looseref_list_next (match_ref);
    }
    ipr_index_delete (self->vhost->subscr_index, self->index);
    ipr_looseref_list_destroy (&self->matches);
    ipr_longstr_destroy       (&self->selector);
</method>

<method name = "selftest">
</method>

</class>
