/*---------------------------------------------------------------------------
    icl_mem_thin.c - icl_mem_thin component

    A lightwieght memory allocator using a lock-free singly-linked stack.
    Generated from icl_mem_thin.icl by icl_gen using GSL/4.
    
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
#include "icl_mem_thin.h"               //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_mem_thin_t

//  Shorthands for class methods

#define self_cache_alloc    icl_mem_thin_cache_alloc
#define self_alloc          icl_mem_thin_alloc
#define self_realloc        icl_mem_thin_realloc
#define self_free           icl_mem_thin_free
#define self_cache_insert   icl_mem_thin_cache_insert
#define self_cache_reuse    icl_mem_thin_cache_reuse
#define self_cache_recycle  icl_mem_thin_cache_recycle
#define self_cache_purge    icl_mem_thin_cache_purge
#define self_cache_reset    icl_mem_thin_cache_reset
#define self_cache_assert   icl_mem_thin_cache_assert
#define self_show           icl_mem_thin_show
#define self_possess        icl_mem_thin_possess
#define self_set_callback   icl_mem_thin_set_callback
#define self_selftest       icl_mem_thin_selftest
#define self_new            icl_mem_thin_new
#define self_remove_from_all_containers  icl_mem_thin_remove_from_all_containers
#define self_cache_pop      icl_mem_thin_cache_pop
#define self_cache_new      icl_mem_thin_cache_new
#define self_destroy        icl_mem_thin_destroy
#define self_show_animation  icl_mem_thin_show_animation

Bool
    icl_mem_thin_animating = TRUE;      //  Animation enabled by default
//  Global variables

Bool
    s_icl_mem_thin_active = FALSE;
/*  -------------------------------------------------------------------------
    icl_mem_thin_cache_purge

    Type: Component method
    The 'purge' method frees half of the unused blocks.
    -------------------------------------------------------------------------
 */

void
    icl_mem_thin_cache_purge (
    icl_cache_t * cache                 //  The cache to purge
)
{
qbyte
    target_unused_count;
icl_mem_thin_t
    *self;

target_unused_count = icl_mem_thin_list_count (cache->thin_unused_list) / 2;
while (icl_mem_thin_list_count (cache->thin_unused_list) > target_unused_count) {
    self = icl_mem_thin_cache_reuse (cache);
    if (self)
        icl_mem_thin_destroy (&self);
    else
        break;
}
}
/*  -------------------------------------------------------------------------
    icl_mem_thin_cache_assert

    Type: Component method
    This is a private method used by icl_mem_thin_cache_alloc.

    Its purpose is report on leaked allocations in the cache.
    -------------------------------------------------------------------------
 */

int
    icl_mem_thin_cache_assert (
    icl_cache_t * cache,                //  The cache for the allocation
    FILE ** trace_file                  //  File to print to, opened on error
)
{
    int
        rc = 0;                         //  Return code

if (cache->thin_used_count) {
    rc = -1;
    icl_console_print ("E: %lu active cache allocations of size: %lu.",
                       (long) cache->thin_used_count, (long) cache->size);
    icl_console_print ("E: Rebuild with fat memory allocator for details");
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_mem_thin_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_mem_thin_show (
void)
{

}
/*  -------------------------------------------------------------------------
    icl_mem_thin_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_mem_thin_selftest (
void)
{
icl_cache_t
    *cache;
void
    *buffer;

cache  = icl_cache_get (500);
buffer = icl_mem_thin_cache_alloc (cache);
icl_mem_thin_free (buffer);
}
/*  -------------------------------------------------------------------------
    icl_mem_thin_new

    Type: Component method
    Allocate a block.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

icl_mem_thin_t *
    icl_mem_thin_new (
    size_t size                         //  How much memory to allocate
)
{
    icl_mem_thin_t *
        self;                           //  The allocation block

self = malloc (ICL_MEM_THIN_SIZE + size);

//  Increment the record of total memory allocations
icl_mem_increment_used (ICL_MEM_THIN_SIZE + size);

if (self) {
    self->cache = NULL;
    self->size  = size;
self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_mem_thin_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    icl_mem_thin_remove_from_all_containers (
    icl_mem_thin_t * self               //  The item
)
{

icl_mem_thin_list_remove (self);
}
/*  -------------------------------------------------------------------------
    icl_mem_thin_cache_pop

    Type: Component method
    Pops a block from a cache.
    cache.
    -------------------------------------------------------------------------
 */

void *
    icl_mem_thin_cache_pop_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
icl_mem_thin_t
    *self;
    void *
        item = NULL;                    //  The returned client pointer

    self = icl_mem_thin_cache_reuse_ (cache, file, line);
    if (self) {
#if (defined (BASE_STATS) || defined (BASE_STATS_ICL_MEM_THIN))
        icl_stats_inc (NULL, &cache->allocs_stat);
#endif
        self->tag = ICL_CACHE_ALIVE;
        icl_mem_thin_cache_insert (cache, self);
        item = (byte *) self + ICL_MEM_THIN_SIZE;
        icl_mem_thin_possess_ (item, file, line);
    }

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_thin_cache_new

    Type: Component method
    Allocates a block in a cache.  The size of the block is the size of the
    cache.
    -------------------------------------------------------------------------
 */

void *
    icl_mem_thin_cache_new_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
icl_mem_thin_t
    *self;
    void *
        item = NULL;                    //  The returned client pointer

    self = icl_mem_thin_new (cache->size);
    if (self) {
        self->cache = cache;
#if (defined (BASE_STATS) || defined (BASE_STATS_ICL_MEM_THIN))
        icl_stats_inc (NULL, &cache->allocs_stat);
#endif
        self->tag = ICL_CACHE_ALIVE;
        icl_mem_thin_cache_insert (cache, self);
        item = (byte *) self + ICL_MEM_THIN_SIZE;
        icl_mem_thin_possess_ (item, file, line);
    }

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_thin_destroy

    Type: Component method
    Destroy an allocated block.
    -------------------------------------------------------------------------
 */

void
    icl_mem_thin_destroy (
    icl_mem_thin_t * ( * self_p )       //  The allocation block
)
{
    icl_mem_thin_t *
        self = *self_p;                 //  Dereferenced The allocation block

if (self) {
    //  Decrement the record of total memory allocations
    icl_mem_decrement_used (ICL_MEM_THIN_SIZE + self->size);

    free (self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_mem_thin_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_mem_thin_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_mem_thin_animating = enabled;
}
//  Embed the version information in the resulting binary

char *EMBED__icl_mem_thin_version_start   = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_mem_thin_component       = "icl_mem_thin ";
char *EMBED__icl_mem_thin_version         = "1.1 ";
char *EMBED__icl_mem_thin_copyright       = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_mem_thin_filename        = "icl_mem_thin.icl ";
char *EMBED__icl_mem_thin_builddate       = "2011/03/01 ";
char *EMBED__icl_mem_thin_version_end     = "VeRsIoNeNd:ipc";

