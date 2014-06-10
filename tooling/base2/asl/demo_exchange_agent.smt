<?xml?>
<!---------------------------------------------------------------------------
    demo_exchange_agent.smt - demo_exchange component

    This class implements the ASL demo server exchange class (this is a
    fanout exchange).
    Generated from demo_exchange.icl by smt_object_gen using GSL/4.

    
    Copyright (c) 1996-2009 iMatix Corporation
    
    This file is licensed under the GPL as follows:
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.
    
    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
    
    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 ----------------------------------------------------------------------------->

<agent
    name = "demo_exchange_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "demo_exchange" />

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
</method>

<method name = "bind">
    <argument name = "channel" type = "demo_server_channel_t *" />
    <argument name = "queue" type = "demo_queue_t *" />
    <argument name = "arguments" type = "icl_longstr_t *" />
</method>

<method name = "publish">
    <argument name = "channel" type = "demo_server_channel_t *" />
    <argument name = "content" type = "demo_content_basic_t *" />
    <possess>

    content = demo_content_basic_link (content);
    demo_content_basic_chrono_add (content);
    
    </possess>
    <release>

    demo_content_basic_unlink (&content);
    
    </release>
</method>

<thread name = "class">
    <context>
        demo_exchange_t
            *demo_exchange;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "demo_exchange_t *">The iCL object</argument>
        tcb->demo_exchange = self;
    </handler>
        
    <state name = "main">

        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->demo_exchange
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
icl_mem_strfree (&self->name);
demo_queue_list_destroy (&self->queue_list);
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->demo_exchange
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on demo_exchange object");
    demo_exchange_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    demo_exchange_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "bind"  >
            <action name = "bind">

#define self tcb->demo_exchange
demo_server_channel_t *
    channel;
demo_queue_t *
    queue;
icl_longstr_t *
    arguments;

    channel = bind_m->channel;
    queue = bind_m->queue;
    arguments = bind_m->arguments;
        {
demo_queue_list_iter_t *
    iterator;

iterator = demo_queue_list_first (self->queue_list);
while (iterator && iterator->item != queue)
    iterator = demo_queue_list_next (&iterator);
if (!iterator)
    demo_queue_list_queue (self->queue_list, queue);
demo_server_agent_queue_bind_ok (channel->connection->thread, channel->number);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "publish"  >
            <action name = "publish">

#define self tcb->demo_exchange
demo_server_channel_t *
    channel;
demo_content_basic_t *
    content;

    channel = publish_m->channel;
    content = publish_m->content;
        {
demo_queue_list_iter_t *
    iterator;
demo_queue_t
    *queue;

iterator = demo_queue_list_first (self->queue_list);
while (iterator) {
    queue = demo_queue_link (iterator->item);
    if (queue->lease && queue->feed_on && queue->lease->thread) {
        demo_server_agent_direct_out (queue->lease->thread, content, 0);
        icl_atomic_inc32 ((volatile qbyte *) &(queue->lease->pending));
    }
    else
        demo_queue_publish (iterator->item, channel, content);
    demo_queue_unlink (&queue);
    iterator = demo_queue_list_next (&iterator);
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <event name = "shutdown" nextstate = "shutting down" />
    </state>
    
    <state name = "shutting down">

        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->demo_exchange
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
icl_mem_strfree (&self->name);
demo_queue_list_destroy (&self->queue_list);
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->demo_exchange
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on demo_exchange object");
    demo_exchange_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    demo_exchange_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "bind" />
        <method name = "publish" />
    </state>
</thread>

</agent>
