/*---------------------------------------------------------------------------
    ipr_mempool.c - ipr_mempool component

    Defines the basic interface for iPR memory pool objects.

    Generated from ipr_mempool.icl by icl_gen using GSL/4.
    DO NOT MODIFY THIS FILE.

    Copyright (c) 2002-2004 iMatix Corporation
    Parts copyright (c) 1991-2004 iMatix Corporation.
    Licensed under the iMatix Generated Code License.
    For documentation and updates see http://www.imatix.com.
 *---------------------------------------------------------------------------*/

#include "sfl.h"
#include "ipr_mempool.h"

static Bool
    s_ipr_mempool_animate = FALSE;      /*  Animation enabled                */

/*  Embed the version information in the resulting binary                    */

char *ipr_mempool_version_start  = "VeRsIoNsTaRt:ipc";
char *ipr_mempool_component      = "ipr_mempool";
char *ipr_mempool_version        = "1.0a1";
char *ipr_mempool_copyright      = "Copyright (c) 2002-2004 iMatix Corporation";
char *ipr_mempool_filename       = "ipr_mempool.icl";
char *ipr_mempool_script         = "icl_gen";
char *ipr_mempool_builddate      = "2004/10/22";
char *ipr_mempool_version_end    = "VeRsIoNeNd:ipc";

/*  -------------------------------------------------------------------------
    ipr_mempool_new

    Type: Component method
        Creates a new memory pool.  If 'pool' is not NULL then a subpool of
        that pool is returned.
    -------------------------------------------------------------------------
 */

void *
    ipr_mempool_new (void * pool)
{
    /*
     * For now, we don't use APR, and simply use malloc - subpooling
     * not available.
     */
    if (pool == NULL) {
        pool = ipr_mem_alloc (sizeof (ipr_mempool_t));
        memset (pool, 0, sizeof (ipr_mempool_t));
    }
    return (pool);
}


/*  -------------------------------------------------------------------------
    ipr_mempool_alloc

    Type: Component method
        Allocate 'size' bytes of memory from the 'pool' and return a pointer
        to it.
    -------------------------------------------------------------------------
 */

void *
    ipr_mempool_alloc (void * pool, size_t size)
{
    void *
        ret = NULL;                     /*                                   */

        ipr_mempool_t *mypool = (ipr_mempool_t *)pool;

    /*
     * We simply add memory to the pool and return a pointer to that.
     */
    if (mypool != NULL) {
        mypool->data  = ipr_mem_realloc (mypool->data, mypool->size + size);
        ret           = mypool->data + mypool->size;
        mypool->size += size;
    }
    return (ret);
}


/*  -------------------------------------------------------------------------
    ipr_mempool_destroy

    Type: Component method
        Free everything in the pool.
    -------------------------------------------------------------------------
 */

void
    ipr_mempool_destroy (void * pool)
{
        ipr_mempool_t *mypool = (ipr_mempool_t *)pool;

    if (mypool != NULL) {
        if (mypool->data != NULL) {
            ipr_mem_free (mypool->data);
        }
        memset (mypool, 0, sizeof (ipr_mempool_t));
    }
}


/*  -------------------------------------------------------------------------
    ipr_mempool_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_mempool_selftest (void)
{
        ipr_mempool_t *pool;
        char        *buffer;

    pool   = ipr_mempool_new (NULL);
    buffer = ipr_mempool_alloc (pool, 50);
    buffer = ipr_mempool_alloc (pool, 100);
    ipr_mempool_destroy (pool);
}


/*  -------------------------------------------------------------------------
    ipr_mempool_animate

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_mempool_animate (Bool enabled)
{
    s_ipr_mempool_animate = enabled;
}

