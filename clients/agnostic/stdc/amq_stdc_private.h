/*---------------------------------------------------------------------------
 *  amqp_common.h - common definitions for AMQ client API level 1 project
 *  File is to be removed during developement phase
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#ifndef AMQP_COMMON_H_INCLUDED
#define AMQP_COMMON_H_INCLUDED

#include <base_apr.h>
#include <stdlib.h>
#include "amq_stdc_framing.h"

#define BUFFER_SIZE 32768
#define MAX_FRAME_SIZE 32000
#define AMQTRACE_FSMS


#ifdef AMQTRACE_MEMORY
void *amq_malloc_trace (size_t sz, const char *file, long line);
void amq_free_trace (void *p, const char *file, long line);
void amq_stats_trace ();
#    define amq_malloc(sz) amq_malloc_trace (sz, __FILE__, __LINE__)
#    define amq_free(p) amq_free_trace (p, __FILE__, __LINE__)
#    define amq_stats() amq_stats_trace ()
#else
#    define amq_malloc(sz) malloc (sz)
#    define amq_free(p) free (p)
#    define amq_stats()
#endif

#define TEST(result,func,buffer) \
    if (result != APR_SUCCESS) {\
        printf ("%s:%ld " #func " failed - %ld: %s\n",\
            __FILE__, (long) __LINE__, (long)result,\
            amqp_strerror (result, buffer, BUFFER_SIZE));\
        exit (1);\
    }
 
#endif
