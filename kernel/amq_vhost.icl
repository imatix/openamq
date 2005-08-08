<?xml?>
<class
    name      = "amq_vhost"
    comment   = "Virtual host class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
Defines a virtual host.
</doc>

<inherit class = "icl_object">
    <option name = "alloc"  value = "cache" />
    <option name = "rwlock" value = "1" />
    <option name = "links"  value = "1" />
</inherit>

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
    amq_vhost_config_t
        *config;                        //  Virtual host configuration
    amq_exchange_table_t
        *exchange_table;                //  Exchanges for vhost, hash table
    amq_queue_table_t
        *queue_table;                   //  Queues for vhost, hash table
    amq_queue_list_t
        *queue_list;                    //  Queues for dispatching
</context>

<method name = "new">
    //TODO: load config from directory
    self->config         = amq_vhost_config_new (
        NULL, amq_server_config_trace (amq_server_config));
    self->exchange_table = amq_exchange_table_new ();
    self->queue_table    = amq_queue_table_new ();
    self->queue_list     = amq_queue_list_new ();
</method>

<method name = "destroy">
    amq_vhost_config_destroy   (&self->config);
    amq_exchange_table_destroy (&self->exchange_table);
    amq_queue_table_destroy    (&self->queue_table);
    amq_queue_list_destroy     (&self->queue_list);
</method>

<method name = "dispatch" template = "function">
    <doc>
    Dispatches all queues that have received new messages, i.e. have the
    'dirty' property set. All dispatchable queues are at the head of the
    vhost queue list.
    </doc>
    <local>
    amq_queue_t
        *queue;
    </local>
    //
    //  Dispatch all dirty message queues, which come at start of list
    queue = amq_queue_list_first (self->queue_list);
    while (queue) {
        if (queue->dirty) {
            amq_queue_dispatch (queue);
            queue = amq_queue_list_next (&queue);
        }
        else {
            amq_queue_unlink (&queue);
            break;
        }
    }
</method>

<method name = "selftest" />

</class>
