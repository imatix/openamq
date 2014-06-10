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
    name      = "smt_fileio"
    comment   = "SMT/3 fileio class"
    version   = "1.0"
    script    = "icl_gen"
    >

<doc>
    The smt_fileio class is used to create socket objects for use with the
    smt_fileio_request class.
</doc>

<inherit class = "icl_object">
    <option name = "nullify" value = "0"/>
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "0"/>
</inherit>

<import class = "smt_thread"/>
<import class = "smt_fileio_request"/>

<public name = "header" >
#include "base_apr.h"

//  Types                                                                      

typedef enum {
    SMT_FILE_READ   = 1,
    SMT_FILE_WRITE  = 2,
    SMT_FILE_APPEND = 3
} smt_file_mode_e;
</public>

<context>
    apr_pool_t
        *pool;
    apr_file_t
        *handle;
    size_t
        io_size;
    Bool
        eof;
</context>

<method name = "new" private = "1">
    self->handle  = NULL;
    self->io_size = 0;
    self->eof     = FALSE;
    icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
    apr_pool_tag (self->pool, "$(selfname)_$(method.name)");
</method>

<method name = "destroy">
    apr_file_close (self->handle);
    apr_pool_destroy (self->pool);
</method>

<method name = "open" return = "self">
    <argument name = "thread" type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "path"   type = "const char *">The optional path???</argument>
    <argument name = "name"   type = "const char *">The name of the file to open</argument>
    <argument name = "mode"   type = "smt_file_mode_e">The mode (read/write/append)</argument>
    <argument name = "binary" type = "Bool">Open file for binary data?</argument>
    <argument name = "event"  type = "smt_event_t">Event to deliver on open; SMT_NULL_EVENT for simple thread-blocking</argument>
    <declare  name = "self"   type = "smt_fileio_t *">The fileio item</declare>
    <local>
    apr_status_t
        status;
    apr_int32_t
#if (defined (BASE_THREADSAFE))
        flag = APR_XTHREAD;
#else
        flag = 0;
#endif
    </local>
    self = smt_fileio_new ();
    switch (mode) {
        case SMT_FILE_READ:   flag = APR_READ;                break;
        case SMT_FILE_WRITE:  flag = APR_WRITE | APR_CREATE;  break;
        case SMT_FILE_APPEND: flag = APR_WRITE | APR_APPEND;  break;
    }
    if (binary)
        flag |= APR_BINARY;
        
    status = apr_file_open (&self->handle, name, flag,
                            APR_OS_DEFAULT, self->pool);
    if (status == APR_SUCCESS)
        thread->result = SMT_OK;
        if (event != SMT_NULL_EVENT)
            smt_thread_set_next_event (thread, event);
    else {
        smt_fileio_destroy (&self);
        thread->result = SMT_FILEIO_ERROR;
        thread->error  = status;
        thread->catcher (thread);
    }
</method>

<method name = "temp" return = "self">
    <argument name = "thread" type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "path"   type = "const char *">The optional path???</argument>
    <argument name = "binary" type = "Bool">Open file for binary data?</argument>
    <argument name = "event"  type = "smt_event_t">Event to deliver on open; SMT_NULL_EVENT for simple thread-blocking</argument>
    <declare  name = "self"   type = "smt_fileio_t *">The fileio item</declare>
    <local>
    apr_status_t
        status;
    apr_int32_t
#if (defined (BASE_THREADSAFE))
        flag = APR_XTHREAD | APR_CREATE | APR_READ | APR_WRITE | APR_EXCL | APR_DELONCLOSE;
#else
        flag = APR_CREATE | APR_READ | APR_WRITE | APR_EXCL | APR_DELONCLOSE;
#endif
    icl_shortstr_t
        name_template;
    </local>
    icl_shortstr_cpy (name_template, (char *) path);
    icl_shortstr_cat (name_template, "/tmp_XXXXXX");
    self = smt_fileio_new ();
    if (binary)
        flag |= APR_BINARY;
        
    status = apr_file_mktemp (&self->handle, name_template, flag, self->pool);
    if (status == APR_SUCCESS)
        thread->result = SMT_OK;
        if (event != SMT_NULL_EVENT)
            smt_thread_set_next_event (thread, event);
    else {
        smt_fileio_destroy (&self);
        thread->result = SMT_FILEIO_ERROR;
        thread->error  = status;
        thread->catcher (thread);
    }
</method>

<method name = "eof" return = "rc">
    <argument name = "self" type = "smt_fileio_t *">The fileio item</argument>
    <declare  name = "rc"   type = "Bool">The result code</declare>
    rc = self->eof;
</method>

<method name = "selftest">
</method>

</class>
