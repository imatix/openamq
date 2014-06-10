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
    name      = "icl_thread"
    comment   = "iCL thread class"
    version   = "1.0"
    abstract  = "1"
    script    = "icl_gen"
    >

<doc>
    Wraps APR thread as an iCL object.
</doc>

<inherit name = "icl_object"/>

<import class = "icl_system"/>

<public name = "header" >
#include "base_apr.h"
</public>

<context>
    apr_pool_t          *pool;
#if (defined (BASE_THREADSAFE))
    apr_threadattr_t    *threadattr;
    apr_thread_t        *thread;
#endif
    char * file;
    size_t line;
    Bool   started;
    apr_thread_start_t startup;
</context>

<method name = "new">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    self->file          = file;
    self->line          = line;
    self->started       = FALSE;
    icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
    apr_pool_tag (self->pool, "$(selfname)_$(method.name)");
#if (defined (BASE_THREADSAFE))
    self->threadattr = NULL;
    icl_apr_assert (apr_threadattr_create (&self->threadattr, self->pool));
#endif
</method>

<method name = "start" template = "function">
    <footer>
    self->started = TRUE;
#if (defined (BASE_THREADSAFE))
.if links ? 0
    $(selfname)_link (self);            //  OS thread needs its own link
.endif
    icl_apr_assert (apr_thread_create (&self->thread, self->threadattr, self->startup, self, self->pool));
#endif
    </footer>
</method>

<method name = "join" return = "rc">
    <doc>
    Does a 'join' to the thread which waits until the thread terminates.
    
    Don't use 'function' template since it needs to be called with a zombie
    object.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    $(selfname:upper)_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    apr_thread_join (&rc, self->thread);
#endif
</method>

<method name = "destroy">
</method>

<method name = "free">
    <footer>
    apr_pool_destroy (self->pool);
    </footer>
</method>

<method name = "startup" return = "rc" abstract = "1">
    <argument name = "self" type = "$(selftype) *"/>
    <declare name = "rc" type = "int" default = "0"/>
</method>

</class>
