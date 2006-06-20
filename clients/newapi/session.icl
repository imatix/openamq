<?xml?>
<class
    name      = "session"
    comment   = "Simple wrapper for AMQ session"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_object" />

<import class = "message" />

<import class = "amq_client_session" type = "private" />
<import class = "icl_longstr" type = "private" />


<context>
    amq_client_session_t
        *session;
</context>

<method name = "new">
    <argument name = "session" type = "amq_client_session_t*" />
    self->session = session;
</method>

<method name = "destroy">
    amq_client_session_destroy (&self->session);
</method>

<method name = "flow" template = "function">
    <argument name = "active" type = "int" const = "1">start/stop content frames</argument>
    amq_client_session_channel_flow (self->session, active);
</method>

<method name = "access request" template = "function">
    <argument name = "realm" type = "char *" const = "1">name of requested realm</argument>
    <argument name = "exclusive" type = "int" default = "0" const = "1">request exclusive access</argument>
    <argument name = "passive" type = "int" default = "0" const = "1">request passive access</argument>
    <argument name = "active" type = "int" default = "1" const = "1">request active access</argument>
    <argument name = "write" type = "int" default = "1" const = "1">request write access</argument>
    <argument name = "read" type = "int" default = "1" const = "1">request read access</argument>
    amq_client_session_access_request (self->session, realm, exclusive,
        passive, active, write, read);
</method>

<method name = "exchange declare" template = "function">
    <argument name = "exchange" type = "char *" const = "1">exchange name</argument>
    <argument name = "type" type = "char *" const = "1">exchange type</argument>
    <argument name = "passive" type = "int" default = "0" const = "1">do not create exchange</argument>
    <argument name = "durable" type = "int" default = "1" const = "1">request a durable exchange</argument>
    <argument name = "auto_delete" type = "int" default = "0" const = "1">auto-delete when unused</argument>
    <argument name = "internal" type = "int" default = "0" const = "1">create internal exchange</argument>
    amq_client_session_exchange_declare (self->session, 0, exchange, type, passive,
        durable, auto_delete, internal, NULL);
</method>

<method name = "exchange delete" template = "function">
    <argument name = "exchange" type = "char *" const = "1">exchange name</argument>
    <argument name = "if_unused" type = "int" default = "0" const = "1">delete only if unused</argument>
    amq_client_session_exchange_delete (self->session, 0, exchange, if_unused);
</method>

<method name = "queue declare" template = "function">
    <argument name = "queue" type = "char *" const = "1">queue name</argument>
    <argument name = "passive" type = "int" default = "0" const = "1">do not create queue</argument>
    <argument name = "durable" type = "int" default = "1" const = "1">request a durable queue</argument>
    <argument name = "exclusive" type = "int" default = "0" const = "1">request an exclusive queue</argument>
    <argument name = "auto_delete" type = "int" default = "0" const = "1">auto-delete queue when unused</argument>
    amq_client_session_queue_declare (self->session, 0, queue, passive,
        durable, exclusive, auto_delete, NULL);
</method>

<method name = "queue bind" template = "function">
    <argument name = "queue" type = "char *" const = "1">queue name</argument>
    <argument name = "exchange" type = "char *" default = "&quot;amq.direct&quot;" const = "1">exchange name</argument>
    <argument name = "routing_key" type = "char *" const = "1" default = "&quot;&quot;">message routing key</argument>
    <argument name = "arguments" type = "field_table_t*" const = "1" default = "NULL">binging arguments</argument>
    <declare name = "args" type = "icl_longstr_t*" default = "NULL" />
    if (arguments) {
        args = asl_field_list_flatten (arguments->list);
        assert (args);
    }
    amq_client_session_queue_bind (self->session, 0, queue, exchange,
        routing_key, args);
    icl_longstr_destroy (&args);
</method>

<method name = "queue purge" template = "function">
    <argument name = "queue" type = "char *" const = "1">queue name</argument>
    amq_client_session_queue_purge (self->session, 0, queue);
</method>

<method name = "queue delete" template = "function">
    <argument name = "queue" type = "char *" const = "1">queue name</argument>
    <argument name = "if_unused" type = "int" default = "0" const = "1">delete only if unused</argument>
    <argument name = "if_empty" type = "int" default = "0" const = "1">delete only if empty</argument>
    amq_client_session_queue_delete (self->session, 0, queue,
        if_unused, if_empty);
</method>

<method name = "basic consume" template = "function">
    <argument name = "queue" type = "char *" const = "1">queue name</argument>
    <argument name = "consumer_tag" type = "char *" default = "&quot;&quot;" const = "1">consumer tag</argument>
    <argument name = "no_local" type = "int" default = "0" const = "1">do not deliver own messages</argument>
    <argument name = "no_ack" type = "int" default = "1" const = "1">no acknowledgement needed</argument>
    <argument name = "exclusive" type = "int" default = "0" const = "1">request exclusive access</argument>
    amq_client_session_basic_consume (self->session, 0, queue, consumer_tag, no_local, no_ack, exclusive);
</method>

<method name = "basic cancel" template = "function">
    <argument name = "consumer_tag" type = "char *" const = "1">consumer tag</argument>
    amq_client_session_basic_cancel (self->session, consumer_tag);
</method>

