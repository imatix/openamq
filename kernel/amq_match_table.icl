<?xml?>
<class
    name      = "amq_match_table"
    comment   = "Implements the hash table container for the matcher class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
This class implements the message matching.
</doc>

<inherit class = "ipr_hash_table" />
<import class = "amq_match" />
<option name = "childname" value = "amq_match" />
<option name = "hash_size" value = "65535" />

<context>
</context>

<method name = "new">
</method>

<method name = "parse topic" template = "function">
    <doc>
    Parse a topic name specifier and derive topic terms from it.  We compare
    the topic name specifier with each existing topic destination and create
    a term for each match.
    </doc>
    <argument name = "dest list" type = "amq_dest_list_t *">Destinations</argument>
    <argument name = "topic exp" type = "char *"        >Topic expression</argument>
    <argument name = "subscr"    type = "amq_subscr_t *">Subscription to register</argument>
    <local>
    ipr_shortstr_t
        pattern;
    amq_dest_t
        *dest;                          /*  Vhost destination                */
    amq_match_t
        *match;                         /*  Match item                       */
    ipr_regexp_t
        *regexp;                        /*  Regular expression object        */
    </local>
    assert (dest_list);
    assert (topic_exp);
    assert (subscr);

    /*  We scan all known topics to see which ones match our criteria        */
    /*  This strategy assumes that topics are declared in advance...         */
    amq_match_topic_re (pattern, topic_exp);
    regexp = ipr_regexp_new (pattern);

    dest = amq_dest_list_first (dest_list);
    while (dest) {
        if (dest->service_type == AMQP_SERVICE_TOPIC) {
            if (ipr_regexp_match (regexp, dest->key, NULL)) {
                match = amq_match_search (self, dest->key);
                if (match == NULL)
                    match = amq_match_new (self, dest->key);

                /*  Flag this subscription as matching                       */
                ipr_bits_set (match->bits, subscr->index);

                /*  Add a reference to the subscription                      */
                ipr_looseref_new (subscr->matches, match);
            }
        }
        dest = amq_dest_list_next (dest_list, dest);
    }
    ipr_regexp_destroy (&regexp);
</method>

<method name = "parse fields" template = "function">
    <doc>
    Parse the selector fields specifier and derive terms from it.  We create
    a term for each field/value combination.  We restrict field names to 30
    bytes and field values to 127 bytes.
    </doc>
    <argument name = "selector" type = "ipr_longstr_t *">Selector table</argument>
    <argument name = "subscr"   type = "amq_subscr_t *" >Subscription to register</argument>
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

    fields = amq_field_list_new (selector);
    field  = amq_field_list_first (fields);
    while (field) {
        amq_match_field_value (match_key, field);
        coprintf ("SUBSCRIBING ON FIELD: %s", match_key);

        match = amq_match_search (self, match_key);
        if (match == NULL)
            match = amq_match_new (self, match_key);

        /*  Flag this subscription as matching                               */
        ipr_bits_set (match->bits, subscr->index);

        /*  Add a reference to the subscription                              */
        ipr_looseref_new (subscr->matches, match);

        field = amq_field_list_next (fields, field);
        subscr->field_count++;
    }
    amq_field_list_destroy (&fields);
</method>

<method name = "selftest" />

</class>
