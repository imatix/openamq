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
    name      = "icl_mutex"
    comment   = "Manage mutex objects"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Defines wrappers to manage mutex objects.
</doc>

<inherit class = "icl_object">
    <!-- Use direct allocation to avoid recursion -->
    <option name = "alloc"   value = "direct"/>
    <option name = "nullify" value = "1"/>
</inherit>

<context>
#if (defined (BASE_THREADSAFE))
    apr_thread_mutex_t
        *mutex;
    apr_pool_t
        *pool;
#else
    qbyte
        filler;
#endif
</context>

<method name = "alloc">
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
    apr_pool_tag (self->pool, "$(selfname)_$(method.name)");
    icl_apr_assert (apr_thread_mutex_create (&self->mutex, 
                    APR_THREAD_MUTEX_DEFAULT, self->pool));
#endif
</method>

<method name = "free">
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_mutex_destroy (self->mutex));
    self->mutex = NULL;
    apr_pool_destroy (self->pool);
    self->pool = NULL;
#endif
</method>

<!-- Use inherit=none to isolate method from that defined in 
     icl_object.icl -->
<method name = "lock" inline = "1" inherit = "none">
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    $(selfname:upper)_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    assert (self->mutex);
    icl_apr_assert (apr_thread_mutex_lock (self->mutex));
#endif
</method>
 
<method name = "unlock" inline = "1" inherit = "none">
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    $(selfname:upper)_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    assert (self->mutex);
    icl_apr_assert (apr_thread_mutex_unlock (self->mutex));
#endif
</method>

<method name = "selftest" export = "none">
</method>

</class>
