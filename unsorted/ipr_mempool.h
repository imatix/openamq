/*---------------------------------------------------------------------------
    ipr_mempool.h - ipr_mempool component

    Defines the basic interface for iPR memory pool objects.

    Generated from ipr_mempool.icl by icl_gen using GSL/4.
    DO NOT MODIFY THIS FILE.

    Copyright (c) 2002-2004 iMatix Corporation
    Parts copyright (c) 1991-2004 iMatix Corporation.
    Licensed under the iMatix Generated Code License.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.

    For documentation and updates see http://www.imatix.com.
 *---------------------------------------------------------------------------*/

#if !defined (IMPORT_HEADERS) || IMPORT_HEADERS == 1
#ifndef INCLUDE_IPR_MEMPOOL_SAFE
#define INCLUDE_IPR_MEMPOOL_SAFE

#if !defined (IMPORT_HEADERS)
#   define IMPORT_IPR_MEMPOOL_ROOT
#   define IMPORT_HEADERS 1
#endif

/*  Our own class typedef                                                    */

typedef struct _ipr_mempool_t ipr_mempool_t;
#   include "ipr_mem.h"

/*  Abstract types for the class methods to allow indirection                */

typedef void *
    (ipr_mempool_new_fn) (void * pool);
typedef void *
    (ipr_mempool_alloc_fn) (void * pool, size_t size);
typedef void
    (ipr_mempool_destroy_fn) (void * pool);
typedef void
    (ipr_mempool_selftest_fn) (void);
typedef void
    (ipr_mempool_animate_fn) (Bool enabled);

#   if defined (IMPORT_IPR_MEMPOOL_ROOT)
#       undef  IMPORT_HEADERS
#       define IMPORT_HEADERS 2
#   endif
#endif
#endif

#if !defined (IMPORT_HEADERS) || IMPORT_HEADERS == 2
#ifndef INCLUDE_IPR_MEMPOOL_UNSAFE
#define INCLUDE_IPR_MEMPOOL_UNSAFE
#   include "ipr_mem.h"

/*  ipr_mempool_t properties, one instance per object                        */

struct _ipr_mempool_t {
    size_t size;
    byte  *data;
};

/*  Public method prototypes                                                 */

#   ifdef __cplusplus
extern "C" {
#   endif

void *
    ipr_mempool_new (void * pool);
void *
    ipr_mempool_alloc (void * pool, size_t size);
void
    ipr_mempool_destroy (void * pool);
void
    ipr_mempool_selftest (void);
void
    ipr_mempool_animate (Bool enabled);

#   ifdef __cplusplus
}
#   endif

#   if defined (IMPORT_IPR_MEMPOOL_ROOT)
#       undef IMPORT_HEADERS
#       undef IMPORT_IPR_MEMPOOL_ROOT
#   endif
#endif
#endif
