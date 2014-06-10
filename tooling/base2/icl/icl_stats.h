/*---------------------------------------------------------------------------
    icl_stats.h - icl_stats component

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

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_STATS_SAFE
#   define INCLUDE_ICL_STATS_SAFE
#   define INCLUDE_ICL_STATS_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_STATS_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

typedef volatile unsigned int icl_stats_t;
#   ifdef __cplusplus
}
#   endif

#   include "icl_apr.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_STATS_ACTIVE
#   if defined (ICL_IMPORT_ICL_STATS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_STATS_UNSAFE
#   define INCLUDE_ICL_STATS_UNSAFE
#   define INCLUDE_ICL_STATS_ACTIVE

#   include "icl_apr.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_stats_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
icl_stats_t *
    icl_stats_new (
const char * name                       //  The name of the statistic
);

void
    icl_stats_dump (
void);

static inline void
    icl_stats_inc (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat                     //  The statistic to increment
);

static inline void
    icl_stats_dec (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat                     //  The statistic to decrement
);

static inline void
    icl_stats_add (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat,                    //  The statistic to add to
icl_stats_t value                       //  The value to add to the statistic
);

static inline void
    icl_stats_assign (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat,                    //  The statistic to assign to
icl_stats_t value                       //  The value to assign to the statistic
);

static inline void
    icl_stats_max (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat,                    //  The statistic to add to
icl_stats_t value                       //  The value to add to the statistic
);

static inline void
    icl_stats_min (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat,                    //  The statistic to add to
icl_stats_t value                       //  The value to add to the statistic
);

void
    icl_stats_selftest (
void);

void
    icl_stats_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_STATS_ACTIVE
#   if defined (ICL_IMPORT_ICL_STATS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_STATS_INLINE)
#   define INCLUDE_ICL_STATS_INLINE
#   define INCLUDE_ICL_STATS_ACTIVE

#   include "icl_apr.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_stats_inc

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_inc (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat                 //  The statistic to increment
)
{

if (!*stat)
    *stat = icl_stats_new (name);
icl_atomic_inc32 (*stat);
}
/*  -------------------------------------------------------------------------
    icl_stats_dec

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_dec (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat                 //  The statistic to decrement
)
{

if (!*stat)
    *stat = icl_stats_new (name);
icl_atomic_dec32 (*stat);
}
/*  -------------------------------------------------------------------------
    icl_stats_add

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_add (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat,                //  The statistic to add to
    icl_stats_t value                   //  The value to add to the statistic
)
{

if (!*stat)
    *stat = icl_stats_new (name);
icl_atomic_add32 (*stat, value);
}
/*  -------------------------------------------------------------------------
    icl_stats_assign

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_assign (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat,                //  The statistic to assign to
    icl_stats_t value                   //  The value to assign to the statistic
)
{

if (!*stat)
    *stat = icl_stats_new (name);
icl_atomic_set32 (*stat, value);
}
/*  -------------------------------------------------------------------------
    icl_stats_max

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_max (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat,                //  The statistic to add to
    icl_stats_t value                   //  The value to add to the statistic
)
{
icl_stats_t
    cur_value,
    new_value;

if (!*stat)
    *stat = icl_stats_new (name);
cur_value = icl_atomic_get32 (*stat);
while (value > cur_value) {
    new_value = icl_atomic_cas32 (*stat, value, cur_value);
    if (new_value == cur_value)
        cur_value = value;
    else
        cur_value = new_value;
}
}
/*  -------------------------------------------------------------------------
    icl_stats_min

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_min (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat,                //  The statistic to add to
    icl_stats_t value                   //  The value to add to the statistic
)
{
icl_stats_t
    cur_value,
    new_value;

if (!*stat)
    *stat = icl_stats_new (name);
cur_value = icl_atomic_get32 (*stat);
while (value < cur_value) {
    new_value = icl_atomic_cas32 (*stat, value, cur_value);
    if (cur_value == cur_value)
        cur_value = value;
    else
        cur_value = new_value;
}
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_STATS_ACTIVE
#   if defined (ICL_IMPORT_ICL_STATS_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_STATS_ROOT
#   endif
# endif
#endif
