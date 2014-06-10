/*---------------------------------------------------------------------------
    ipr_dir.h - ipr_dir component

    Provides a directory listing object that can be resorted in various
    ways.
    Generated from ipr_dir.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_DIR_SAFE
#   define INCLUDE_IPR_DIR_SAFE
#   define INCLUDE_IPR_DIR_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_DIR_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_dir_t ipr_dir_t;

#define IPR_DIR_ALIVE                   0xFABB
#define IPR_DIR_DEAD                    0xDEAD

#define IPR_DIR_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_dir\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_DIR_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_dir at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_DIR_ALIVE, self->object_tag);\
        ipr_dir_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_finfo.h"
#   include "ipr_str.h"
#   include "icl_shortstr.h"
#   include "ipr_file.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DIR_ACTIVE
#   if defined (ICL_IMPORT_IPR_DIR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_DIR_UNSAFE
#   define INCLUDE_IPR_DIR_UNSAFE
#   define INCLUDE_IPR_DIR_ACTIVE

#   include "ipr_finfo.h"
#   include "ipr_str.h"
#   include "icl_shortstr.h"
#   include "ipr_file.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_dir_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_dir_new(path,sortby)        ipr_dir_new_ (__FILE__, __LINE__, path, sortby)
ipr_dir_t *
    ipr_dir_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * path,                            //  Directory path to load
char sortby                             //  Sort key
);

int
    ipr_dir_resort (
ipr_dir_t * self,                       //  Reference to object
char sortby                             //  Sort key
);

int
    ipr_dir_dump (
ipr_dir_t * self                        //  Reference to object
);

Bool
    ipr_dir_create (
char * path_to_create                   //  Directory path to create
);

Bool
    ipr_dir_remove (
char * path                             //  Directory path to remove
);

void
    ipr_dir_selftest (
void);

uint
    ipr_dir_index_size (
ipr_dir_t * self                        //  Not documented
);

ipr_finfo_t **
    ipr_dir_index (
ipr_dir_t * self                        //  Not documented
);

void
    ipr_dir_terminate (
void);

#define ipr_dir_show(item,opcode,trace_file)  ipr_dir_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_dir_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_dir_destroy(self)           ipr_dir_destroy_ (self, __FILE__, __LINE__)
void
    ipr_dir_destroy_ (
ipr_dir_t * ( * self_p ),               //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    ipr_dir_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_dir_new_in_scope(self_p,_scope,path,sortby)  ipr_dir_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, path, sortby)
void
    ipr_dir_new_in_scope_ (
ipr_dir_t * * self_p,                   //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * path,                            //  Directory path to load
char sortby                             //  Sort key
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DIR_ACTIVE
#   if defined (ICL_IMPORT_IPR_DIR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_DIR_INLINE)
#   define INCLUDE_IPR_DIR_INLINE
#   define INCLUDE_IPR_DIR_ACTIVE

#   include "ipr_finfo.h"
#   include "ipr_str.h"
#   include "icl_shortstr.h"
#   include "ipr_file.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DIR_ACTIVE
#   if defined (ICL_IMPORT_IPR_DIR_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_DIR_ROOT
#   endif
# endif
#endif
