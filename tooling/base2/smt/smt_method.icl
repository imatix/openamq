<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

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
 -->
<class
    name      = "smt_method"
    comment   = "SMT reply class"
    version   = "1.0"
    script    = "icl_gen"
    >

<doc>
    The smt_method class is used to send and receive 'replies' (queued events
    with result code and data) for smt threads.
</doc>

<option name = "links" value = "1"/>

<inherit class = "icl_object">
    <option name = "nullify" value = "0"/>
    <option name = "alloc" value = "cache" />
</inherit>

<inherit class = "icl_queue_item"/>
<inherit class = "icl_list_item">
    <option name = "rwlock" value = "0" />
</inherit>

<import  class = "smt_thread"/>

<public name = "header">
typedef int (smt_method_destructor_fn) (void *data);
</public>

<context>
    smt_event_t 
        event;                          //  The event to deliver
    smt_priority_t
        priority;                       //  The priority for the method
    int
        result;                         //  The result
    void
        *data;                          //  Opaque data
    smt_method_destructor_fn
        *destructor;                    //  Data destructor function
</context>

<method name = "destroy">
    if (self->destructor)
        (self->destructor) (self->data);
    self->data = NULL;
</method>

<method name = "send" return = "rc">
    <argument name = "queue"      type = "smt_method_queue_t *" >Queue to send reply to</argument>
    <argument name = "event"      type = "smt_event_t"    >Internal event number</argument>
    <argument name = "priority"   type = "smt_priority_t" >The priority for the event</argument>
    <argument name = "result"     type = "int"            >Result to return</argument>
    <argument name = "data"       type = "void *"         >Data pointer</argument>
    <argument name = "destructor" type = "smt_method_destructor_fn *">Data destructor callbak</argument>
    <declare  name = "rc"         type = "int" default = "0">Return value</declare>
    <doc>
    Sends an event with accompanying result code and data pointer to a  method
    queue and call the queue's 'trigger' method.
    </doc>
    <local>
    $(selftype)  *self;
    </local>
    //  Need to create the method even if the queue is null, so that destructor
    //  can be called.
    self = $(selfname)_new ();
    self->event      = event;
    self->priority   = priority;
    self->result     = result;
    self->data       = data;
    self->destructor = destructor;
    
    queue = smt_method_queue_link (queue);  //  So can't be freed
    if (queue) {                            //  Might already be zombie
        if (smt_method_queue_queue (queue, self) == 0) {
            $(selfname)_unlink (&self);
            smt_method_queue_trigger (queue, SMT_METHOD_QUEUE_INSERT);
        }
        else {
            $(selfname)_destroy (&self);
            rc = -1;
        }
        smt_method_queue_unlink (&queue);
    }
    else {
        $(selfname)_destroy (&self);
        rc = -1;
    }
</method>

<method name = "selftest">
</method>

</class>
