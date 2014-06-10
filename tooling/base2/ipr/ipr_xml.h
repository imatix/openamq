/*---------------------------------------------------------------------------
    ipr_xml.h - ipr_xml component

    Implements XML objects.
    Generated from ipr_xml.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_XML_SAFE
#   define INCLUDE_IPR_XML_SAFE
#   define INCLUDE_IPR_XML_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_XML_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_xml_t ipr_xml_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
#    define IPR_XML_HISTORY_LENGTH 32
#endif

#define IPR_XML_ALIVE                   0xFABB
#define IPR_XML_DEAD                    0xDEAD

#define IPR_XML_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_xml\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_XML_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_xml at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_XML_ALIVE, self->object_tag);\
        ipr_xml_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "icl_mem.h"
#   include "ipr_bucket.h"
#   include "ipr_xml_attr.h"
#   include "ipr_xml_list.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_XML_ACTIVE
#   if defined (ICL_IMPORT_IPR_XML_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_XML_UNSAFE
#   define INCLUDE_IPR_XML_UNSAFE
#   define INCLUDE_IPR_XML_ACTIVE

#   include "icl_mem.h"
#   include "ipr_bucket.h"
#   include "ipr_xml_attr.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_xml_animating;
#define IPR_FOR_XML_CHILDREN(child,item)             for (child  = ipr_xml_first_child (item);    child != NULL;    child  = ipr_xml_next_sibling (&child))    if (ipr_xml_name (child))

#define IPR_FOR_XML_VALUES(child,item)               for (child  = ipr_xml_first_child (item);    child != NULL;    child  = ipr_xml_next_sibling (&child))    if (!ipr_xml_name (child))

#define IPR_FOR_XML_ATTRIBUTES(attr,item)            for (attr  = ipr_xml_attr_first (item);    attr != NULL;    attr  = ipr_xml_attr_next (&attr))

//  Structure of the ipr_xml object

struct _ipr_xml_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_XML_HISTORY_LENGTH];
    int
        history_line  [IPR_XML_HISTORY_LENGTH];
    char
        *history_type [IPR_XML_HISTORY_LENGTH];
    int
        history_links [IPR_XML_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
volatile ipr_xml_list_t *list_head;
volatile ipr_xml_t *list_prev;
volatile ipr_xml_t *list_next;
ipr_xml_t
    *parent;
char
    *name,                          //  Item name, allocated string
    *value;                         //  Value node, allocated string
ipr_xml_list_t
    *children;
ipr_xml_attr_list_t
    *attrs;
};

#   ifdef __cplusplus
}
#   endif
#   include "ipr_xml_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_xml_new(parent,name,value)  ipr_xml_new_ (__FILE__, __LINE__, parent, name, value)
ipr_xml_t *
    ipr_xml_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_xml_t * parent,                     //  The parent of the new item
char * name,                            //  The name of the new item
char * value                            //  The value of the new item
);

ipr_xml_t *
    ipr_xml_parse_file (
char * path,                            //  Not documented
char * filename                         //  Not documented
);

ipr_xml_t *
    ipr_xml_parse_string (
char * string                           //  Not documented
);

ipr_xml_t *
    ipr_xml_parse_bucket (
ipr_bucket_t * bucket                   //  Not documented
);

void
    ipr_xml_attach_child (
ipr_xml_t * parent,                     //  The new parent of the item
ipr_xml_t * self                        //  Not documented
);

void
    ipr_xml_attach_sibling (
ipr_xml_t * sibling,                    //  Item inserted before the sibling
ipr_xml_t * self                        //  The item
);

void
    ipr_xml_detach (
ipr_xml_t * self                        //  The item
);

ipr_xml_t *
    ipr_xml_first_child (
ipr_xml_t * self                        //  Not documented
);

ipr_xml_t *
    ipr_xml_next_sibling (
ipr_xml_t ** pself                      //  Not documented
);

ipr_xml_t *
    ipr_xml_parent (
ipr_xml_t ** pself                      //  Not documented
);

char *
    ipr_xml_name (
ipr_xml_t * self                        //  Not documented
);

char *
    ipr_xml_value (
ipr_xml_t * self                        //  Not documented
);

void
    ipr_xml_rename (
ipr_xml_t * self,                       //  Not documented
char * name                             //  Not documented
);

void
    ipr_xml_value_set (
ipr_xml_t * self,                       //  Not documented
char * value                            //  Not documented
);

char *
    ipr_xml_text (
ipr_xml_t * self                        //  Not documented
);

ipr_xml_t *
    ipr_xml_find_item (
ipr_xml_t * xml_root,                   //  Not documented
char * p_path                           //  Not documented
);

ipr_xml_t *
    ipr_xml_next_item (
ipr_xml_t ** pself                      //  Not documented
);

ipr_xml_attr_t*
    ipr_xml_attr (
ipr_xml_t* item,                        //  Not documented
const char* name                        //  Not documented
);

void
    ipr_xml_selftest (
void);

void
    ipr_xml_remove_from_all_containers (
ipr_xml_t * self                        //  The item
);

#define ipr_xml_show(item,opcode,trace_file)  ipr_xml_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_xml_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_xml_terminate (
void);

#define ipr_xml_destroy(self)           ipr_xml_destroy_ (self, __FILE__, __LINE__)
void
    ipr_xml_destroy_ (
ipr_xml_t * ( * self_p ),               //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_xml_read_lock (
ipr_xml_t * self                        //  Reference to object
);

int
    ipr_xml_write_lock (
ipr_xml_t * self                        //  Reference to object
);

int
    ipr_xml_unlock (
ipr_xml_t * self                        //  Reference to object
);

#define ipr_xml_link(self)              ipr_xml_link_ (self, __FILE__, __LINE__)
ipr_xml_t *
    ipr_xml_link_ (
ipr_xml_t * self,                       //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_xml_unlink(self)            ipr_xml_unlink_ (self, __FILE__, __LINE__)
void
    ipr_xml_unlink_ (
ipr_xml_t * ( * self_p ),               //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_xml_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_xml_new_in_scope(self_p,_scope,parent,name,value)  ipr_xml_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, parent, name, value)
void
    ipr_xml_new_in_scope_ (
ipr_xml_t * * self_p,                   //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_xml_t * parent,                     //  The parent of the new item
char * name,                            //  The name of the new item
char * value                            //  The value of the new item
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_XML_ACTIVE
#   if defined (ICL_IMPORT_IPR_XML_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_XML_INLINE)
#   define INCLUDE_IPR_XML_INLINE
#   define INCLUDE_IPR_XML_ACTIVE

#   include "icl_mem.h"
#   include "ipr_bucket.h"
#   include "ipr_xml_attr.h"
#   include "ipr_xml_list.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_XML_ACTIVE
#   if defined (ICL_IMPORT_IPR_XML_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_XML_ROOT
#   endif
# endif
#endif
