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
    name      = "icl_cond"
    comment   = "Manage condition variable objects"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Defines wrappers to manage condition variable objects.
</doc>

<inherit class = "icl_object">
    <!-- Use direct, no point in potentially locking on allocating a 
         synchronization object -->
    <option name = "alloc"   value = "direct"/>
    <option name = "nullify" value = "1"/>
</inherit>

<context>
#if (defined (BASE_THREADSAFE))
    apr_thread_cond_t
        *cond;
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
    icl_apr_assert (apr_thread_cond_create (&self->cond, self->pool));
#endif
</method>

<method name = "free">
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_cond_destroy (self->cond));
    self->cond = NULL;
    apr_pool_destroy (self->pool);
    self->pool = NULL;
#endif
</method>

<method name = "signal" inline = "1" template = "function">
#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    icl_apr_assert (apr_thread_cond_signal (self->cond));
#endif
</method>
    
<method name = "broadcast" inline = "1" template = "function">
#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    icl_apr_assert (apr_thread_cond_broadcast (self->cond));
#endif
</method>
    
<method name = "wait" inline = "1" template = "function">
    <argument name = "mutex_p" type = "icl_mutex_t *">The mutex pointer</argument>
#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    assert (mutex_p);
    assert (mutex_p->mutex);
    icl_apr_assert (apr_thread_cond_wait (self->cond, mutex_p->mutex));
#endif
</method>

<method name = "timed wait" return = "rc" inline = "1" template = "function">
    <argument name = "mutex_p" type = "icl_mutex_t *">The mutex pointer</argument>
    <argument name = "usecs"   type = "int64_t"      >Timeout for wait</argument>
    <declare  name = "rc"      type = "int"    default = "0">Return code</declare>
    <doc>
    Returns -1 on timeout; zero otherwise.
    </doc>
    <local>
#if (defined (BASE_THREADSAFE))
    apr_status_t
        status;
#endif
    </local>
#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    assert (mutex_p);
    assert (mutex_p->mutex);
    status = apr_thread_cond_timedwait (self->cond, mutex_p->mutex, usecs);
    if (APR_STATUS_IS_TIMEUP (status)) {
        status = APR_SUCCESS;
        rc = -1;
    }
    icl_apr_assert (status);
#endif
</method>

<method name = "selftest" export = "none">
</method>

</class>
