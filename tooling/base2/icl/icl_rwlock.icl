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
    name      = "icl_rwlock"
    comment   = "Manage read-write lock objects"
    version   = "1.2"
    script    = "icl_gen"
    >
<doc>
    Defines wrappers to managed read-write lock objects.
</doc>

<inherit class = "icl_object">
    <option name = "alloc"   value = "private"/>
    <option name = "nullify" value = "0"/>
</inherit>

<context>
#if (defined (BASE_THREADSAFE))
    apr_thread_rwlock_t
        *rwlock;
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
    apr_pool_tag   (self->pool, "$(selfname)_$(method.name)");
    icl_apr_assert (apr_thread_rwlock_create (&self->rwlock, self->pool));
#endif
</method>

<method name = "free">
#if (defined (BASE_THREADSAFE))
    if (icl_mem_allocator == ICL_MEM_DIRECT) {
        icl_apr_assert (apr_thread_rwlock_destroy (self->rwlock));
        apr_pool_destroy (self->pool);
    }
#endif
</method>

<method name = "cache initialise" private = "1" inherit = "none">
    <doc>
    We need a private cache since we're going to re-use the apr_rwlock fields.
    </doc>
    s_cache = icl_cache_create (sizeof (icl_rwlock_t));
    icl_system_register ($(base)cache_purge, $(base)cache_terminate);
</method>

<method name = "read lock" inline = "1" inherit = "none">
    <argument name = "self" type = "icl_rwlock_t *"/>
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_rdlock (self->rwlock));
#endif
</method>

<method name = "write lock" inline = "1" inherit = "none">
    <argument name = "self" type = "icl_rwlock_t *"/>
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_wrlock (self->rwlock));
#endif
</method>

<method name = "unlock" inline = "1" inherit = "none">
    <argument name = "self" type = "icl_rwlock_t *"/>
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_unlock (self->rwlock));
#endif
</method>

<method name = "selftest" export = "none">
</method>

</class>
