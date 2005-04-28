/*---------------------------------------------------------------------------
 *  amqp_common.h - common definitions for AMQ client API level 1 project
 *  File is to be removed during developement phase
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_private.h"

#ifdef AMQTRACE_MEMORY

static qbyte s_allocated_blocks = 0;

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

void *amq_realloc_trace (void *p, size_t sz, const char *file, long line)
{
    void 
        *pnew;

    pnew = realloc (p, sz);
    printf ("> memory block on adress %lx reallocated to size %ld; "
        "now on address %lx (%s:%ld)\n", 
        (long) p, (long) sz, (long) pnew, file, line);
    if (!p) s_allocated_blocks++;
    return pnew;
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
        buffer [1024];

    apr_strerror (result, buffer, 1024);
    printf ("%s:%ld - %s (%ld: %s)\n", file, line, text, (long) result, buffer);
    assert (0);
}
