<?xml?>
<!---------------------------------------------------------------------------
    demo_server_channel_agent.smt - demo_server_channel component

    This class implements the ASL demo server channel class.
    Generated from demo_server_channel.icl by smt_object_gen using GSL/4.

    
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
    name = "demo_server_channel_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "demo_server_channel" />

<method name = "consume">
    <argument name = "method" type = "demo_server_method_t *" />
    <possess>

    method = demo_server_method_link (method);
    
    </possess>
    <release>

    demo_server_method_unlink (&method);
    
    </release>
</method>

<method name = "cancel">
    <argument name = "method" type = "demo_server_method_t *" />
    <possess>

    method = demo_server_method_link (method);
    
    </possess>
    <release>

    demo_server_method_unlink (&method);
    
    </release>
</method>

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
</method>

<thread name = "class">
    <context>
        demo_server_channel_t
            *demo_server_channel;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "demo_server_channel_t *">The iCL object</argument>
        tcb->demo_server_channel = self;
    </handler>
        
    <state name = "main">

        <method name = "consume"  >
            <action name = "consume">

#define self tcb->demo_server_channel
demo_server_method_t *
    method;

    method = consume_m->method;
        {
demo_queue_t
    *queue;

queue = demo_queue_search (method->payload.basic_consume.queue);
if (queue) {
    if (queue->exclusive && queue->channel == self) {
        demo_server_agent_basic_consume_ok (
            self->connection->thread, 
            self->number, 
            queue->name);
        queue->feed_on = TRUE;
        demo_queue_unlink (&queue);
    }
    else
        demo_server_channel_error (self, ASL_ACCESS_REFUSED, 
            "Access refused to queue",
            DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CONSUME);
}
else
    demo_server_channel_error (self, ASL_NOT_FOUND, 
        "No such queue defined", 
        DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CONSUME);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "cancel"  >
            <action name = "cancel">

#define self tcb->demo_server_channel
demo_server_method_t *
    method;

    method = cancel_m->method;
        {
demo_queue_t
    *queue;

queue = demo_queue_search (method->payload.basic_cancel.consumer_tag);
if (queue) {
    if (queue->exclusive && queue->channel == self) {
        demo_server_agent_basic_cancel_ok (
            self->connection->thread, 
            self->number, 
            queue->name);
        queue->feed_on = FALSE;
        demo_queue_unlink (&queue);
    }
    else
        demo_server_channel_error (self, ASL_ACCESS_REFUSED, 
            "Access refused to consumer",
            DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CANCEL);
}
else
    demo_server_channel_error (self, ASL_NOT_FOUND, 
        "Invalid consumer tag", 
        DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CANCEL);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->demo_server_channel
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
demo_server_connection_unlink (&self->connection);
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->demo_server_channel
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on demo_server_channel object");
    demo_server_channel_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    demo_server_channel_free (self);
    self = NULL;
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

        <method name = "cancel"  >
            <action name = "cancel">

#define self tcb->demo_server_channel
demo_server_method_t *
    method;

    method = cancel_m->method;
        {
demo_queue_t
    *queue;

queue = demo_queue_search (method->payload.basic_cancel.consumer_tag);
if (queue) {
    if (queue->exclusive && queue->channel == self) {
        demo_server_agent_basic_cancel_ok (
            self->connection->thread, 
            self->number, 
            queue->name);
        queue->feed_on = FALSE;
        demo_queue_unlink (&queue);
    }
    else
        demo_server_channel_error (self, ASL_ACCESS_REFUSED, 
            "Access refused to consumer",
            DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CANCEL);
}
else
    demo_server_channel_error (self, ASL_NOT_FOUND, 
        "Invalid consumer tag", 
        DEMO_SERVER_BASIC, DEMO_SERVER_BASIC_CANCEL);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->demo_server_channel
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
demo_server_connection_unlink (&self->connection);
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->demo_server_channel
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on demo_server_channel object");
    demo_server_channel_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    demo_server_channel_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "consume" />
    </state>
</thread>

</agent>
