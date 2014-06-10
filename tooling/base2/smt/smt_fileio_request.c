/*---------------------------------------------------------------------------
    smt_fileio_request.c - smt_fileio_request component

    The smt_fileio_request class contains all the methods for doing activity
    on an open file.
    Generated from smt_fileio_request.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "smt_fileio_request.h"         //  Definitions for our class

//  Shorthands for class methods

#define self_read           smt_fileio_request_read
#define self_write          smt_fileio_request_write
#define self_close          smt_fileio_request_close
#define self_seek           smt_fileio_request_seek
#define self_selftest       smt_fileio_request_selftest
#define self_show_animation  smt_fileio_request_show_animation

Bool
    smt_fileio_request_animating = TRUE;  //  Animation enabled by default
/*  -------------------------------------------------------------------------
    smt_fileio_request_read

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    smt_fileio_request_read (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_fileio_t * file,                //  Open file to read data from
    size_t min_size,                    //  Minimum data size to read
    size_t max_size,                    //  Maximum data size to read
    byte * buffer,                      //  Buffer to store data read
    smt_event_t event                   //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
)
{
apr_status_t
    status = APR_SUCCESS;
size_t
    cur_size = 0;
apr_size_t
    nbytes;
char
    peek_char;
    int
        rc = SMT_OK;                    //  Not documented

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

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_fileio_request_write

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    smt_fileio_request_write (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_fileio_t * file,                //  Open file to read data from
    size_t size,                        //  Data size to write
    byte * buffer,                      //  Buffer to store data read
    smt_event_t event                   //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
)
{
apr_status_t
    status = APR_SUCCESS;
size_t
    cur_size = 0;
apr_size_t
    nbytes;
    int
        rc = SMT_OK;                    //  Not documented

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

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_fileio_request_close

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    smt_fileio_request_close (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_fileio_t * file,                //  Open file to read data from
    smt_event_t event                   //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
)
{
apr_status_t
    status = APR_SUCCESS;
    int
        rc = SMT_OK;                    //  Not documented

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

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_fileio_request_seek

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    smt_fileio_request_seek (
    smt_thread_t * thread,              //  Thread to deliver reply to
    smt_fileio_t * file,                //  Open file to read data from
    size_t offset,                      //  Position to seek to
    smt_event_t event                   //  Event to deliver on read complete; SMT_NULL_EVENT for simple thread-blocking
)
{
apr_status_t
    status;
apr_off_t
    offset_after_seek = offset;
    int
        rc = SMT_OK;                    //  Not documented

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

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_fileio_request_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_fileio_request_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_fileio_request_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_fileio_request_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_fileio_request_animating = enabled;
}
//  Embed the version information in the resulting binary

char *EMBED__smt_fileio_request_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_fileio_request_component  = "smt_fileio_request ";
char *EMBED__smt_fileio_request_version   = "1.0 ";
char *EMBED__smt_fileio_request_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_fileio_request_filename  = "smt_fileio_request.icl ";
char *EMBED__smt_fileio_request_builddate  = "2011/03/01 ";
char *EMBED__smt_fileio_request_version_end  = "VeRsIoNeNd:ipc";

