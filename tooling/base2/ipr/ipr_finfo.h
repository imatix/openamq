/*---------------------------------------------------------------------------
    ipr_finfo.h - ipr_finfo component

    This class wraps the APR finfo object so that we can use it in iCL
    lists, etc.  Generates a list container - ipr_finfo_list.
    Generated from ipr_finfo.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_FINFO_SAFE
#   define INCLUDE_IPR_FINFO_SAFE
#   define INCLUDE_IPR_FINFO_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_FINFO_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_finfo_t ipr_finfo_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
#    define IPR_FINFO_HISTORY_LENGTH 32
#endif

#define IPR_FINFO_ALIVE                 0xFABB
#define IPR_FINFO_DEAD                  0xDEAD

#define IPR_FINFO_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_finfo\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_FINFO_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_finfo at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_FINFO_ALIVE, self->object_tag);\
        ipr_finfo_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_time.h"
#   include "ipr_finfo_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_FINFO_ACTIVE
#   if defined (ICL_IMPORT_IPR_FINFO_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_FINFO_UNSAFE
#   define INCLUDE_IPR_FINFO_UNSAFE
#   define INCLUDE_IPR_FINFO_ACTIVE

#   include "ipr_time.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_finfo_animating;

//  Structure of the ipr_finfo object

struct _ipr_finfo_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_FINFO_HISTORY_LENGTH];
    int
        history_line  [IPR_FINFO_HISTORY_LENGTH];
    char
        *history_type [IPR_FINFO_HISTORY_LENGTH];
    int
        history_links [IPR_FINFO_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
volatile ipr_finfo_list_t *list_head;
volatile ipr_finfo_t *list_prev;
volatile ipr_finfo_t *list_next;
icl_shortstr_t
    name;                           //  File name, without path
apr_fileperms_t
    protection;                     //  Access permissions
apr_filetype_e
    filetype;                       //  Type of file - APR_REG, APR_DIR,...
apr_ino_t
    inode;                          //  inode on file system
apr_dev_t
    device;                         //  ID of device that file is on
apr_int32_t
    nlink;                          //  Number of hard links to file
apr_uid_t
    user;                           //  Owning user id
apr_gid_t
    group;                          //  Owning group id
apr_off_t
    size;                           //  Size of file in bytes
apr_time_t
    atime;                          //  Time file was last accessed
apr_time_t
    mtime;                          //  Time file was last modified
apr_time_t
    ctime;                          //  Time file was created
};

#   ifdef __cplusplus
}
#   endif
#   include "ipr_finfo_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_finfo_new(finfo)            ipr_finfo_new_ (__FILE__, __LINE__, finfo)
ipr_finfo_t *
    ipr_finfo_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
apr_finfo_t * finfo                     //  APR finfo struct
);

int
    ipr_finfo_matches (
ipr_finfo_t * self,                     //  Reference to object
char * pattern                          //  Wildcard pattern
);

int
    ipr_finfo_print (
ipr_finfo_t * self,                     //  Reference to object
char * buffer                           //  Buffer to prepare
);

void
    ipr_finfo_selftest (
void);

void
    ipr_finfo_remove_from_all_containers (
ipr_finfo_t * self                      //  The item
);

#define ipr_finfo_show(item,opcode,trace_file)  ipr_finfo_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_finfo_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_finfo_terminate (
void);

#define ipr_finfo_destroy(self)         ipr_finfo_destroy_ (self, __FILE__, __LINE__)
void
    ipr_finfo_destroy_ (
ipr_finfo_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define ipr_finfo_link(self)            ipr_finfo_link_ (self, __FILE__, __LINE__)
ipr_finfo_t *
    ipr_finfo_link_ (
ipr_finfo_t * self,                     //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_finfo_unlink(self)          ipr_finfo_unlink_ (self, __FILE__, __LINE__)
void
    ipr_finfo_unlink_ (
ipr_finfo_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_finfo_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_finfo_new_in_scope(self_p,_scope,finfo)  ipr_finfo_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, finfo)
void
    ipr_finfo_new_in_scope_ (
ipr_finfo_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
apr_finfo_t * finfo                     //  APR finfo struct
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_FINFO_ACTIVE
#   if defined (ICL_IMPORT_IPR_FINFO_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_FINFO_INLINE)
#   define INCLUDE_IPR_FINFO_INLINE
#   define INCLUDE_IPR_FINFO_ACTIVE

#   include "ipr_time.h"
#   include "ipr_finfo_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_FINFO_ACTIVE
#   if defined (ICL_IMPORT_IPR_FINFO_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_FINFO_ROOT
#   endif
# endif
#endif
