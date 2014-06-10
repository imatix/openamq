/*---------------------------------------------------------------------------
    icl_mem_fat.h - icl_mem_fat component

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

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_MEM_FAT_SAFE
#   define INCLUDE_ICL_MEM_FAT_SAFE
#   define INCLUDE_ICL_MEM_FAT_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_MEM_FAT_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_mem_fat_t icl_mem_fat_t;
#   ifdef __cplusplus
}
#   endif

#   include "icl_cache.h"
#   include "icl_mem_fat_list.h"
#   include "icl_system.h"
#   include "icl_stats.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MEM_FAT_ACTIVE
#   if defined (ICL_IMPORT_ICL_MEM_FAT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_MEM_FAT_UNSAFE
#   define INCLUDE_ICL_MEM_FAT_UNSAFE
#   define INCLUDE_ICL_MEM_FAT_ACTIVE

#   include "icl_cache.h"
#   include "icl_system.h"
#   include "icl_stats.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_mem_fat_animating;
//  Macros

#define ICL_MEM_FAT_SIZE (((sizeof (icl_mem_fat_t) + 7) / 8) * 8)

#define ICL_MEM_FAT_ASSERT_SANE(self, item)\
{\
    if (self->tag != ICL_CACHE_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Allocated item at %p expected tag=0x%x, actual tag=0x%x\n", item, ICL_CACHE_ALIVE, self->tag);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}

//  Global variables

extern Bool
    s_icl_mem_fat_active;

//  Structure of the icl_mem_fat object

struct _icl_mem_fat_t {
qbyte
    tag;                            //  To detect corruption
icl_cache_t
    *cache;                         //  The cache for this object
qbyte
    size;                           //  The size of this object
volatile icl_mem_fat_list_t *list_head;
volatile icl_mem_fat_t *list_prev;
volatile icl_mem_fat_t *list_next;
char 
    *file;                          //  Source file of last allocation
size_t
    line;                           //  Source line of last allocation
icl_callback_fn
    *callback;                      //  Function to callback memory contents
};

#   ifdef __cplusplus
}
#   endif
#   include "icl_mem_fat_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
icl_mem_fat_t *
    icl_mem_fat_new (
size_t size                             //  How much memory to allocate
);

void
    icl_mem_fat_destroy (
icl_mem_fat_t * ( * self_p )            //  The allocation block
);

#define icl_mem_fat_cache_reuse(cache)  icl_mem_fat_cache_reuse_ (cache, __FILE__, __LINE__)
void *
    icl_mem_fat_cache_reuse_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    icl_mem_fat_cache_purge (
icl_cache_t * cache                     //  The cache to purge
);

int
    icl_mem_fat_cache_assert (
icl_cache_t * cache,                    //  The cache for the allocation
FILE ** trace_file                      //  File to print to, opened on error
);

#define icl_mem_fat_possess(item)       icl_mem_fat_possess_ (item, __FILE__, __LINE__)
void
    icl_mem_fat_possess_ (
void * item,                            //  Client buffer to free
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    icl_mem_fat_set_callback (
void * item,                            //  Client buffer to free
icl_callback_fn * callback              //  Callback function to set
);

void
    icl_mem_fat_selftest (
void);

void
    icl_mem_fat_remove_from_all_containers (
icl_mem_fat_t * self                    //  The item
);

#define icl_mem_fat_cache_alloc(cache)  icl_mem_fat_cache_alloc_ (cache, __FILE__, __LINE__)
void *
    icl_mem_fat_cache_alloc_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_fat_cache_pop(cache)    icl_mem_fat_cache_pop_ (cache, __FILE__, __LINE__)
void *
    icl_mem_fat_cache_pop_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_fat_cache_new(cache)    icl_mem_fat_cache_new_ (cache, __FILE__, __LINE__)
void *
    icl_mem_fat_cache_new_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_fat_alloc(size)         icl_mem_fat_alloc_ (size, __FILE__, __LINE__)
void *
    icl_mem_fat_alloc_ (
size_t size,                            //  How much memory to allocate
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_fat_realloc(item,size)  icl_mem_fat_realloc_ (item, size, __FILE__, __LINE__)
void *
    icl_mem_fat_realloc_ (
void * item,                            //  Cient buffer to reallocate
qbyte size,                             //  How much memory to allocate
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    icl_mem_fat_free (
void * item                             //  Client buffer to free
);

void
    icl_mem_fat_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MEM_FAT_ACTIVE
#   if defined (ICL_IMPORT_ICL_MEM_FAT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_MEM_FAT_INLINE)
#   define INCLUDE_ICL_MEM_FAT_INLINE
#   define INCLUDE_ICL_MEM_FAT_ACTIVE

#   include "icl_cache.h"
#   include "icl_mem_fat_list.h"
#   include "icl_system.h"
#   include "icl_stats.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MEM_FAT_ACTIVE
#   if defined (ICL_IMPORT_ICL_MEM_FAT_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_MEM_FAT_ROOT
#   endif
# endif
#endif
