<?xml?>
<!---------------------------------------------------------------------------
    demo_queue_agent.smt - demo_queue component

    This class implements the server queue class, an asynchronous object
    that acts as a envelope for the separate queue managers for each
    class.
    Generated from demo_queue.icl by smt_object_gen using GSL/4.

    
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
    name = "demo_queue_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "demo_queue" />

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
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

<method name = "get">
    <argument name = "channel" type = "demo_server_channel_t *" />
</method>

<thread name = "class">
    <context>
        demo_queue_t
            *demo_queue;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "demo_queue_t *">The iCL object</argument>
        tcb->demo_queue = self;
    </handler>
        
    <state name = "main">

        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->demo_queue
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
demo_server_channel_unlink (&self->channel);
demo_content_basic_list_destroy (&self->content_list);
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->demo_queue
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on demo_queue object");
    demo_queue_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    demo_queue_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "publish"  >
            <action name = "publish">

#define self tcb->demo_queue
demo_server_channel_t *
    channel;
demo_content_basic_t *
    content;

    channel = publish_m->channel;
    content = publish_m->content;
        {
demo_content_basic_list_queue (self->content_list, content);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "get"  >
            <action name = "get">

#define self tcb->demo_queue
demo_server_channel_t *
    channel;

    channel = get_m->channel;
        {
demo_content_basic_t
    *content;                       //  Content object reference

//  Get next message off list, if any
if (!demo_content_basic_list_isempty (self->content_list)) {
    content = demo_content_basic_list_pop (self->content_list);
    demo_server_agent_basic_get_ok (
        channel->connection->thread,
        channel->number,
        content,
        content->exchange,
        content->routing_key,
        demo_content_basic_list_count (self->content_list),
        NULL);
    demo_content_basic_unlink (&content);
}
else
    demo_server_agent_basic_get_empty (
        channel->connection->thread,
        channel->number);
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

#define self tcb->demo_queue
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
demo_server_channel_unlink (&self->channel);
demo_content_basic_list_destroy (&self->content_list);
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->demo_queue
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on demo_queue object");
    demo_queue_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    demo_queue_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "publish" />
        <method name = "get" />
    </state>
</thread>

</agent>
