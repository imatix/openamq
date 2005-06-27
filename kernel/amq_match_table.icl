<?xml?>
<class
    name      = "amq_match_table"
    comment   = "Implements the hash table container for the matcher class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
This class is a hash table container for the match class.  This class
works together with the hitset class to perform the matching on topic
names and field values.
</doc>

<inherit class = "ipr_hash_head" />
<import class = "amq_match" />
<option name = "childname"  value = "amq_match" />
<option name = "childtype"  value = "amq_match_t" />
<!-- This limits the number of topics we can handle to 100k or so -->
<option name = "rwlock"     value = "1" />
<option name = "hash_size"  value = "150000" />
<option name = "hash_type"  value = "str"/>

<import class = "ipr_shortstr"/>

<context>
    amq_topic_array_t
        *topics;
</context>

<method name = "new">
    self->topics = amq_topic_array_new ();
</method>

<method name = "destroy">
    amq_topic_array_destroy (&self->topics);
</method>

<method name = "parse topic" template = "function">
    <doc>
    Parse a topic name specifier and derive topic terms from it.  We compare
    the topic name specifier with each existing topic destination and create
    a term for each match.
    </doc>
    <argument name = "subscr" type = "amq_subscr_t *">Subscription to register</argument>
    <local>
    amq_topic_t
        *topic;                         /*  Vhost topic                      */
    amq_match_t
        *match;                         /*  Match item                       */
    ipr_regexp_t
        *regexp;                        /*  Regular expression object        */
    int
        topic_nbr;                      /*  Topic index                      */
    </local>
    assert (subscr);
    assert (self->topics);

    /*  We scan all known topics to see which ones match our criteria        */
    regexp = ipr_regexp_new (subscr->topic_re);

    for (topic_nbr = 0; topic_nbr < self->topics->bound; topic_nbr++) {
        topic = amq_topic_array_fetch (self->topics, topic_nbr);
        assert (topic);
        if (ipr_regexp_match (regexp, topic->dest_name, NULL)) {
            /*  Topic must be defined in match table                         */
            match = amq_match_table_search (self, topic->dest_name);
            assert (match);

            /*  Flag this subscription as matching                           */
            ipr_bits_set (match->bits, subscr->index);

            /*  Add a reference to the subscription                          */
            ipr_looseref_queue (subscr->matches, match);
            
            /*  Remove surplus link to match                                 */
            amq_match_unlink (&match);
        }
    }
    ipr_regexp_destroy (&regexp);
</method>

<method name = "parse fields" template = "function">
    <doc>
    Parse the selector fields specifier and derive terms from it.  We create
    a term for each field/value combination.  We restrict field names to 30
    bytes and field values to 127 bytes.
    </doc>
    <argument name = "subscr" type = "amq_subscr_t *" >Subscription to register</argument>
    <local>
    amq_field_list_t
        *fields;                        /*  Selector fields                  */
    amq_field_t
        *field;                         /*  Next field to examine            */
    amq_match_t
        *match;                         /*  Match item                       */
    ipr_shortstr_t
        match_key;
    </local>

#   define FIELD_NAME_MAX   30          /*  Arbitrary limit to get           */
#   define FIELD_VALUE_MAX  30          /*    name=value into shortstr       */

    fields = amq_field_list_new (subscr->selector);
    if (fields) {
        field = amq_field_list_first (fields);
        while (field) {
            amq_match_field_value (match_key, field);
            coprintf ("SUBSCRIBING ON FIELD: %s, subscriber=%d", match_key, subscr->index);

            match = amq_match_table_search (self, match_key);
            if (match == NULL)
                match = amq_match_new (self, match_key);

            /*  Flag this subscription as matching                               */
            ipr_bits_set (match->bits, subscr->index);

            /*  Add a reference to the subscription                              */
            ipr_looseref_queue (subscr->matches, match);

            field = amq_field_list_next (field);
            subscr->field_count++;
            
            amq_match_unlink (&match);
        }
        amq_field_list_destroy (&fields);
    }
    else
        amq_global_set_error (AMQP_SYNTAX_ERROR, "Invalid selector field table");
</method>

<method name = "check topic" template = "function">
    <doc>
    Checks whether the specified topic name has already been seen by
    susbcribers, and if not, rebuilds a match table entry for the
    topic, for all subscribers.  This method is designed to allow the
    topic match table to be constructed dynamically as publishers use
    new topic names.
    </doc>
    <argument name = "dest name"   type = "char *">Topic destination</argument>
    <argument name = "subscr_list" type = "amq_subscr_list_t *">Current subscribers</argument>
    <local>
    amq_match_t
        *match;                         /*  Match item                       */
    amq_topic_t
        *topic;                         /*  Topic item                       */
    amq_subscr_t
        *subscr;                        /*  Subscription object              */
    ipr_regexp_t
        *regexp;                        /*  Regular expression object        */
    </local>
    assert (self->topics);

    match = amq_match_table_search (self, dest_name);
    if (match == NULL) {
        icl_console_print ("NEW TOPIC (%s), REBUILDING SUBSCRIPTIONS", dest_name);
        match = amq_match_new (self, dest_name);
        topic = amq_topic_new (self->topics, self->topics->bound, dest_name);

        /*  Recompile all subscriptions for this topic                       */
        subscr = amq_subscr_list_first (subscr_list);
        while (subscr) {
            regexp = ipr_regexp_new (subscr->topic_re);
            if (ipr_regexp_match (regexp, dest_name, NULL)) {
                /*  Flag this subscription as matching                       */
                ipr_bits_set (match->bits, subscr->index);

                /*  Add a reference to the subscription                      */
                ipr_looseref_queue (subscr->matches, match);
            }
            ipr_regexp_destroy (&regexp);
            subscr = amq_subscr_list_next (subscr);
        }
        amq_topic_unlink (&topic);
        amq_match_unlink (&match);
    }
    
</method>

<method name = "selftest"/>

</class>
