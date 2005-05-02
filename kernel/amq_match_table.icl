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
    <argument name = "dest name" type = "char *"        >Topic name specifier</argument>
    <argument name = "subscr"    type = "amq_subscr_t *">Subscription to register</argument>
    <local>
    ipr_shortstr_t
        regexp;
    amq_dest_t
        *dest;                          /*  Vhost destination                */
    amq_match_t
        *match;                         /*  Match item                       */
    </local>
    assert (dest_list);
    assert (dest_name);
    assert (subscr);

    amq_match_topic (regexp, dest_name);
    dest = amq_dest_list_first (dest_list);
    while (dest) {
        if (dest->service_type == AMQP_SERVICE_TOPIC) {
            if (ipr_regexp_parse (dest->key, regexp, NULL)) {
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
</method>

<method name = "selftest" />

</class>
