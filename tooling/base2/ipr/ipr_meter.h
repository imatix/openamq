/*---------------------------------------------------------------------------
    ipr_meter.h - ipr_meter component

    The traffic rate meter tracks activity per time segment.
    Generated from ipr_meter.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_METER_SAFE
#   define INCLUDE_IPR_METER_SAFE
#   define INCLUDE_IPR_METER_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_METER_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_meter_t ipr_meter_t;

#define IPR_METER_ALIVE                 0xFABB
#define IPR_METER_DEAD                  0xDEAD

#define IPR_METER_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_meter\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_METER_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_meter at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_METER_ALIVE, self->object_tag);\
        ipr_meter_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
//  Metering is done on small sample size

#define IPR_METER_HISTORY  10
#   ifdef __cplusplus
}
#   endif

#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_METER_ACTIVE
#   if defined (ICL_IMPORT_IPR_METER_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_METER_UNSAFE
#   define INCLUDE_IPR_METER_UNSAFE
#   define INCLUDE_IPR_METER_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_meter_animating;

//  Structure of the ipr_meter object

struct _ipr_meter_t {
dbyte
    object_tag;                     //  Object validity marker
qbyte
    sample,                         //  Current meter value
    last_sample,                    //  Previous meter value
    top_sample,                     //  Highest meter value
    history [IPR_METER_HISTORY];    //  Last set of samples
uint
    history_index;                  //  Current point in history
qbyte
    average,                        //  Last average value
    maximum,                        //  Last maximum value
    current;                        //  Last current value
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
static inline int
    ipr_meter_count (
ipr_meter_t * self                      //  Reference to object
);

int
    ipr_meter_mark (
ipr_meter_t * self,                     //  Reference to object
int interval                            //  Interval, in seconds
);

void
    ipr_meter_selftest (
void);

void
    ipr_meter_terminate (
void);

#define ipr_meter_new()                 ipr_meter_new_ (__FILE__, __LINE__)
ipr_meter_t *
    ipr_meter_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_meter_show(item,opcode,trace_file)  ipr_meter_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_meter_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_meter_destroy(self)         ipr_meter_destroy_ (self, __FILE__, __LINE__)
void
    ipr_meter_destroy_ (
ipr_meter_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    ipr_meter_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_meter_new_in_scope(self_p,_scope)  ipr_meter_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_meter_new_in_scope_ (
ipr_meter_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_METER_ACTIVE
#   if defined (ICL_IMPORT_IPR_METER_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_METER_INLINE)
#   define INCLUDE_IPR_METER_INLINE
#   define INCLUDE_IPR_METER_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    ipr_meter_count

    Type: Component method
    Accepts a ipr_meter_t reference and returns zero in case of success,
    1 in case of errors.
    Increments the current sample value.  You should call this for each
    activity you want to meter.  Note that the sample values can wrap around,
    this is intentional and works correctly.
    -------------------------------------------------------------------------
 */

static inline int
    ipr_meter_count (
    ipr_meter_t * self                  //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_METER_ASSERT_SANE (self);

icl_atomic_inc32 ((volatile qbyte *) &(self->sample));


    return (rc);
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_METER_ACTIVE
#   if defined (ICL_IMPORT_IPR_METER_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_METER_ROOT
#   endif
# endif
#endif
