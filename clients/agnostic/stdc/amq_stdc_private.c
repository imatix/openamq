/*---------------------------------------------------------------------------
 *  amqp_common.h - common definitions for AMQ client API level 1 project
 *  File is to be removed during developement phase
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_private.h"

#ifdef AMQTRACE_MEMORY

static apr_uint32_t s_allocated_blocks = 0;

void *amq_malloc_trace (size_t sz, const char *file, long line)
{
    void 
        *p;

    p = malloc (sz);
    printf ("> memory block of size %ld allocated on address %lx (%s:%ld)\n",
        (long) sz, (long) p, file, line);
    s_allocated_blocks++;
    return p;
}

void amq_free_trace (void *p, const char *file, long line)
{
    free (p);
    printf ("> memory block freed on adress %lx (%s:%ld)\n", (long) p,
        file, line);
    s_allocated_blocks--;
}

void amq_stats_trace ()
{
    printf ("> %ld memory blocks still unfreed\n", (long) s_allocated_blocks);
}

#endif

void amq_stdc_assert (
    apr_status_t result,
    const char *text,
    const char *file,
    long line
    )
{
    char
        buffer [BUFFER_SIZE];

    amqp_strerror (result, buffer, BUFFER_SIZE);
    printf ("%s:%ld - %s (%ld: %s)\n", file, line, text, (long) result, buffer);
    assert (0);
}