<method name = "basic publish" template = "function">
    <argument name = "msg" type = "message_t*" const = "1">message to publish</argument>
    <argument name = "exchange" type = "char *" default = "&quot;amq.direct&quot;" const = "1">exchange name</argument>
    <argument name = "routing_key" type = "char *" default = "&quot;&quot;" const = "1">Message routing key</argument>
    <argument name = "mandatory" type = "int" default = "0" const = "1">indicate mandatory routing</argument>
    <argument name = "immediate" type = "int" default = "0" const = "1">request immediate delivery</argument>
    <declare name = "content" type = "amq_content_basic_t*" />
    content = amq_content_basic_new ();
    assert (content);
    amq_content_basic_set_body (content, msg->data, msg->size,
        icl_mem_free);
    msg->data = NULL;
    msg->size = 0;
    msg->pos = 0;
    if (msg->headers) {
        amq_content_basic_set_headers (content, msg->headers);
        icl_longstr_destroy (&msg->headers);
    }
    content->weight = msg->weight;
    msg->weight = 0;
    msg->exchange [0] = 0;
    msg->routing_key [0] = 0;
    icl_shortstr_cpy (content->producer_id, msg->producer_id);
    msg->producer_id [0] = 0;
    icl_shortstr_cpy (content->content_type, msg->content_type);
    msg->content_type [0] = 0;
    icl_shortstr_cpy (content->content_encoding, msg->content_encoding);
    msg->content_encoding [0] = 0;
    content->delivery_mode = msg->delivery_mode;
    msg->delivery_mode = 1;
    content->priority = msg->priority;
    msg->priority = 0;
    icl_shortstr_cpy (content->correlation_id, msg->correlation_id);
    msg->correlation_id [0] = 0;
    icl_shortstr_cpy (content->reply_to, msg->reply_to);
    msg->reply_to [0] = 0;
    icl_shortstr_cpy (content->expiration, msg->expiration);
    msg->expiration [0] = 0;
    icl_shortstr_cpy (content->message_id, msg->message_id);
    msg->message_id [0] = 0;
    icl_shortstr_cpy (content->type, msg->type);
    msg->type [0] = 0;
    icl_shortstr_cpy (content->user_id, msg->user_id);
    msg->user_id [0] = 0;
    icl_shortstr_cpy (content->app_id, msg->app_id);
    msg->app_id [0] = 0;
    amq_client_session_basic_publish (self->session, content, 0, exchange,
        routing_key, mandatory, immediate);
    amq_content_basic_destroy (&content);
</method>

<method name = "basic get" template = "function">
    <argument name = "queue" type = "char *" const = "1">queue name</argument>
    <argument name = "no_ack" type = "int" default = "1" const = "1">no acknowledgement needed</argument>
    amq_client_session_basic_get (self->session, 0, queue, no_ack);
</method>

<method name = "basic ack" template = "function">
    <argument name = "delivery_tag" type = "char*" const = "1">server-assigned delivery tag</argument>
    <argument name = "multiple" type = "int" default = "1" const = "1">acknowledge multiple messages</argument>
    <declare name = "tag" type = "int64_t" />
    <declare name = "pos" type = "int" />
    tag = 0;
    pos = 0;
    while (delivery_tag [pos]) {
        tag *= 10;
        tag += delivery_tag [pos] - '0';
    }
    amq_client_session_basic_ack (self->session, tag, multiple);
</method>

<method name = "basic reject" template = "function">
    <argument name = "delivery_tag" type = "char*" const = "1">server-assigned delivery tag</argument>
    <argument name = "requeue" type = "int" default = "0" const = "1">requeue the message</argument>
    <declare name = "tag" type = "int64_t" />
    <declare name = "pos" type = "int" />
    tag = 0;
    pos = 0;
    while (delivery_tag [pos]) {
        tag *= 10;
        tag += delivery_tag [pos] - '0';
    }
    amq_client_session_basic_reject (self->session, tag, requeue);
</method>

<method name = "get message" template = "function" return = "rc">
    <doc>
    Retrieves an incomming message
    </doc>
    <declare name = "rc" type = "message_t*" default = "NULL" />
    <declare name = "content" type = "amq_content_basic_t*" />
    <declare name = "reader" type = "asl_reader_t" />
    <declare name = "body" type = "ipr_bucket_t*" default = "NULL" />
    rc = message_new ();
    assert (rc);
    content = NULL;
    while (!content) {
        amq_client_session_wait (self->session, 0);
        content = amq_client_session_basic_arrived (self->session);
    }
    rc->data = icl_mem_alloc (content->body_size);        
    amq_content_basic_set_reader (content, &reader, 16384);
    while ((body = amq_content_basic_replay_body (content, &reader))) {
        assert (rc->size + body->cur_size <= content->body_size);
        memcpy (rc->data + rc->size, body->data, body->cur_size);
        rc->size += body->cur_size;            
        ipr_bucket_destroy (&body);
    }
    rc->headers = icl_longstr_dup (content->headers);
    rc->weight = content->weight;
    icl_shortstr_cpy (rc->exchange, content->exchange);
    icl_shortstr_cpy (rc->routing_key, content->routing_key);
    icl_shortstr_cpy (rc->producer_id, content->producer_id);
    icl_shortstr_cpy (rc->content_type, content->content_type);
    icl_shortstr_cpy (rc->content_encoding, content->content_encoding);
    rc->delivery_mode = content->delivery_mode;
    rc->priority = content->priority;
    icl_shortstr_cpy (rc->correlation_id, content->correlation_id);
    icl_shortstr_cpy (rc->reply_to, content->reply_to);
    icl_shortstr_cpy (rc->expiration, content->expiration);
    icl_shortstr_cpy (rc->message_id, content->message_id);
    icl_shortstr_cpy (rc->type, content->type);
    icl_shortstr_cpy (rc->user_id, content->user_id);
    icl_shortstr_cpy (rc->app_id, content->app_id);
    amq_content_basic_destroy (&content);
</method>

<method name = "selftest">
</method>

</class>
