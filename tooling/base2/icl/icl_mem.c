/*---------------------------------------------------------------------------
    icl_mem.c - icl_mem component

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
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "icl_mem.h"                    //  Definitions for our class

//  Shorthands for class methods

#define self_set_allocator  icl_set_allocator
#define self_initialise     icl_mem_initialise
#define self_cache_reuse    icl_mem_cache_reuse
#define self_cache_alloc    icl_mem_cache_alloc
#define self_cache_pop      icl_mem_cache_pop
#define self_cache_new      icl_mem_cache_new
#define self_alloc          icl_mem_alloc
#define self_realloc        icl_mem_realloc
#define self_free           icl_mem_free
#define self_possess        icl_mem_possess
#define self_set_callback   icl_mem_set_callback
#define self_cache_purge    icl_mem_cache_purge
#define self_increment_used  icl_mem_increment_used
#define self_decrement_used  icl_mem_decrement_used
#define self_used           icl_mem_used
#define self_strdup         icl_mem_strdup
#define self_strfree        icl_mem_strfree
#define self_selftest       icl_mem_selftest
#define self_terminate      icl_mem_terminate

static void
    icl_mem_terminate (
void);

static Bool
    s_icl_mem_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_icl_mem_mutex          = NULL;
#endif
//  Global variables

//  Set default memory allocagtor depending on build model and
//  definition or otherwise of ICL_MEM_FAT/ICL_MEM_THIN.
icl_mem_allocator_t
    icl_mem_allocator;

volatile void *
    icl_mem_total_used = 0;
/*  -------------------------------------------------------------------------
    icl_set_allocator

    Type: Component method
    This method allows application control over the memory allocator to use.

    It affects all successive memory allocation, so should be called before
    initialisation.
    -------------------------------------------------------------------------
 */

void
    icl_set_allocator (
    icl_mem_allocator_t allocator       //  The allocator to use
)
{

icl_mem_allocator = allocator;
}
/*  -------------------------------------------------------------------------
    icl_mem_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_mem_initialise (
void)
{
apr_status_t
    status;
char
    *value;

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_icl_mem_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_icl_mem_mutex)
            s_icl_mem_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_icl_mem_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_icl_mem_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);
    //
    status = apr_env_get (&value, "ALLOCATOR", icl_global_pool);
    if (status == APR_SUCCESS) {
        if (streq (value, "thin"))
            icl_mem_allocator = ICL_MEM_THIN;
        else if (streq (value, "fat"))
            icl_mem_allocator = ICL_MEM_FAT;
        else if (streq (value, "direct"))
            icl_mem_allocator = ICL_MEM_DIRECT;
        else {
            icl_system_panic ("icl_mem", "Unknown allocator: %s, legal values are 'fat', 'thin', 'direct'", value);
            abort ();
        }
    }
    else
        //  If no value was provided choose a default based on value of DEBUG
#if defined (DEBUG)
        icl_mem_allocator = ICL_MEM_FAT;
#else
        icl_mem_allocator = ICL_MEM_DIRECT;
#endif
            s_icl_mem_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_icl_mem_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    icl_mem_cache_purge

    Type: Component method
    The 'purge' method frees half of the unused blocks.
    -------------------------------------------------------------------------
 */

void
    icl_mem_cache_purge (
    icl_cache_t * cache                 //  The cache to purge
)
{

if (icl_mem_allocator == ICL_MEM_THIN)
    icl_mem_thin_cache_purge (cache);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    icl_mem_fat_cache_purge (cache);
}
/*  -------------------------------------------------------------------------
    icl_mem_used

    Type: Component method
    Return the total amount of memory allocated.
    -------------------------------------------------------------------------
 */

size_t
    icl_mem_used (
void)
{
    size_t
        used;                           //  The returned value

//  Do this in a loop to prevent race condition giving a messed-up value
//  On 32-bit architecture, could use atomic assignment, but not on 64-bit.
do
    used = (size_t) icl_mem_total_used;
while (used != (size_t) icl_mem_total_used);

    return (used);
}
/*  -------------------------------------------------------------------------
    icl_mem_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_mem_selftest (
void)
{
char
    *str;

str = icl_mem_strdup ("icl_mem: Hello world");
icl_mem_strfree (&str);
assert (str == NULL);
}
/*  -------------------------------------------------------------------------
    icl_mem_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_mem_terminate (
void)
{

if (s_icl_mem_active) {
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_icl_mem_mutex);
#endif
        s_icl_mem_active = FALSE;
    }
}
//  Embed the version information in the resulting binary

char *EMBED__icl_mem_version_start        = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_mem_component            = "icl_mem ";
char *EMBED__icl_mem_version              = "1.1 ";
char *EMBED__icl_mem_copyright            = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_mem_filename             = "icl_mem.icl ";
char *EMBED__icl_mem_builddate            = "2011/03/01 ";
char *EMBED__icl_mem_version_end          = "VeRsIoNeNd:ipc";

