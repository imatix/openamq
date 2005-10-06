<?xml?>
<class
    name      = "amq_exchange_topic"
    comment   = "topic exchange class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements the topic exchange, which routes messages
based on their "routing_key" property matched against a wild-card
topic tree specification.
</doc>

<inherit class = "amq_exchange_base" />

<import class = "amq_hash" />

<context>
    amq_index_hash_t
        *index_hash;                    //  Access by text key
    amq_index_array_t
        *index_array;                   //  Access by number 0..n-1
</context>

<method name = "new">
    self->index_hash  = amq_index_hash_new  ();
    self->index_array = amq_index_array_new ();
</method>

<method name = "destroy">
    amq_index_hash_destroy  (&self->index_hash);
    amq_index_array_destroy (&self->index_array);
</method>

<method name = "compile">
    <doc>
    Compiles a topic binding.  The arguments must contain a field
    called "routing_key", who's value is a routing_key wildcard pattern
    that conforms to the AMQP specifications for topic matching.  This
    means:
    
      - multiple levels separated by '.', e.g. "nasdaq.ibm.usd"
      - single level wildcard represented by '*', e.g. "nasdaq.*.usd"
      - multiple level wildcard represented by '#', e.g. "#.usd"
    </doc>
    <local>
    asl_field_list_t
        *fields;                        //  Decoded arguments
    asl_field_t
        *routing_key;                   //  Routing key value
    ipr_regexp_t
        *regexp;                        //  Regular expression object
    uint
        index_nbr;                      //  Index number in self->index_array
    amq_index_t
        *index;                         //  Index reference from index_array
    </local>
    //
    fields = asl_field_list_new (binding->arguments);
    if (fields) {
        routing_key = asl_field_list_search (fields, "routing_key");
        if (routing_key) {
            //  Turn the routing_key string into a nice regexp
            icl_shortstr_cpy (binding->routing_key, asl_field_string (routing_key));
            s_topic_to_regexp (asl_field_string (routing_key), binding->regexp);
            regexp = ipr_regexp_new (binding->regexp);

            if (amq_server_config_trace_route (amq_server_config))
                icl_console_print ("X: reindex  wildcard=%s", binding->routing_key);

            //  We scan all indices to see which ones match our regexp
            for (index_nbr = 0; index_nbr < self->index_array->bound; index_nbr++) {
                index = amq_index_array_fetch (self->index_array, index_nbr);
                if (index) {
                    if (ipr_regexp_match (regexp, index->key, NULL)) {
                        if (amq_server_config_trace_route (amq_server_config))
                            icl_console_print ("X: index    wildcard=%s routing_key=%s",
                                binding->routing_key, index->key);

                        //  Cross-reference binding and index
                        ipr_bits_set (index->bindset, binding->index);
                        ipr_looseref_queue (binding->index_list, index);
                    }
                    amq_index_unlink (&index);
                }
            }
            ipr_regexp_destroy (&regexp);
            asl_field_unlink (&routing_key);
        }
        else {
            rc = 1;
            amq_server_channel_close (
                channel, ASL_COMMAND_INVALID, "No routing_key field specified");
        }
        asl_field_list_destroy (&fields);
    }
    else {
        rc = 1;
        amq_server_channel_close (
            channel, ASL_COMMAND_INVALID, "Invalid binding arguments");
    }
</method>

<method name = "publish">
    <local>
    amq_index_t
        *index;                         //  Index object
    amq_binding_t
        *binding;                       //  Binding object
    ipr_regexp_t
        *regexp;                        //  Regular expression object
    int
        binding_nbr;                    //  Binding number, 1..n from bindset
    </local>
    //
    //  Check if routing_key is already indexed, else reindex bindings on it
    index = amq_index_hash_search (self->index_hash, routing_key);
    if (index == NULL) {
        if (amq_server_config_trace_route (amq_server_config))
            icl_console_print ("X: reindex  routing_key=%s", routing_key);

        //  Create new index and recompile all bindings for it
        index = amq_index_new (self->index_hash, routing_key, self->index_array);
        binding = amq_binding_list_first (self->exchange->binding_list);
        while (binding) {
            //  TODO: size of regexp object? keep it active per binding
            //  sub-structure for bindings, dependent on exchange class...
            regexp = ipr_regexp_new (binding->regexp);
            if (ipr_regexp_match (regexp, routing_key, NULL)) {
                if (amq_server_config_trace_route (amq_server_config))
                    icl_console_print ("X: index  routing_key=%s wildcard=%s",
                        routing_key, binding->routing_key);
                        
                //  Cross-reference binding and index
                ipr_bits_set (index->bindset, binding->index);
                ipr_looseref_queue (binding->index_list, index);
            }
            ipr_regexp_destroy (&regexp);
            binding = amq_binding_list_next (&binding);
        }
    }
    if (amq_server_config_trace_route (amq_server_config))
        icl_console_print ("X: route    routing_key=%s", routing_key);

    assert (index);
    for (IPR_BITS_EACH (binding_nbr, index->bindset)) {
        binding = self->exchange->binding_index->data [binding_nbr];
        if (amq_server_config_trace_route (amq_server_config))
            icl_console_print ("X: hit      wildcard=%s", binding->routing_key);
        amq_binding_publish (binding, channel, class_id, content, mandatory, immediate);
        delivered = TRUE;
    }
    amq_index_unlink (&index);
</method>

<private name = "header">
//  Topic names observe the Perl "word" syntax, i.e. letters,
//  digits, and underscores.
#define S_WILDCARD_SINGLE     "`w+"                 //  *
#define S_WILDCARD_MULTIPLE   "`w+(?:`.`w+)*"       //  #
static void
    s_topic_to_regexp (char *index_regexp, char *regexp);
</private>

<private>
/*    Converts a index routing_key name into a regular expression. The index
      name can contain wildcards that index part or all of a index name tree.
      '*' in the routing_key name means wildcard a single level of indexs.
      '#' in the routing_key name means wildcard zero or more levels.
      index levels are separated by '.'.  regexp must be an icl_shortstr_t.
 */
static void
s_topic_to_regexp (char *topic, char *regexp)
{
    char
        *from_ptr,
        *to_ptr;

    /*  We want a regexp starting with ^, ending with $, and with the
        * and # index wildcards replaced by appropriate regexp chars.
        We also filter out any non-alphanum characters.  We may allow
        full RE indexing on index names at a later stage.
     */
    to_ptr = regexp;
    *to_ptr++ = '^';                    //  index start of index name
    for (from_ptr = topic; *from_ptr; from_ptr++) {
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
    *to_ptr++ = '$';                    //  index end of index name
    *to_ptr++ = 0;
}
</private>

</class>
