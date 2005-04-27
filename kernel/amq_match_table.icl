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
<option name = "bigtable"  value = "1" />

<context>
</context>

<method name = "new">
</method>

<private name = "header">
#define S_WILDCARD_SINGLE     "[a-zA-Z0-9]+"
#define S_WILDCARD_MULTIPLE   "[a-zA-Z0-9]+(`.[a-zA-Z0-9]+)*"
</private>

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
        pattern;
    char
        *from_ptr,
        *to_ptr;
    amq_dest_t
        *dest;                          /*  Vhost destination                */
    amq_match_t
        *match;                         /*  Match item                       */
    </local>
    assert (dest_list);
    assert (dest_name);
    assert (subscr);
    
    /*  We want a regexp starting with ^, ending with $, and with the
        * and # topic wildcards replaced by appropriate regexp chars.
        We also filter out any non-alphanum characters.  We may allow
        full RE matching on topic names at a later stage.
     */
    to_ptr = pattern;
    *to_ptr++ = '^';                    /*  Match start of topic name        */
    for (from_ptr = dest_name; *from_ptr; from_ptr++) {
        if (isalnum (*from_ptr))
            *to_ptr++ = *from_ptr;
        else
        if (*from_ptr == '.') {
            *to_ptr++ = '`';
            *to_ptr++ = '.';
        }
        else
        if (*from_ptr == '*') {
            strcpy (to_ptr, S_WILDCARD_SINGLE);
            to_ptr += strlen (S_WILDCARD_SINGLE);
        }
        else
        if (*from_ptr == '#') {
            strcpy (to_ptr, S_WILDCARD_MULTIPLE);
            to_ptr += strlen (S_WILDCARD_MULTIPLE);
        }
    }
    *to_ptr++ = '$';                    /*  Match end of topic name          */
    *to_ptr++ = 0;
    
    dest = amq_dest_list_first (dest_list);
    while (dest) {
        if (dest->service_type == AMQP_SERVICE_TOPIC) {
            if (ipr_regexp_parse (dest->key, pattern, NULL)) {

                coprintf ("$(selfname): pattern=%s topic=%s", dest_name, dest->key);
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

<method name = "selftest">
    <local>
    </local>
</method>

</class>
