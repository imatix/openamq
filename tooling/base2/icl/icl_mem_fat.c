/*---------------------------------------------------------------------------
    icl_mem_fat.c - icl_mem_fat component

    A memory allocator using fully linked lists and providing debug features.
    Generated from icl_mem_fat.icl by icl_gen using GSL/4.
    
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
#include "icl_mem_fat.h"                //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_mem_fat_t

//  Shorthands for class methods

#define self_new            icl_mem_fat_new
#define self_destroy        icl_mem_fat_destroy
#define self_cache_insert   icl_mem_fat_cache_insert
#define self_cache_reuse    icl_mem_fat_cache_reuse
#define self_cache_recycle  icl_mem_fat_cache_recycle
#define self_cache_purge    icl_mem_fat_cache_purge
#define self_cache_reset    icl_mem_fat_cache_reset
#define self_cache_assert   icl_mem_fat_cache_assert
#define self_possess        icl_mem_fat_possess
#define self_set_callback   icl_mem_fat_set_callback
#define self_selftest       icl_mem_fat_selftest
#define self_remove_from_all_containers  icl_mem_fat_remove_from_all_containers
#define self_cache_alloc    icl_mem_fat_cache_alloc
#define self_cache_pop      icl_mem_fat_cache_pop
#define self_cache_new      icl_mem_fat_cache_new
#define self_alloc          icl_mem_fat_alloc
#define self_realloc        icl_mem_fat_realloc
#define self_free           icl_mem_fat_free
#define self_show_animation  icl_mem_fat_show_animation

static void
    icl_mem_fat_cache_insert (
icl_cache_t * cache,                    //  The cache for the allocation
icl_mem_fat_t * self                    //  The allocation block
);

static void
    icl_mem_fat_cache_recycle (
icl_mem_fat_t * ( * self_p )            //  The allocation block
);

static void
    icl_mem_fat_cache_reset (
icl_mem_fat_t * self                    //  The allocation block
);

Bool
    icl_mem_fat_animating = TRUE;       //  Animation enabled by default
//  Global variables

Bool
    s_icl_mem_fat_active = FALSE;
/*  -------------------------------------------------------------------------
    icl_mem_fat_new

    Type: Component method
    Allocate a block.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

icl_mem_fat_t *
    icl_mem_fat_new (
    size_t size                         //  How much memory to allocate
)
{
    icl_mem_fat_t *
        self;                           //  The allocation block

self = malloc (ICL_MEM_FAT_SIZE + size);

//  Increment the record of total memory allocations
icl_mem_increment_used (ICL_MEM_FAT_SIZE + size);

if (self) {
    self->cache = NULL;
    self->size  = size;
self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
self->file     = NULL;
self->line     = 0;
self->callback = NULL;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_destroy

    Type: Component method
    Destroy an allocated block.
    -------------------------------------------------------------------------
 */

