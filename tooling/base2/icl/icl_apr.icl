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
    name      = "icl_apr"
    base      = "icl"
    comment   = "APR wrapper class"
    version   = "1.1"
    script    = "icl_gen"
    >
<doc>
    Provides iCL wrappers for some APR functions.
</doc>

<inherit class = "icl_base" />

<public name = "header">
#include "base_apr.h"

//  Macros                                                                     

#if (defined (BASE_THREADSAFE))
#define icl_os_thread_t apr_os_thread_t
#else
#define icl_os_thread_t qbyte
#endif

//  Global variables                                                           

extern apr_pool_t *
    icl_global_pool;
extern apr_allocator_t *
    icl_global_allocator;
    
#if (defined (BASE_THREADSAFE))
extern apr_thread_mutex_t *
    icl_global_mutex;
extern apr_thread_mutex_t *
    icl_global_allocator_mutex;
#endif
</public>

<private name = "header">
//  Global variables                                                           

apr_pool_t *
    icl_global_pool = NULL;
apr_allocator_t *
    icl_global_allocator = NULL;
    
#if (defined (BASE_THREADSAFE))
apr_thread_mutex_t *
    icl_global_mutex = NULL;
apr_thread_mutex_t *
    icl_global_allocator_mutex = NULL;
#endif
</private>

<method name = "apr assert" inline = "1">
    <argument name = "status" type = "apr_status_t">The status code to print</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    if (status != APR_SUCCESS) {
        icl_apr_error_print_ (status, file, line);
        abort ();
    }
</method>

<method name = "apr error print">
    <argument name = "status" type = "apr_status_t">The status code to print</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <local>
    char
        buffer [128];
    </local>
    icl_console_print ("(%s %lu) %s", file, (long) line, apr_strerror (status, buffer, 128));
</method>

<method name = "atomic inc32" return = "val" inline = "1">
    <argument name = "mem" type = "volatile qbyte *">The memory address</argument>
    <declare name = "val" type = "qbyte">The old value</declare>
#if (defined (BASE_THREADSAFE))
    val = (qbyte) apr_atomic_inc32 (mem);
#else
    val = (*mem)++;
#endif
</method>
    
<method name = "atomic dec32" return = "non zero" inline = "1">
    <argument name = "mem" type = "volatile qbyte *">The memory address</argument>
    <declare name = "non zero" type = "Bool">True if value becomes non-zero</declare>
#if (defined (BASE_THREADSAFE))
    non_zero = (apr_atomic_dec32 (mem) != 0);
#else
    non_zero = (--(*mem) != 0);
#endif
</method>
    
<method name = "atomic get32" return = "val" inline = "1">
    <argument name = "mem" type = "volatile qbyte *">The memory address</argument>
    <declare name = "val" type = "qbyte">The value</declare>
#if (defined (BASE_THREADSAFE))
    val = (qbyte) apr_atomic_read32 (mem);
#else
    val = *mem;
#endif
</method>
    
<method name = "atomic set32" inline = "1">
    <argument name = "mem" type = "volatile qbyte *">The memory address</argument>
    <argument name = "val" type = "qbyte">The value</argument>
#if (defined (BASE_THREADSAFE))
    apr_atomic_set32 (mem, val);
#else
    *mem = val;
#endif
</method>

<method name = "atomic add32" return = "old val" inline = "1">
    <argument name = "mem" type = "volatile qbyte *">The memory address</argument>
    <argument name = "add val" type = "qbyte">The value to add</argument>
    <declare name = "old val" type = "qbyte">The old value</declare>
#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_add32 (mem, add_val);
#else
    old_val = *mem;
    *mem += add_val;
#endif
</method>
    
<method name = "atomic sub32" inline = "1">
    <argument name = "mem" type = "volatile qbyte *">The memory address</argument>
    <argument name = "val" type = "qbyte">The value to subtract</argument>
#if (defined (BASE_THREADSAFE))
    apr_atomic_sub32 (mem, val);
#else
    *mem -= val;
#endif
</method>
    
<method name = "atomic cas32" return = "old val" inline = "1">
    <argument name = "mem" type = "volatile qbyte *">The memory address</argument>
    <argument name = "new val" type = "qbyte">The new value</argument>
    <argument name = "cmp val" type = "qbyte">The value to compare with</argument>
    <declare name = "old val" type = "qbyte">The old value</declare>
#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_cas32 (mem, new_val, cmp_val);
#else
    old_val = *mem;
    if (*mem == cmp_val)
        *mem = new_val;
#endif
</method>
    
<method name = "atomic casptr" return = "old val" inline = "1">
    <argument name = "mem" type = "volatile void **">The memory address</argument>
    <argument name = "new val" type = "void *">The new value</argument>
    <argument name = "cmp val" type = "void *">The value to compare with</argument>
    <declare name = "old val" type = "volatile void *">The old value</declare>
#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_casptr (mem, new_val, cmp_val);
#else
    old_val = *mem;
    if (*mem == cmp_val)
        *mem = new_val;
#endif
</method>

<method name = "atomic swap" return = "old val" inline = "1">
    <argument name = "mem" type = "volatile qbyte *">The memory address</argument>
    <argument name = "new val" type = "qbyte">The new value</argument>
    <declare name = "old val" type = "qbyte">The old value</declare>
#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_xchg32 (mem, new_val);
#else
    old_val = *mem;
    *mem = new_val;
#endif
</method>
    
<method name = "selftest" />

</class>
