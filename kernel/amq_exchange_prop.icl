<?xml?>
<class
    name      = "amq_exchange_prop"
    comment   = "prop exchange class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements the prop exchange, which routes messages
based on the message header fields (content header "headers"
table).
</doc>

<inherit class = "amq_exchange_base" />

<import class = "amq_hash" />

<context>
    amq_index_hash_t
        *index_hash;                    //  Indices by hash lookup
    amq_index_array_t
        *index_array;                   //  Indices by number
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
    Compiles a prop binding.
    </doc>
    <local>
    asl_field_list_t
        *fields;                        //  Decoded arguments
    asl_field_t
        *destination;                   //  Destination value
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
        destination = asl_field_list_search (fields, "destination");
        if (destination) {
            //  Turn the destination string into a nice regexp
            icl_shortstr_cpy (binding->destination, asl_field_string (destination));
            s_prop_to_regexp (asl_field_string (destination), binding->regexp);
            regexp = ipr_regexp_new (binding->regexp);

            if (amq_server_config_trace_route (amq_server_config))
                icl_console_print ("X: reindex  wildcard=%s", binding->destination);

            //  We scan all indices to see which ones match our regexp
            for (index_nbr = 0; index_nbr < self->index_array->bound; index_nbr++) {
                index = amq_index_array_fetch (self->index_array, index_nbr);
                if (index) {
                    if (ipr_regexp_match (regexp, index->key, NULL)) {
                        if (amq_server_config_trace_route (amq_server_config))
                            icl_console_print ("X: index    wildcard=%s destination=%s",
                                binding->destination, index->key);

                        //  Cross-reference binding and index
                        ipr_bits_set (index->hitset, binding->index);
                        ipr_looseref_queue (binding->index_list, index);
                    }
                    amq_index_unlink (&index);
                }
            }
            ipr_regexp_destroy (&regexp);
            asl_field_unlink (&destination);
        }
        else {
            rc = 1;
            amq_server_channel_close (
                channel, ASL_COMMAND_INVALID, "No destination field specified");
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
        binding_nbr;                    //  Binding number, 1..n from hitset
    </local>
    //
    //  Check if destination is already indexed, else reindex bindings on it
    index = amq_index_hash_search (self->index_hash, destination);
    if (index == NULL) {
        if (amq_server_config_trace_route (amq_server_config))
            icl_console_print ("X: reindex  destination=%s", destination);

        //  Create new index and recompile all bindings for it
        index = amq_index_new (self->index_hash, destination, self->index_array);
        binding = amq_binding_list_first (self->exchange->binding_list);
        while (binding) {
            //  TODO: size of regexp object? keep it active per binding
            //  sub-structure for bindings, dependent on exchange class...
            regexp = ipr_regexp_new (binding->regexp);
            if (ipr_regexp_match (regexp, destination, NULL)) {
                if (amq_server_config_trace_route (amq_server_config))
                    icl_console_print ("X: index  destination=%s wildcard=%s",
                        destination, binding->destination);
                        
                //  Cross-reference binding and index
                ipr_bits_set (index->hitset, binding->index);
                ipr_looseref_queue (binding->index_list, index);
            }
            ipr_regexp_destroy (&regexp);
            binding = amq_binding_list_next (&binding);
        }
    }
    if (amq_server_config_trace_route (amq_server_config))
        icl_console_print ("X: route    destination=%s", destination);

    assert (index);
    for (IPR_BITS_EACH (binding_nbr, index->hitset)) {
        binding = self->exchange->binding_index->data [binding_nbr];
        if (amq_server_config_trace_route (amq_server_config))
            icl_console_print ("X: hit      wildcard=%s", binding->destination);
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
    s_prop_to_regexp (char *index_regexp, char *regexp);
</private>

<private>
/*    Converts a index destination name into a regular expression. The index
      name can contain wildcards that index part or all of a index name tree.
      '*' in the destination name means wildcard a single level of indexs.
      '#' in the destination name means wildcard zero or more levels.
      index levels are separated by '.'.  regexp must be an icl_shortstr_t.
 */
static void
s_prop_to_regexp (char *prop, char *regexp)
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
    for (from_ptr = prop; *from_ptr; from_ptr++) {
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