void
    icl_mem_fat_destroy (
    icl_mem_fat_t * ( * self_p )        //  The allocation block
)
{
    icl_mem_fat_t *
        self = *self_p;                 //  Dereferenced The allocation block

if (self) {
icl_mem_fat_list_remove (self);
    //  Decrement the record of total memory allocations
    icl_mem_decrement_used (ICL_MEM_FAT_SIZE + self->size);

    free (self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_cache_insert

    Type: Component method
    This is a private method used by icl_mem_fat_cache_alloc.

    Its purpose is to record an allocator block in use.
    -------------------------------------------------------------------------
 */

static void
    icl_mem_fat_cache_insert (
    icl_cache_t * cache,                //  The cache for the allocation
    icl_mem_fat_t * self                //  The allocation block
)
{

#if (defined (BASE_STATS) || defined (BASE_STATS_ICL_MEM_FAT))
    icl_stats_inc (NULL, &cache->used_stat);
#endif

    icl_mem_fat_list_queue (cache->fat_used_list, self);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_cache_reuse

    Type: Component method
    This is a private method used by icl_mem_fat_cache_alloc.

    Its purpose is to find an allocator block from the cache for reuse.
    -------------------------------------------------------------------------
 */

void *
    icl_mem_fat_cache_reuse_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        self = NULL;                    //  The returned client pointer

self = icl_mem_fat_list_pop (cache->fat_unused_list);
#if (defined (BASE_STATS) || defined (BASE_STATS_ICL_MEM_FAT))
    if (self)
        icl_stats_dec (NULL, &cache->unused_stat);
#endif

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_cache_recycle

    Type: Component method
    This is a private method used by icl_mem_fat_cache_alloc.

    Its purpose is put an allocated block in the unused list of the cache.
    -------------------------------------------------------------------------
 */

static void
    icl_mem_fat_cache_recycle (
    icl_mem_fat_t * ( * self_p )        //  The allocation block
)
{
    icl_mem_fat_t *
        self = *self_p;                 //  Dereferenced The allocation block
    icl_cache_t *
        cache = self->cache;            //  The cache for the allocation

#if (defined (BASE_STATS) || defined (BASE_STATS_ICL_MEM_FAT))
    icl_stats_dec (NULL, &cache->used_stat);
#endif
icl_mem_fat_list_remove (self);
if (cache->size)                    //  ie not direct cache
    icl_mem_fat_list_queue (cache->fat_unused_list, self);
#if (defined (BASE_STATS) || defined (BASE_STATS_ICL_MEM_FAT))
    if (cache->size)                    //  ie not direct cache
        icl_stats_inc (NULL, &cache->unused_stat);
#endif
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_cache_purge

    Type: Component method
    The 'purge' method frees half of the unused blocks.
    -------------------------------------------------------------------------
 */

void
    icl_mem_fat_cache_purge (
    icl_cache_t * cache                 //  The cache to purge
)
{
qbyte
    target_unused_count;
icl_mem_fat_t
    *self;

target_unused_count = icl_mem_fat_list_count (cache->fat_unused_list) / 2;
while (icl_mem_fat_list_count (cache->fat_unused_list) > target_unused_count) {
    self = icl_mem_fat_cache_reuse (cache);
    icl_mem_fat_destroy (&self);
}
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_cache_reset

    Type: Component method
    This is a private method used by icl_mem_fat_cache_realloc.

    Its purpose is to reset the block's cache pointers following a realloc.
    -------------------------------------------------------------------------
 */

static void
    icl_mem_fat_cache_reset (
    icl_mem_fat_t * self                //  The allocation block
)
{

//  Manipulate the list pointers manually as this is an unusual case.
self->list_next = self;
self->list_prev = self;
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_cache_assert

    Type: Component method
    This is a private method used by icl_mem_fat_cache_alloc.

    Its purpose is report on leaked allocations in the cache.
    -------------------------------------------------------------------------
 */

int
    icl_mem_fat_cache_assert (
    icl_cache_t * cache,                //  The cache for the allocation
    FILE ** trace_file                  //  File to print to, opened on error
)
{
icl_mem_fat_t
    *self;
    int
        rc = 0;                         //  Return code

self = icl_mem_fat_list_first (cache->fat_used_list);
if (self) {
    rc = -1;
    if (*trace_file == NULL) {
        icl_console_print ("E: Active cache allocation details are in icl_mem_fat.xml");
        *trace_file = fopen ("icl_mem_fat.xml", "w");
        fprintf (*trace_file, "<?xml version=\"1.0\"?>\n");
    }
    icl_console_print ("E: %lu active cache allocations of size: %lu",
                       (long) icl_mem_fat_list_count (cache->fat_used_list),
                       (long) cache->size);
    fprintf (*trace_file, "<list>\n");
    while (self) {
        if (self->callback)
            (self->callback) (((byte *) self) + ICL_MEM_FAT_SIZE, ICL_CALLBACK_DUMP, *trace_file, self->file, self->line);
        else
            fprintf (*trace_file,
                    "    <block-size=\"%lu\" file=\"%s\" line=\"%lu\" pointer=\"%pp\"/>\n",
                    (long) cache->size,
                    self->file ? self->file : "<Unknown>",
                    (long) self->line,
                    (byte *) self + ICL_MEM_FAT_SIZE);

        self = icl_mem_fat_list_next (&self);
    }
    fprintf (*trace_file, "</list>\n");
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_possess

    Type: Component method
    Sets file and line number of memory allocation.
    -------------------------------------------------------------------------
 */

void
    icl_mem_fat_possess_ (
    void * item,                        //  Client buffer to free
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
icl_mem_fat_t
    *self;

if (item) {
    self = (icl_mem_fat_t *) (((byte *) item) - ICL_MEM_FAT_SIZE);
    ICL_MEM_FAT_ASSERT_SANE (self, item);
    self->file = file;
    self->line = line;
}
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_set_callback

    Type: Component method
    Sets callback of memory allocation.
    -------------------------------------------------------------------------
 */

void
    icl_mem_fat_set_callback (
    void * item,                        //  Client buffer to free
    icl_callback_fn * callback          //  Callback function to set
)
{
icl_mem_fat_t
    *self;

if (item) {
    self = (icl_mem_fat_t *) (((byte *) item) - ICL_MEM_FAT_SIZE);
    ICL_MEM_FAT_ASSERT_SANE (self, item);
    self->callback = callback;
}
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_mem_fat_selftest (
void)
{
icl_cache_t
    *cache;
void
    *buffer;

//
cache  = icl_cache_get (500);
buffer = icl_mem_fat_cache_alloc (cache);
icl_mem_fat_free (buffer);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    icl_mem_fat_remove_from_all_containers (
    icl_mem_fat_t * self                //  The item
)
{

icl_mem_fat_list_remove (self);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_cache_alloc

    Type: Component method
    Reuses or allocates a block in a cache.  The size of the block is the size
    of the cache.
    -------------------------------------------------------------------------
 */

void *
    icl_mem_fat_cache_alloc_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

item = icl_mem_fat_cache_pop_ (cache, file, line);
if (!item)
    item = icl_mem_fat_cache_new_ (cache, file, line);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_cache_pop

    Type: Component method
    Pops a block from a cache.
    cache.
    -------------------------------------------------------------------------
 */

void *
    icl_mem_fat_cache_pop_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
icl_mem_fat_t
    *self;
    void *
        item = NULL;                    //  The returned client pointer

    self = icl_mem_fat_cache_reuse_ (cache, file, line);
    if (self) {
#if (defined (BASE_STATS) || defined (BASE_STATS_ICL_MEM_FAT))
        icl_stats_inc (NULL, &cache->allocs_stat);
#endif
        self->tag = ICL_CACHE_ALIVE;
        icl_mem_fat_cache_insert (cache, self);
        item = (byte *) self + ICL_MEM_FAT_SIZE;
        icl_mem_fat_possess_ (item, file, line);
    }

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_cache_new

    Type: Component method
    Allocates a block in a cache.  The size of the block is the size of the
    cache.
    -------------------------------------------------------------------------
 */

void *
    icl_mem_fat_cache_new_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
icl_mem_fat_t
    *self;
    void *
        item = NULL;                    //  The returned client pointer

    self = icl_mem_fat_new (cache->size);
    if (self) {
        self->cache = cache;
#if (defined (BASE_STATS) || defined (BASE_STATS_ICL_MEM_FAT))
        icl_stats_inc (NULL, &cache->allocs_stat);
#endif
        self->tag = ICL_CACHE_ALIVE;
        icl_mem_fat_cache_insert (cache, self);
        item = (byte *) self + ICL_MEM_FAT_SIZE;
        icl_mem_fat_possess_ (item, file, line);
    }

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_alloc

    Type: Component method
    Allocate a block of a given size in the direct cache.  This isn't really
    a cache at all and just serves to track allocated blocks.
    cache.
    -------------------------------------------------------------------------
 */

void *
    icl_mem_fat_alloc_ (
    size_t size,                        //  How much memory to allocate
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
icl_mem_fat_t
    *self;
    void *
        item = NULL;                    //  The returned client pointer

    if (!s_direct_cache)
        icl_cache_initialise ();

    self = icl_mem_fat_new (size);
    if (self) {
        self->cache = s_direct_cache;
        self->size  = size;
#if (defined (BASE_STATS) || defined (BASE_STATS_ICL_MEM_FAT))
        icl_stats_inc (NULL, &s_direct_cache->allocs_stat);
#endif
        self->tag = ICL_CACHE_ALIVE;
        icl_mem_fat_cache_insert (s_direct_cache, self);
        item = (byte *) self + ICL_MEM_FAT_SIZE;
        icl_mem_fat_possess_ (item, file, line);
    }

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_realloc

    Type: Component method
    Adjust the size of the pointed to 'buffer' to the new 'size'.
    If 'buffer' is NULL, self call behaves as a regular alloc().
    -------------------------------------------------------------------------
 */

void *
    icl_mem_fat_realloc_ (
    void * item,                        //  Cient buffer to reallocate
    qbyte size,                         //  How much memory to allocate
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
icl_mem_fat_t
    *self;

if (item == NULL)
    item = icl_mem_fat_alloc_ (size, file, line);
else {
    self = (icl_mem_fat_t *) (((byte *) item) - ICL_MEM_FAT_SIZE);
    ICL_MEM_FAT_ASSERT_SANE (self, item);

    item = NULL;
    if (!self->cache->size) {       //  ie direct cache
        icl_mem_fat_cache_recycle (&self);
        self = (icl_mem_fat_t*) realloc (self, ICL_MEM_FAT_SIZE + size);
        if (self) {
            //  Need to reset the cache pointer(s) following realloc.
            icl_mem_fat_cache_reset (self);

            self->size  = size;
            self->tag = ICL_CACHE_ALIVE;
            icl_mem_fat_cache_insert (s_direct_cache, self);

            item = (byte *) self + ICL_MEM_FAT_SIZE;
        }
    }
}

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_free

    Type: Component method
    Destroys an allocation block.
    -------------------------------------------------------------------------
 */

void
    icl_mem_fat_free (
    void * item                         //  Client buffer to free
)
{
icl_mem_fat_t
    *self;

    if (item) {
        self = (icl_mem_fat_t *) (((byte *) item) - ICL_MEM_FAT_SIZE);
        ICL_MEM_FAT_ASSERT_SANE (self, item);
        self->tag = ICL_CACHE_DEAD;

        icl_mem_fat_cache_recycle (&self);
        if (!self->cache->size) {       //  ie direct cache
            icl_mem_fat_destroy (&self);
#if (defined (BASE_STATS) || defined (BASE_STATS_ICL_MEM_FAT))
            icl_stats_inc (NULL, &s_direct_cache->frees_stat);
#endif
        }
    }
}
/*  -------------------------------------------------------------------------
    icl_mem_fat_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_mem_fat_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_mem_fat_animating = enabled;
}
//  Embed the version information in the resulting binary

char *EMBED__icl_mem_fat_version_start    = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_mem_fat_component        = "icl_mem_fat ";
char *EMBED__icl_mem_fat_version          = "1.1 ";
char *EMBED__icl_mem_fat_copyright        = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_mem_fat_filename         = "icl_mem_fat.icl ";
char *EMBED__icl_mem_fat_builddate        = "2011/03/01 ";
char *EMBED__icl_mem_fat_version_end      = "VeRsIoNeNd:ipc";

