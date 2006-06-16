<?xml?>
<class
    name      = "message"
    comment   = "Simple wrapper for basic messages"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_object" />

<import class = "field_table" />

<import class = "amq_client_session" type = "private" />
<import class = "amq_content_basic" type = "private" />
<import class = "icl_longstr" type = "private" />
<import class = "icl_shortstr" type = "private" />

<context>
    void
        *data;
    size_t
        size;
    size_t
        pos;
    icl_longstr_t
        *headers;
    dbyte
        weight;
    icl_shortstr_t
        exchange;
    icl_shortstr_t
        routing_key;
    icl_shortstr_t
        producer_id;
    icl_shortstr_t
        content_type;
    icl_shortstr_t
        content_encoding;
    int
        delivery_mode;
    int
        priority;
    icl_shortstr_t
        correlation_id;
    icl_shortstr_t
        reply_to;
    icl_shortstr_t
        expiration;
    icl_shortstr_t
        message_id;
    icl_shortstr_t
        type;
    icl_shortstr_t
        user_id;
    icl_shortstr_t
        app_id;
</context>

<method name = "new">
    self->data = NULL;
    self->size = 0;
    self->pos = 0;
    self->headers = NULL;
    self->weight = 0;
    self->exchange [0] = 0;
    self->routing_key [0] = 0;
    self->producer_id [0] = 0;
    self->content_type [0] = 0;
    self->content_encoding [0] = 0;
    self->delivery_mode = 1;
    self->priority = 0;
    self->correlation_id [0] = 0;
    self->reply_to [0] = 0;
    self->expiration [0] = 0;
    self->message_id [0] = 0;
    self->type [0] = 0;
    self->user_id [0] = 0;
    self->app_id [0] = 0;
</method>

<method name = "destroy">
    if (self->data)
        icl_mem_free (self->data);
    if (self->headers)
        icl_longstr_destroy (&self->headers);
</method>

<method name = "get byte" template = "function" return = "rc">
   <doc>
   Retrieves a single byte of the content
   </doc>
   <declare name = "rc" type = "int" />
   if (self->pos >= self->size)
       rc = -1;
   else {
       rc = ((unsigned char*)self->data) [self->pos];
       self->pos++;
   }
</method>

<method name = "put byte" template = "function">
    <doc>
    Writes single byte to the content
    </doc>
    <argument name = "value" type = "unsigned char" />
    self->data = icl_mem_realloc (self->data, self->size + 1);
    assert (self->data);
    ((byte*) (self->data)) [self->size] = value;
    self->size++;
</method>

<method name = "put blob" template = "function">
    <argument name = "data" type = "char*" />
    <argument name = "size" type = "int" />
    self->data = icl_mem_realloc (self->data, self->size + size);
    assert (self->data);
    memcpy (self->data + self->size, data, size);
    self->size += size;
</method>

<property name = "headers">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "field_table_t*" />
        rc = field_table_new ();
        assert (rc);
        asl_field_list_parse (rc->list, self->headers);
    </get>
    <put template = "function">
        <argument name = "headers" type = "field_table_t*" />
        if (self->headers)
            icl_longstr_destroy (&self->headers);
        self->headers = asl_field_list_flatten (headers->list);
    </put>
</property>

<property name = "weight">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "dbyte" />
        rc = self->weight;
    </get>
    <put template = "function">
        <argument name = "weight" type = "dbyte" />
        self->weight = weight;
    </put>
</property>

<property name = "exchange">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->exchange;
    </get>
</property>

<property name = "routing key">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->routing_key;
    </get>
</property>

<property name = "producer id">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->producer_id;
    </get>
</property>

<property name = "content type">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->content_type;
    </get>
    <put template = "function">
        <argument name = "content_type" type = "char*" />
        icl_shortstr_cpy (self->content_type, content_type);
    </put>
</property>

<property name = "content encoding">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->content_encoding;
    </get>
    <put template = "function">
        <argument name = "content_encoding" type = "char*" />
        icl_shortstr_cpy (self->content_encoding, content_encoding);
    </put>
</property>

<property name = "delivery mode">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "int" />
        rc = self->delivery_mode;
    </get>
    <put template = "function">
        <argument name = "delivery_mode" type = "int" />
        self->delivery_mode = delivery_mode;
    </put>
</property>

<property name = "priority">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "int" />
        rc = self->priority;
    </get>
    <put template = "function">
        <argument name = "priority" type = "int" />
        self->priority = priority;
    </put>
</property>

<property name = "correlation id">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->correlation_id;
    </get>
    <put template = "function">
        <argument name = "correlation_id" type = "char*" />
        icl_shortstr_cpy (self->correlation_id, correlation_id);
    </put>
</property>

<property name = "reply to">
    <get template = "function" return = "rc">
       <declare name = "rc" type = "char*" />
       rc = self->reply_to;
    </get>
    <put template = "function">
        <argument name = "reply_to" type = "char*" />
        icl_shortstr_cpy (self->reply_to, reply_to);
    </put>
</property>

<property name = "expiration">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->expiration;
    </get>
    <put template = "function">
        <argument name = "expiration" type = "char*" />
        icl_shortstr_cpy (self->expiration, expiration);
    </put>
</property>

<property name = "message id">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->message_id;
    </get>
    <put template = "function">
        <argument name = "message_id" type = "char*" />
        icl_shortstr_cpy (self->message_id, message_id);
    </put>
</property>

<property name = "type">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->type;
    </get>
    <put template = "function">
        <argument name = "type" type = "char*" />
        icl_shortstr_cpy (self->type, type);
    </put>
</property>

<property name = "user id">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->user_id;
    </get>
    <put template = "function">
        <argument name = "user_id" type = "char*" />
        icl_shortstr_cpy (self->user_id, user_id);
    </put>
</property>

<property name = "app id">
    <get template = "function" return = "rc">
        <declare name = "rc" type = "char*" />
        rc = self->app_id;
    </get>
    <put template = "function">
        <argument name = "app id" type = "char*" />
        icl_shortstr_cpy (self->app_id, app_id);
    </put>
</property>

<method name = "selftest">
</method>

</class>
