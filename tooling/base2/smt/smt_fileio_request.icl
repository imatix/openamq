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
    name      = "smt_fileio_request"
    comment   = "SMT/3 socket class"
    version   = "1.0"
    script    = "icl_gen"
    target    = "stdc"
    >

<doc>
    The smt_fileio_request class contains all the methods for doing activity
    on an open file.
</doc>

<inherit class = "icl_base"/>

<import class = "smt_thread"/>
<import class = "smt_fileio"/>

<public name = "header">
#include "base_apr.h"
</public>

<method name = "read" return = "rc">
    <argument name = "thread"   type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "file"     type = "smt_fileio_t *" >Open file to read data from</argument>
    <argument name = "min_size" type = "size_t"         >Minimum data size to read</argument>
    <argument name = "max_size" type = "size_t"         >Maximum data size to read</argument>
    <argument name = "buffer"   type = "byte *"         >Buffer to store data read</argument>
    <argument name = "event"    type = "smt_event_t"    >Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking</argument>
    <declare  name = "rc"       type = "int" default = "SMT_OK" />
    <local>
    apr_status_t
        status = APR_SUCCESS;
    size_t
        cur_size = 0;
    apr_size_t
        nbytes;
    char
        peek_char;
    </local>
    while (status == APR_SUCCESS
       &&  cur_size < min_size) {
        nbytes = max_size - cur_size;
        status = apr_file_read (file->handle,
                                buffer + cur_size,
                                &nbytes);
        cur_size += nbytes;
    }
    
    //  Because APR doesn't set eof until a failed attempt to read, we         
    //  try reading one extra byte to see if we are at the end of the file.    
    if (status == APR_SUCCESS) {
        status = apr_file_getc (&peek_char, file->handle);
        if (APR_STATUS_IS_EOF (status))
            file->eof = TRUE;
        else
            status = apr_file_ungetc (peek_char, file->handle);
    }
    
    file->io_size = cur_size;
    if (event != SMT_NULL_EVENT)
        smt_thread_set_next_event (thread, event);

    if (status == APR_SUCCESS)
        thread->result = SMT_OK;
    else {
        rc = SMT_FILEIO_ERROR;
        thread->result = SMT_FILEIO_ERROR;
        thread->error  = status;
        thread->catcher (thread);
    }
</method>

<method name = "write" return = "rc">
    <argument name = "thread"   type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "file"     type = "smt_fileio_t *" >Open file to read data from</argument>
    <argument name = "size"     type = "size_t"         >Data size to write</argument>
    <argument name = "buffer"   type = "byte *"         >Buffer to store data read</argument>
    <argument name = "event"    type = "smt_event_t"    >Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking</argument>
    <declare  name = "rc"       type = "int" default = "SMT_OK" />
    <local>
    apr_status_t
        status = APR_SUCCESS;
    size_t
        cur_size = 0;
    apr_size_t
        nbytes;
    </local>
    while (status == APR_SUCCESS
       &&  cur_size < size) {
        nbytes = size - cur_size;
        status = apr_file_write (file->handle,
                                 buffer + cur_size,
                                 &nbytes);
        cur_size += nbytes;
    }
    
    file->io_size = cur_size;
    if (event != SMT_NULL_EVENT)
        smt_thread_set_next_event (thread, event);

    if (status == APR_SUCCESS)
        thread->result = SMT_OK;
    else {
        rc = SMT_FILEIO_ERROR;
        thread->result = SMT_FILEIO_ERROR;
        thread->error  = status;
        thread->catcher (thread);
    }
</method>

<method name = "close" return = "rc">
    <argument name = "thread"   type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "file"     type = "smt_fileio_t *" >Open file to read data from</argument>
    <argument name = "event"    type = "smt_event_t"    >Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking</argument>
    <declare  name = "rc"       type = "int" default = "SMT_OK" />
    <local>
    apr_status_t
        status = APR_SUCCESS;
    </local>
    status = apr_file_close (file->handle);
    
    if (event != SMT_NULL_EVENT)
        smt_thread_set_next_event (thread, event);

    if (status == APR_SUCCESS)
        thread->result = SMT_OK;
    else {
        rc = SMT_FILEIO_ERROR;
        thread->result = SMT_FILEIO_ERROR;
        thread->error  = status;
        thread->catcher (thread);
    }
</method>

<method name = "seek" return = "rc">
    <argument name = "thread"   type = "smt_thread_t *" >Thread to deliver reply to</argument>
    <argument name = "file"     type = "smt_fileio_t *" >Open file to read data from</argument>
    <argument name = "offset"   type = "size_t"         >Position to seek to</argument>
    <argument name = "event"    type = "smt_event_t"    >Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking</argument>
    <declare  name = "rc"       type = "int" default = "SMT_OK" />
    <local>
    apr_status_t
        status;
    apr_off_t
        offset_after_seek = offset;
    </local>
    status = apr_file_seek (file->handle, APR_SET, &offset_after_seek);
    
    if (event != SMT_NULL_EVENT)
        smt_thread_set_next_event (thread, event);

    if (status == APR_SUCCESS
    &&  offset_after_seek == offset)
        thread->result = SMT_OK;
    else {
        rc = SMT_FILEIO_ERROR;
        thread->result = SMT_FILEIO_ERROR;
        thread->error  = status;
        thread->catcher (thread);
    }
</method>
    
<method name = "selftest">
</method>

</class>
