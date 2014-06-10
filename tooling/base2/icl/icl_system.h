/*---------------------------------------------------------------------------
    icl_system.h - icl_system component

    Implements the metacache manager.  Provides two functions for
    application developers: icl_system_purge(), which does garbage
    collection, and icl_system_terminate(), which does final destruction
    of all active and cached objects. Provides one function for iCL
    developers: icl_system_register(), which registers a class for
    purging and destroying.

    Also responsible for system-wide initialisation, comprising
    icl_global_pool and icl_global_mutex.  Note that to avoid circular
    dependencies, these two are declared in icl_apr.
    Generated from icl_system.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_ICL_SYSTEM_SAFE
#   define INCLUDE_ICL_SYSTEM_SAFE
#   define INCLUDE_ICL_SYSTEM_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_SYSTEM_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_system_t icl_system_t;

#define ICL_SYSTEM_ALIVE                0xFABB
#define ICL_SYSTEM_DEAD                 0xDEAD

#define ICL_SYSTEM_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_system\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_SYSTEM_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_system at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_SYSTEM_ALIVE, self->object_tag);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
//  Types

typedef void (icl_system_fn) (void);    //  Call-back function

//  Global variables

extern int
    g_argc;
extern char
    **g_argv;
#   ifdef __cplusplus
}
#   endif

#   include "icl_destroy_list.h"
#   include "icl_system_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SYSTEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_SYSTEM_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_SYSTEM_UNSAFE
#   define INCLUDE_ICL_SYSTEM_UNSAFE
#   define INCLUDE_ICL_SYSTEM_ACTIVE

#   include "icl_destroy_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_system_animating;

//  Structure of the icl_system object

struct _icl_system_t {
dbyte
    object_tag;                     //  Object validity marker
volatile icl_system_list_t *list_head;
volatile icl_system_t *list_prev;
volatile icl_system_t *list_next;
icl_system_fn *purge;               //  Purge cache call-back
icl_system_fn *destroy;             //  Destroy cache call-back
};

#   ifdef __cplusplus
}
#   endif
#   include "icl_system_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    icl_system_initialise (
int argc,                               //  The number of program arguments
char ** argv                            //  The program arguments
);

#define icl_system_new()                icl_system_new_ (__FILE__, __LINE__)
icl_system_t *
    icl_system_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_system_register(purge,destroy)  icl_system_register_ (__FILE__, __LINE__, purge, destroy)
void
    icl_system_register_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
icl_system_fn * purge,                  //  Address of cache purge function in calling class
icl_system_fn * destroy                 //  Address of cache destroy function in calling class
);

void
    icl_system_purge (
void);

void
    icl_system_terminate (
void);

void
    icl_system_panic (
char * caller,                          //  Name of caller
char * format,                          //  Format specifier
...                                     //  Arguments to print
);

void
    icl_system_panic_v (
char * caller,                          //  Name of caller
char * format,                          //  Format specifier
va_list args                            //  Arguments to print
);

char *
    icl_system_name (
char * name                             //  Name of platform
);

void
    icl_system_selftest (
void);

void
    icl_system_remove_from_all_containers (
icl_system_t * self                     //  The item
);

#define icl_system_destroy(self)        icl_system_destroy_ (self, __FILE__, __LINE__)
void
    icl_system_destroy_ (
icl_system_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    icl_system_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define icl_system_new_in_scope(self_p,_scope)  icl_system_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    icl_system_new_in_scope_ (
icl_system_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SYSTEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_SYSTEM_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_SYSTEM_INLINE)
#   define INCLUDE_ICL_SYSTEM_INLINE
#   define INCLUDE_ICL_SYSTEM_ACTIVE

#   include "icl_destroy_list.h"
#   include "icl_system_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SYSTEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_SYSTEM_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_SYSTEM_ROOT
#   endif
# endif
#endif
