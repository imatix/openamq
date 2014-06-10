/*---------------------------------------------------------------------------
    ipr_dict_table.h - ipr_dict_table component

    This class implements the hash table container for ipr_dict.  Note
    that this class is not thread safe and uses no rwlocks.  The table
    allows linking for reference counting.
    Generated from ipr_dict_table.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_DICT_TABLE_SAFE
#   define INCLUDE_IPR_DICT_TABLE_SAFE
#   define INCLUDE_IPR_DICT_TABLE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_DICT_TABLE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_dict_table_t ipr_dict_table_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DICT_TABLE)
#    define IPR_DICT_TABLE_HISTORY_LENGTH 32
#endif

#define IPR_DICT_TABLE_ALIVE            0xFABB
#define IPR_DICT_TABLE_DEAD             0xDEAD

#define IPR_DICT_TABLE_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_dict_table\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_DICT_TABLE_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_dict_table at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_DICT_TABLE_ALIVE, self->object_tag);\
        ipr_dict_table_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_dict.h"
#   include "ipr_bucket.h"
#   include "ipr_str.h"
#   include "ipr_http.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DICT_TABLE_ACTIVE
#   if defined (ICL_IMPORT_IPR_DICT_TABLE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_DICT_TABLE_UNSAFE
#   define INCLUDE_IPR_DICT_TABLE_UNSAFE
#   define INCLUDE_IPR_DICT_TABLE_ACTIVE

#   include "ipr_dict.h"
#   include "ipr_bucket.h"
#   include "ipr_str.h"
#   include "ipr_http.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_dict_table_animating;
#define IPR_DICT_TABLE_INITIAL_SIZE     65535
#define IPR_DICT_TABLE_LOAD_FACTOR      75
#define IPR_DICT_TABLE_GROWTH_FACTOR 200

typedef void (ipr_dict_table_callback_fn) (ipr_dict_t *item, void *argument);

//  Structure of the ipr_dict_table object

struct _ipr_dict_table_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_DICT_TABLE)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_DICT_TABLE_HISTORY_LENGTH];
    int
        history_line  [IPR_DICT_TABLE_HISTORY_LENGTH];
    char
        *history_type [IPR_DICT_TABLE_HISTORY_LENGTH];
    int
        history_links [IPR_DICT_TABLE_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
size_t
    nbr_items;
size_t
    max_items;
ipr_dict_t
    **table_items;
ipr_dict_list_t
    *list;                          //  List of all dictionary items
char
    *file_name;                     //  If data was loaded from file
int64_t
    file_size;                      //  Size of loaded file
apr_time_t
    file_time;                      //  Time of loaded file
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_dict_table_new()            ipr_dict_table_new_ (__FILE__, __LINE__)
ipr_dict_table_t *
    ipr_dict_table_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

int
    ipr_dict_table_insert (
ipr_dict_table_t * self,                //  Reference to object
char * key,                             //  Hash key
ipr_dict_t * item                       //  Item to insert
);

int
    ipr_dict_table_remove (
ipr_dict_t * item                       //  Item to remove
);

char *
    ipr_dict_table_lookup (
ipr_dict_table_t * self,                //  Reference to self
char * name                             //  Field to look for
);

int
    ipr_dict_table_matches (
ipr_dict_table_t * self,                //  Reference to object
char * name,                            //  Name of item to look for
char * match                            //  Regular expression to check
);

char *
    ipr_dict_table_template (
ipr_dict_table_t * self,                //  Symbol table
char * string                           //  Original string
);

char **
    ipr_dict_table_export (
ipr_dict_table_t * self,                //  Reference to self
Bool env,                               //  Format as environment
char * prefix                           //  Optional prefix for exported names
);

int
    ipr_dict_table_import (
ipr_dict_table_t * self,                //  Reference to object
char ** strings,                        //  Array of strings to import
Bool lower                              //  Format names into lower case
);

int
    ipr_dict_table_headers_load (
ipr_dict_table_t * self,                //  Reference to object
char * headers,                         //  Headers block to import
Bool trace                              //  Trace headers?
);

ipr_bucket_t *
    ipr_dict_table_headers_save (
ipr_dict_table_t * self,                //  Reference to self
icl_longstr_t * prefix,                 //  Bucket prefix
Bool trace                              //  Trace headers?
);

char *
    ipr_dict_table_headers_search (
ipr_dict_table_t * self,                //  Reference to self
char * name                             //  Field to look for
);

int
    ipr_dict_table_uri_load (
ipr_dict_table_t * self,                //  Reference to object
char * arguments                        //  Arguments to import
);

int
    ipr_dict_table_props_load (
ipr_dict_table_t * self,                //  Reference to object
char * string                           //  Properties to import
);

int
    ipr_dict_table_file_load (
ipr_dict_table_t * self,                //  Reference to object
char * filename,                        //  Name of file to import
char * pattern                          //  Regexp to match each line
);

int
    ipr_dict_table_file_save (
ipr_dict_table_t * self,                //  Reference to object
char * filename,                        //  Name of file to import
char * pattern                          //  Pattern for saved lines
);

int
    ipr_dict_table_file_sync (
ipr_dict_table_t ** self_p,             //  Pointer to table reference
char * filename,                        //  Name of file to import
char * pattern                          //  Regexp to match each line
);

void
    ipr_dict_table_selftest (
void);

ipr_dict_t *
    ipr_dict_table_search (
ipr_dict_table_t * self,                //  Table to search
char * key                              //  Hash key
);

void
    ipr_dict_table_apply (
ipr_dict_table_t * self,                //  Table to iterate
ipr_dict_table_callback_fn * callback,   //  Not documented
void * argument                         //  Arbitrary argument pointer
);

void
    ipr_dict_table_terminate (
void);

#define ipr_dict_table_show(item,opcode,trace_file)  ipr_dict_table_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_dict_table_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_dict_table_destroy(self)    ipr_dict_table_destroy_ (self, __FILE__, __LINE__)
void
    ipr_dict_table_destroy_ (
ipr_dict_table_t * ( * self_p ),        //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define ipr_dict_table_link(self)       ipr_dict_table_link_ (self, __FILE__, __LINE__)
ipr_dict_table_t *
    ipr_dict_table_link_ (
ipr_dict_table_t * self,                //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_dict_table_unlink(self)     ipr_dict_table_unlink_ (self, __FILE__, __LINE__)
void
    ipr_dict_table_unlink_ (
ipr_dict_table_t * ( * self_p ),        //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_dict_table_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_dict_table_new_in_scope(self_p,_scope)  ipr_dict_table_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_dict_table_new_in_scope_ (
ipr_dict_table_t * * self_p,            //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DICT_TABLE_ACTIVE
#   if defined (ICL_IMPORT_IPR_DICT_TABLE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_DICT_TABLE_INLINE)
#   define INCLUDE_IPR_DICT_TABLE_INLINE
#   define INCLUDE_IPR_DICT_TABLE_ACTIVE

#   include "ipr_dict.h"
#   include "ipr_bucket.h"
#   include "ipr_str.h"
#   include "ipr_http.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DICT_TABLE_ACTIVE
#   if defined (ICL_IMPORT_IPR_DICT_TABLE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_DICT_TABLE_ROOT
#   endif
# endif
#endif
