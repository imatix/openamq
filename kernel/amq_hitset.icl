<?xml?>
<class
    name      = "amq_hitset"
    comment   = "Holds a match hitset"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
    Holds a hitset, which is the result of a match on topic name, fields,
    etc.  The hitset is an array of all matching items (subscribers),
    plus upper/lower limits on this array.
</doc>

<inherit class = "icl_alloc_cache" />

<import class = "amq_classes" />
<option name = "nullify" value = "0" />

<context>
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */
    int
        hit_count,                      /*  Number of hit items              */
        item_lo,                        /*  Lowest found item                */
        item_hi;                        /*  Highest found item               */
    int
        item_hits [IPR_INDEX_MAX];      /*  Hit count per item               */
</context>

<method name = "new">
    <argument name = "vhost" type = "amq_vhost_t *">Virtual host</argument>
    self->vhost     = vhost;
    self->item_lo   = IPR_INDEX_MAX;
    self->item_hi   = -1;
    self->hit_count = 0;
</method>

<method name = "match" template = "function">
    <doc>
    Builds hitset of all subscribers interested in a specified message.
    Matches against topic name and message property fields.
    Returns number of subscribers.
    </doc>
    <argument name = "message" type = "amq_smessage_t *">Message, if any</argument>
    <local>
    amq_subscr_t
        *subscr;                        /*  Subscriber object                */
    int
        subscr_nbr;
    amq_field_list_t
        *fields;                        /*  Selector fields                  */
    amq_field_t
        *field;                         /*  Next field to examine            */
    ipr_longstr_t
        *match_key;                     /*  Match key to lookup              */
    </local>

    /*  Lookup topic name in match table, if found collect subscr hits       */
    match_key = ipr_longstr_new_str (message->dest_name);
    amq_hitset_collect (self, self->vhost->match_topics, match_key);
    ipr_longstr_destroy (&match_key);

    /*  Lookup fields in match table, if found, collect subscr hits          */
    fields = amq_field_list_new (message->headers);
    if (fields) {
        field = amq_field_list_first (fields);
        while (field) {
            /*  First match on field name and value                          */
            match_key = amq_match_field_value (field);
            amq_hitset_collect  (self, self->vhost->match_fields, match_key);
            ipr_longstr_destroy (&match_key);

            /*  If field had a value, now match on name alone                */
            if (field->string->cur_size > 0) {
                match_key = amq_match_field_name (field);
                amq_hitset_collect (self, self->vhost->match_fields, match_key);
                ipr_longstr_destroy (&match_key);
            }
            field = amq_field_list_next (fields, field);
        }
        amq_field_list_destroy (&fields);
    }
    else
        amq_global_set_error (AMQP_SYNTAX_ERROR, "Invalid headers field table");

    /*  Now count subscribers with all hits                                  */
    for (subscr_nbr = self->item_lo; subscr_nbr <= self->item_hi; subscr_nbr++) {
        subscr = (amq_subscr_t *) self->vhost->subscr_index->data [subscr_nbr];
        if (subscr) {
            /*  Number of hits we want is field_count, +1 for topic name     */
            coprintf ("SUBSCR %d: have=%d want=%d hits",
                subscr_nbr, self->item_hits [subscr_nbr], subscr->field_count + 1);

            if (self->item_hits [subscr_nbr] == subscr->field_count + 1
            && (subscr->consumer->no_local == FALSE
            ||  subscr->consumer->handle->client_id != message->handle->client_id))
                rc++;
            else
                self->item_hits [subscr_nbr] = 0;
        }
    }
</method>

<method name = "publish" template = "function">
    <doc>
    Publish message to all subscribers in hitset.
    </doc>
    <argument name = "message" type = "amq_smessage_t *">Message, if any</argument>
    <argument name = "txn"     type = "ipr_db_txn_t *">Transaction, if any</argument>
    <local>
    amq_subscr_t
        *subscr;                        /*  Subscriber object                */
    int
        subscr_nbr;
    </local>

    /*  Now publish to subscribers with hits                                 */
    for (subscr_nbr = self->item_lo; subscr_nbr <= self->item_hi; subscr_nbr++) {
        if (self->item_hits [subscr_nbr]) {
            subscr = (amq_subscr_t *) self->vhost->subscr_index->data [subscr_nbr];
            amq_queue_publish (subscr->consumer->queue, message, txn);
        }
    }
</method>

<method name = "collect" template = "function">
    <argument name = "match table" type = "amq_match_table_t *">Search this</argument>
    <argument name = "match key"   type = "ipr_longstr_t *">For this term</argument>
    <local>
    amq_match_t
        *match;                         /*  Match item                       */
    int
        item_nbr;
    </local>

    match = amq_match_search (match_table, match_key);
    if (match) {
        for (IPR_BITS_EACH (item_nbr, match->bits)) {
            coprintf (" -- found subscr nbr %d", item_nbr);
            if (item_nbr < self->item_lo)
                self->item_lo = item_nbr;
            if (item_nbr > self->item_hi) {
                self->item_hits [item_nbr] = 0;
                self->item_hi = item_nbr;
            }
            self->item_hits [item_nbr]++;
        }
    }
</method>

<method name = "selftest" />

</class>

