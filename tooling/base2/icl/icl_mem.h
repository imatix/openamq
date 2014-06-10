/*---------------------------------------------------------------------------
    icl_mem.h - icl_mem component

    Provides API for memory allocator.  Methods are implemented in icl_mem_fat
    and icl_mem_thin.
    Generated from icl_mem.icl by icl_gen using GSL/4.
    
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

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_MEM_SAFE
#   define INCLUDE_ICL_MEM_SAFE
#   define INCLUDE_ICL_MEM_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_MEM_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Types

typedef void (icl_mem_free_fn) (void *data);

typedef enum {
    ICL_MEM_UNDEFINED,
    ICL_MEM_FAT,
    ICL_MEM_THIN,
    ICL_MEM_DIRECT
} icl_mem_allocator_t;

//  Global variables

extern icl_mem_allocator_t
    icl_mem_allocator;
extern volatile void *
    icl_mem_total_used;
#   ifdef __cplusplus
}
#   endif

#   include "icl_mem_fat.h"
#   include "icl_mem_thin.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_MEM_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_MEM_UNSAFE
#   define INCLUDE_ICL_MEM_UNSAFE
#   define INCLUDE_ICL_MEM_ACTIVE

#   include "icl_mem_fat.h"
#   include "icl_mem_thin.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    icl_set_allocator (
icl_mem_allocator_t allocator           //  The allocator to use
);

void
    icl_mem_initialise (
void);

#define icl_mem_cache_reuse(cache)      icl_mem_cache_reuse_ (cache, __FILE__, __LINE__)
static inline void *
    icl_mem_cache_reuse_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_cache_alloc(cache)      icl_mem_cache_alloc_ (cache, __FILE__, __LINE__)
static inline void *
    icl_mem_cache_alloc_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_cache_pop(cache)        icl_mem_cache_pop_ (cache, __FILE__, __LINE__)
static inline void *
    icl_mem_cache_pop_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_cache_new(cache)        icl_mem_cache_new_ (cache, __FILE__, __LINE__)
static inline void *
    icl_mem_cache_new_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_alloc(size)             icl_mem_alloc_ (size, __FILE__, __LINE__)
static inline void *
    icl_mem_alloc_ (
size_t size,                            //  How much memory to allocate
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_realloc(item,size)      icl_mem_realloc_ (item, size, __FILE__, __LINE__)
static inline void *
    icl_mem_realloc_ (
void * item,                            //  Cient buffer to reallocate
qbyte size,                             //  How much memory to allocate
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static inline void
    icl_mem_free (
void * item                             //  Client buffer to free
);

#define icl_mem_possess(item)           icl_mem_possess_ (item, __FILE__, __LINE__)
static inline void
    icl_mem_possess_ (
void * item,                            //  Client buffer to free
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static inline void
    icl_mem_set_callback (
void * item,                            //  Client buffer to free
icl_callback_fn * callback              //  Callback function to set
);

void
    icl_mem_cache_purge (
icl_cache_t * cache                     //  The cache to purge
);

static inline void
    icl_mem_increment_used (
size_t size                             //  The amount by which to increment the total
);

static inline void
    icl_mem_decrement_used (
size_t size                             //  The amount by which to decrement the total
);

size_t
    icl_mem_used (
void);

#define icl_mem_strdup(string)          icl_mem_strdup_ (string, __FILE__, __LINE__)
static inline void *
    icl_mem_strdup_ (
void * string,                          //  String to duplicate
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static inline void
    icl_mem_strfree (
char * ( * client_p )                   //  Client buffer to free
);

void
    icl_mem_selftest (
void);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_MEM_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_MEM_INLINE)
#   define INCLUDE_ICL_MEM_INLINE
#   define INCLUDE_ICL_MEM_ACTIVE

#   include "icl_mem_fat.h"
#   include "icl_mem_thin.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_mem_cache_reuse

    Type: Component method
    Gets a block from a cache but does not allocate a new one if the cache is
    empty.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_cache_reuse_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_cache_reuse_ (cache, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_cache_reuse_ (cache, file, line);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_cache_alloc

    Type: Component method
    Allocate a block in a cache.  The size of the block is the size of the
    cache.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_cache_alloc_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_cache_alloc_ (cache, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_cache_alloc_ (cache, file, line);
else
    item = malloc (cache->size);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_cache_pop

    Type: Component method
    Pops a block from a cache.
    cache.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_cache_pop_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_cache_pop_ (cache, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_cache_pop_ (cache, file, line);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_cache_new

    Type: Component method
    Allocates a block in a cache.  The size of the block is the size of the
    cache.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_cache_new_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_cache_new_ (cache, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_cache_new_ (cache, file, line);
else
    item = malloc (cache->size);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_alloc

    Type: Component method
    Allocate a block in a cache.  The size of the block is the size of the
    cache.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_alloc_ (
    size_t size,                        //  How much memory to allocate
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_alloc_ (size, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_alloc_ (size, file, line);
else
    item = malloc (size);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_realloc

    Type: Component method
    Adjust the size of the pointed to 'buffer' to the new 'size'.
    If 'buffer' is NULL, self call behaves as a regular alloc().
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_realloc_ (
    void * item,                        //  Cient buffer to reallocate
    qbyte size,                         //  How much memory to allocate
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_realloc_ (item, size, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_realloc_ (item, size, file, line);
else
    item = realloc (item, size);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_free

    Type: Component method
    Destroys an allocation block.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_free (
    void * item                         //  Client buffer to free
)
{

if (icl_mem_allocator == ICL_MEM_THIN)
    icl_mem_thin_free (item);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    icl_mem_fat_free (item);
else
    free (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_possess

    Type: Component method
    Sets file and line number of memory allocation.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_possess_ (
    void * item,                        //  Client buffer to free
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

if (icl_mem_allocator == ICL_MEM_THIN)
    icl_mem_thin_possess_ (item, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    icl_mem_fat_possess_ (item, file, line);
}
/*  -------------------------------------------------------------------------
    icl_mem_set_callback

    Type: Component method
    Sets callback of memory allocation.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_set_callback (
    void * item,                        //  Client buffer to free
    icl_callback_fn * callback          //  Callback function to set
)
{

if (icl_mem_allocator == ICL_MEM_THIN)
    icl_mem_thin_set_callback (item, callback);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    icl_mem_fat_set_callback (item, callback);
}
/*  -------------------------------------------------------------------------
    icl_mem_increment_used

    Type: Component method
    Increment the record of the total memory allocations.

    This is non-trivial because it must be thread-safe, and it must work on
    64 bit architectures.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_increment_used (
    size_t size                         //  The amount by which to increment the total
)
{
void
    *old_used;

do
    old_used = (void *) icl_mem_total_used;
while (icl_atomic_casptr (&icl_mem_total_used, (byte *) old_used + size, old_used) != old_used);
}
/*  -------------------------------------------------------------------------
    icl_mem_decrement_used

    Type: Component method
    Decrement the record of the total memory allocations.

    This is non-trivial because it must be thread-safe, and it must work on
    64 bit architectures.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_decrement_used (
    size_t size                         //  The amount by which to decrement the total
)
{
void
    *old_used;

do
    old_used = (void *) icl_mem_total_used;
while (icl_atomic_casptr (&icl_mem_total_used, (byte *) old_used - size, old_used) != old_used);
}
/*  -------------------------------------------------------------------------
    icl_mem_strdup

    Type: Component method
    Duplicates the provided string, returns a fresh copy.  Accepts a null string
    argument, in which case it returns null.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_strdup_ (
    void * string,                      //  String to duplicate
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
qbyte
    len;
    void *
        item = NULL;                    //  The returned client pointer

if (string) {
    len = strlen ((char*) string) + 1;
    item = icl_mem_alloc_ (len, file, line);
    if (item)
        strncpy ((char*) item, (char*) string, len);
}
else
    item = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_strfree

    Type: Component method
    Frees the memory space pointed to by 'buffer'.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_strfree (
    char * ( * client_p )               //  Client buffer to free
)
{
    char *
        client = *client_p;             //  Dereferenced Client buffer to free

if (client) {
    icl_mem_free (client);
    *client_p = NULL;
}
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_MEM_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_MEM_ROOT
#   endif
# endif
#endif
