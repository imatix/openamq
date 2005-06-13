/*---------------------------------------------------------------------------
 *  amq_stdc_private.h - Common definitions for AMQ client API project
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef AMQP_COMMON_H_INCLUDED
#define AMQP_COMMON_H_INCLUDED

#include <base_apr.h>
#include <stdlib.h>
#include "amq_stdc_error.h"
#include "amq_stdc_framing.h"
#include "amq_stdc_client.h" /*  ???  */

/*  Id of AMQ protocol                                                       */
#define PROTOCOL_ID 128
/*  Version of protocol, used during client-server handshaking               */
#define PROTOCOL_VERSION 1
/*  Size of preallocated buffer used to read commands; if command size       */
/*  exceeds this value, auxiliary buffer is allocated dynamically            */
#define COMMAND_BUFFER_SIZE 1024
/*  Size of preallocated buffer used to read message headers; if size        */
/*  of header exceeds this value, auxiliary buffer is allocated dynamically  */
#define MESSAGE_HEADER_BUFFER_SIZE 1024
/*  Number of bytes to use as allocation unit for message content. Message   */
/*  is written/read from socket in blocks of this size.                      */
#define CONTENT_CHUNK_SIZE 1024

#ifdef AMQTRACE_MEMORY
void *amq_malloc_trace (size_t sz, const char *file, long line);
void *amq_realloc_trace (void *p, size_t sz, const char *file, long line);
void amq_free_trace (void *p, const char *file, long line);
void amq_stats_trace ();
#    define amq_malloc(sz) amq_malloc_trace (sz, __FILE__, __LINE__)
#    define amq_realloc(p, sz) amq_realloc_trace (p, sz, __FILE__, __LINE__)
#    define amq_free(p) amq_free_trace (p, __FILE__, __LINE__)
#    define amq_stats() amq_stats_trace ()
#else
#    define amq_malloc(sz) malloc (sz)
#    define amq_realloc(p, sz) realloc (p, sz)
#    define amq_free(p) free (p)
#    define amq_stats()
#endif

void amq_stdc_assert (
    apr_status_t result,
    const char *text,
    const char *file,
    long line
    );

#define AMQ_ASSERT(text) \
    amq_stdc_assert (APR_SUCCESS, #text, __FILE__, __LINE__);

#define AMQ_ASSERT_STATUS(result, text) \
    if (result != APR_SUCCESS) amq_stdc_assert (\
        result, #text, __FILE__, __LINE__);
 
#endif
