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
    name      = "icl_queue_head"
    comment   = "Abstract base class for queue head container"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
    Abstract base class for queue head container
</doc>

<inherit name = "icl_object"/>

<context>
    volatile $(childtype)
        *head,
        *tail;
#if defined (BASE_THREADSAFE)
    icl_mutex_t *
        mutex;
#endif
.if count ? 0
    volatile qbyte
        count;
.endif
</context>

<method name = "new">
    <header export = "after">
#if (defined (BASE_THREADSAFE))
    self->mutex = icl_mutex_new ();
#endif

    self->head = NULL;
    self->tail = NULL;

.if count ? 0
    self->count = 0;
.endif
    </header>
</method>

<method name = "destroy">
    <doc>
    Destroys the queue and unlinks all attached items.
    </doc>
    <local>
    $(childtype)
        *item;
#if defined (BASE_THREADSAFE)
    icl_mutex_t
        *mutex;
#endif
    </local>
    <header>
#if defined (BASE_THREADSAFE)
    mutex = self->mutex;
    if (mutex)
         icl_mutex_lock (mutex);
#endif

    <!-- NB we can't call pop because this will create deadlock             -->
    item = ($(childtype) *) self->head;
    while (item != NULL) {
        //  Unlink child without lock (we already have the lock)
        self->head = item->$(prefix)_next;

.if link_to_child ? child_has_links ? 1
        $(childname)_unlink (&item);
.else
        $(childname)_destroy (&item);
.endif
        item = ($(childtype) *) self->head;
    }
    </header>
    <footer>
#if defined (BASE_THREADSAFE)
    if (mutex)
        icl_mutex_unlock (mutex);
#endif
    </footer>
</method>

<method name = "free">
    <footer>
#if defined (BASE_THREADSAFE)
    if (self->mutex)
        icl_mutex_destroy (&self->mutex);
#endif
    </footer>
</method>

<method name = "pop" return = "item">
    <argument name = "self" type = "$(selftype) *">The queue</argument>
    <declare name = "item"  type = "$(childtype) *" default = "NULL"/>
    <doc>
    Removes the next item in the queue, if any, and returns it.  If
    the queue was empty, returns NULL.

    The number of links is unchanged; removing from the queue and returning
    the value cancel each other out.
    </doc>
    <local>
#if defined (BASE_THREADSAFE)
    icl_mutex_t
        *mutex;
#endif
    </local>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
#if defined (BASE_THREADSAFE)
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

.if links ? 0
    if (!self->zombie) {
.endif
        item = ($(childtype) *) self->head;
        if (item) {
            self->head = item->$(prefix)_next;
            if (self->head == NULL)
                self->tail = NULL;
            item->$(prefix)_next = NULL;
            item->$(prefix)_head = NULL;
        }

.
        if (item) {
.
.#  If we are linking to the child then links are unchanged, but if we aren't
.#  then this method is creating a new link.
.if (child_has_links ? 1) & ! (link_to_child ? 1)
            $(childname)_link (item);
.endif
.
.if count ? 0
            icl_atomic_dec32 (&self->count);
.endif
        }
    </header>
    <footer>
.if links ? 0
    }
.endif

#if defined (BASE_THREADSAFE)
    if (mutex)
        icl_mutex_unlock (mutex);
#endif
    </footer>
</method>

<method name = "queue" return = "rc" >
    <doc>
    Attaches the item to the end of the specified queue.  The item may not
    be in a queue of the same class.

    The number of links is automatically adjusted - if the item was already
    in the queue, it remains unchanged; otherwise it is incremented.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <argument name = "item" type = "$(childtype) *" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
#if defined (BASE_THREADSAFE)
    icl_mutex_t
        *mutex;
#endif
    </local>
    <header>
#if defined (BASE_THREADSAFE)
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif

    $(selfname:upper)_ASSERT_SANE (self);
.if links ? 0
    if (!self->zombie) {
.endif

        if (! item->$(prefix)_head) {
.if link_to_child ? child_has_links ? 1
            $(childname)_link (item);
.endif

            if (self->tail)
                self->tail->$(prefix)_next = item;
            else
                self->head = item;
            self->tail = item;
            item->$(prefix)_next = NULL;
            item->$(prefix)_head = self;
        }
        else {
            //  Requeuing into the same queue isn't allowed.
            icl_console_print ("E: $(childname) item inserted into multiple $(prefix) containers");
            assert (item->$(prefix)_head == NULL);
        }

.if count ? 0
        icl_atomic_inc32 (&self->count);
.endif
    </header>
    <footer>
.if links ? 0
    }
    else
        rc = -1;
.endif

#if defined (BASE_THREADSAFE)
    if (mutex)
        icl_mutex_unlock (mutex);
#endif
    </footer>
</method>

<!--
<method name = "cache initialise">
</method>

<method name = "cache purge">
</method>
-->

</class>
