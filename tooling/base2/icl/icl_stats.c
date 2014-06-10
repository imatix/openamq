/*---------------------------------------------------------------------------
    icl_stats.c - icl_stats component

    Provides statistical reporting for other classes.

    Statistics are held in a shared memory block so they can easily be
    examined from another process, even if the running process is blocked.
    Generated from icl_stats.icl by icl_gen using GSL/4.
    
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
#include "icl_stats.h"                  //  Definitions for our class

//  Shorthands for class methods

#define self_initialise     icl_stats_initialise
#define self_terminate      icl_stats_terminate
#define self_new            icl_stats_new
#define self_dump           icl_stats_dump
#define self_inc            icl_stats_inc
#define self_dec            icl_stats_dec
#define self_add            icl_stats_add
#define self_assign         icl_stats_assign
#define self_max            icl_stats_max
#define self_min            icl_stats_min
#define self_selftest       icl_stats_selftest
#define self_show_animation  icl_stats_show_animation

static void
    icl_stats_initialise (
void);

static void
    icl_stats_terminate (
void);

Bool
    icl_stats_animating = TRUE;         //  Animation enabled by default
#define ICL_STATS_SHARE_SIZE         32768
#define ICL_STATS_SHARE_DEFAULT_NAME "icl_stats"

#define BYTE_ALIGNMENT               4

//  Static variables

static apr_shm_t
    *s_share = NULL;
static char
    *s_share_start,
    *s_share_end;

static Bool
    s_icl_stats_active = FALSE;
#if (defined (BASE_THREADSAFE))
static apr_thread_mutex_t
    *s_icl_stats_mutex  = NULL;
#endif
/*  -------------------------------------------------------------------------
    icl_stats_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_stats_initialise (
void)
{
char
    share_name [128];

#if (defined (BASE_THREADSAFE))
    //  Don't use the icl lock functions or we'll get recursion.
    apr_thread_mutex_lock (icl_global_mutex);
    if (!s_icl_stats_mutex)
        icl_apr_assert (apr_thread_mutex_create (&s_icl_stats_mutex, APR_THREAD_MUTEX_DEFAULT, icl_global_pool));
    apr_thread_mutex_lock   (s_icl_stats_mutex);
    apr_thread_mutex_unlock (icl_global_mutex);
#endif
    if (!s_icl_stats_active) {
        s_icl_stats_active = TRUE;

        if (g_argc > 0) {
            strncpy (share_name, g_argv [0], 120);
            share_name [120] = 0;
        }
        else
            share_name [0] = 0;

        strcat (share_name, "_stats");

        //  Remove share before creating it to avoid clashes.
        apr_shm_remove (share_name, icl_global_pool);
        icl_apr_assert (apr_shm_create (
            &s_share, ICL_STATS_SHARE_SIZE, share_name, icl_global_pool));
        s_share_start = apr_shm_baseaddr_get (s_share);
        s_share_end   = s_share_start;
        *s_share_end  = 0;

        //  Register the class termination call-back functions
        icl_system_register (NULL, icl_stats_terminate);
    }
#if (defined (BASE_THREADSAFE))
    apr_thread_mutex_unlock (s_icl_stats_mutex);
#endif
}
/*  -------------------------------------------------------------------------
    icl_stats_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_stats_terminate (
void)
{

    if (s_icl_stats_active) {
#if (defined (BASE_THREADSAFE))
        apr_thread_mutex_destroy (s_icl_stats_mutex);
        s_icl_stats_mutex = NULL;
#endif
        s_icl_stats_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    icl_stats_new

    Type: Component method
    -------------------------------------------------------------------------
 */

icl_stats_t *
    icl_stats_new (
    const char * name                   //  The name of the statistic
)
{
size_t
    remaining_space,
    name_length;
    icl_stats_t *
        pointer = 0;                    //  Pointer to 32-bit statistic

    if (!s_icl_stats_active)
        icl_stats_initialise ();

#if (defined (BASE_THREADSAFE))
    apr_thread_mutex_lock (s_icl_stats_mutex);
#endif
    remaining_space = ICL_STATS_SHARE_SIZE - (s_share_end - s_share_start);
    name_length = strlen (name) + 1;    //  Including null terminator
    name_length = ((name_length + BYTE_ALIGNMENT - 1) / BYTE_ALIGNMENT) * BYTE_ALIGNMENT;

    if (remaining_space >= name_length + 5) {
        strncpy (s_share_end, name, name_length);
        pointer = (icl_stats_t *) ((byte *) s_share_end + name_length);
        *pointer = 0;
        s_share_end += name_length + 4;
        *s_share_end = 0;
    }
    else {
        icl_system_panic ("icl_stats", "E: shared memory too small, increase ICL_STATS_SHARE_SIZE");
        abort ();
    }
#if (defined (BASE_THREADSAFE))
    apr_thread_mutex_unlock (s_icl_stats_mutex);
#endif

    return (pointer);
}
/*  -------------------------------------------------------------------------
    icl_stats_dump

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_stats_dump (
void)
{
char *
    start;
size_t
    name_length;

icl_console_print ("----------------- START OF STATISTICS DUMP -----------------");
start = s_share_start;
while (start < s_share_end) {
    name_length = strlen (start) + 1;    //  Including null terminator
    name_length = ((name_length + BYTE_ALIGNMENT - 1) / BYTE_ALIGNMENT) * BYTE_ALIGNMENT;
    icl_console_print ("%s: %u", start, * (icl_stats_t *) (start + name_length));
    start += name_length + 4;
}
icl_console_print ("------------------ END OF STATISTICS DUMP ------------------");
}
/*  -------------------------------------------------------------------------
    icl_stats_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_stats_selftest (
void)
{
icl_stats_t
    *stat;

stat = icl_stats_new ("My test statistic");
icl_stats_assign ("My test statistic", &stat, 123456789);
icl_stats_add    (NULL, &stat, 876543210);
}
/*  -------------------------------------------------------------------------
    icl_stats_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_stats_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_stats_animating = enabled;
}
//  Embed the version information in the resulting binary

char *EMBED__icl_stats_version_start      = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_stats_component          = "icl_stats ";
char *EMBED__icl_stats_version            = "1.1 ";
char *EMBED__icl_stats_copyright          = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_stats_filename           = "icl_stats.icl ";
char *EMBED__icl_stats_builddate          = "2011/03/01 ";
char *EMBED__icl_stats_version_end        = "VeRsIoNeNd:ipc";

