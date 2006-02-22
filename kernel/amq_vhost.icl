<?xml?>
<class
    name      = "amq_vhost"
    comment   = "Virtual host class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
Defines a virtual host. This is a lock-free asynchronous class.
</doc>

<inherit class = "smt_object" />
<!-- any containers must come here -->
<inherit class = "amq_console_object" />

<data name = "cml">
    <class name = "vhost" parent = "broker" label = "Virtual Hosts" >
        <field name = "name" label = "Virtual host path">
          <get>icl_shortstr_cpy (field_value, self->name);</get>
        </field>

        <class name = "exchange" label = "Message exchanges" repeat = "1">
          <local>
            amq_exchange_t
                *exchange;
          </local>
          <get>
            exchange = amq_exchange_by_vhost_first (self->exchange_list);
            icl_shortstr_fmt (field_value, "%ld", exchange->object_id);
          </get>
          <next>
            exchange = amq_exchange_by_vhost_next (&exchange);
            if (exchange)
                icl_shortstr_fmt (field_value, "%ld", exchange->object_id);
          </next>
        </class>

        <class name = "queue" label = "Message queues" repeat = "1">
          <local>
            amq_queue_t
                *queue;
          </local>
          <get>
            queue = amq_queue_by_vhost_first (self->queue_list);
            icl_shortstr_fmt (field_value, "%ld", queue->object_id);
          </get>
          <next>
            queue = amq_queue_by_vhost_next (&queue);
            if (queue)
                icl_shortstr_fmt (field_value, "%ld", queue->object_id);
          </next>
        </class>
    </class>
</data>

<import class = "amq_server_classes" />

<public>
extern $(selftype)
    *amq_vhost;                         //  Single vhost, for now
</public>

<private>
$(selftype)
    *amq_vhost = NULL;
</private>

<context>
    amq_broker_t
        *broker;                        //  Parent broker
    icl_shortstr_t
        name;                           //  Virtual host name
    amq_vhost_config_t
        *config;                        //  Virtual host configuration
    amq_exchange_table_t
        *exchange_table;                //  Exchanges for vhost, hash table
    amq_exchange_by_vhost_t
        *exchange_list;                 //  Queues for dispatching
    amq_queue_table_t
        *queue_table;                   //  Queues for vhost, hash table
    amq_queue_by_vhost_t
        *queue_list;                    //  Queues for dispatching
    amq_exchange_t
        *default_exchange;              //  Default exchange
    ipr_symbol_table_t
        *shared_queues;                 //  Cluster shared queues
</context>

<method name = "new">
    <argument name = "broker" type = "amq_broker_t *">Parent broker</argument>
    <argument name = "name"   type = "char *">Virtual host name</argument>

    //TODO: load config from directory
    //TODO: amq_server object, holding vhost hash table
    icl_shortstr_cpy (self->name, name);
    asl_log_print (amq_broker->daily_log, "I: starting virtual host '%s'", self->name);

    self->broker = broker;
    self->config = amq_vhost_config_new (NULL, NULL, FALSE);

    self->exchange_table = amq_exchange_table_new ();
    self->exchange_list  = amq_exchange_by_vhost_new ();
    self->queue_table    = amq_queue_table_new ();
    self->queue_list     = amq_queue_by_vhost_new ();
    self->shared_queues  = ipr_symbol_table_new ();

    //  Automatic wiring schemes
    s_exchange_declare (self, "$default$",  AMQ_EXCHANGE_DIRECT,  TRUE);
    s_exchange_declare (self, "amq.fanout", AMQ_EXCHANGE_FANOUT,  FALSE);
    s_exchange_declare (self, "amq.direct", AMQ_EXCHANGE_DIRECT,  FALSE);
    s_exchange_declare (self, "amq.topic",  AMQ_EXCHANGE_TOPIC,   FALSE);
    s_exchange_declare (self, "amq.match",  AMQ_EXCHANGE_HEADERS, FALSE);
    s_exchange_declare (self, "amq.system", AMQ_EXCHANGE_SYSTEM,  FALSE);
    s_exchange_declare (self, "amq.notify", AMQ_EXCHANGE_TOPIC,   FALSE);
</method>

<method name = "destroy">
    <action>
    amq_vhost_config_destroy       (&self->config);
    amq_exchange_table_destroy     (&self->exchange_table);
    amq_exchange_by_vhost_destroy  (&self->exchange_list);
    amq_queue_table_destroy        (&self->queue_table);
    amq_queue_by_vhost_destroy     (&self->queue_list);
    ipr_symbol_table_destroy       (&self->shared_queues);
    </action>
</method>

<method name = "dispatch" template = "async function" async = "1">
    <doc>
    Dispatches all queues that have received new messages, i.e. have the
    'dirty' property set. All dispatchable queues are at the head of the
    vhost queue list.
    </doc>
    <action>
    amq_queue_t
        *queue;

    //  Dispatch all dirty message queues, which come at start of list
    queue = amq_queue_by_vhost_first (self->queue_list);
    while (queue) {
        if (queue->dirty) {
            amq_queue_dispatch (queue);
            queue = amq_queue_by_vhost_next (&queue);
        }
        else {
            amq_queue_unlink (&queue);
            break;
        }
    }
    </action>
</method>

<method name = "unbind queue" template = "async function" async = "1">
    <doc>
    Unbind a queue from the vhost.
    </doc>
    <argument name = "queue" type = "amq_queue_t *">The queue to unbind</argument>
    //
    <possess>
    queue = amq_queue_link (queue);
    </possess>
    <release>
    amq_queue_unlink (&queue);
    </release>
    //
    <action>
    amq_exchange_t
        *exchange;

    //  Go through all exchanges & bindings, remove link to queue
    exchange = amq_exchange_by_vhost_first (self->exchange_list);
    while (exchange) {
        amq_exchange_unbind_queue (exchange, queue);
        exchange = amq_exchange_by_vhost_next (&exchange);
    }

    //  Remove the queue from queue list and queue table
    amq_queue_by_vhost_remove (queue);
    amq_queue_table_remove (queue);
    </action>
</method>

<private name = "header">
//  Prototypes for local functions
static void
    s_exchange_declare (amq_vhost_t *self, char *name, int type, Bool default_exchange);
</private>

<private name = "footer">
static void
s_exchange_declare (amq_vhost_t *self, char *name, int type, Bool default_exchange)
{
    amq_exchange_t
        *exchange;                      //  Predeclared exchanges

    exchange = amq_exchange_new (
        self->exchange_table,
        self,                           //  Parent vhost
        type,                           //  Exchange type
        name,                           //  Exchange name
        TRUE,                           //  Durable, allows durable queues
        FALSE,                          //  Do not auto-delete
        default_exchange);              //  Internal?
    assert (exchange);
    if (default_exchange)
        self->default_exchange = exchange;

    amq_exchange_unlink (&exchange);
}
</private>

<method name = "selftest" />

</class>
