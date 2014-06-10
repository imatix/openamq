/*---------------------------------------------------------------------------
    ipr_config.h - ipr_config component

    Generated from ipr_config.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_CONFIG_SAFE
#   define INCLUDE_IPR_CONFIG_SAFE
#   define INCLUDE_IPR_CONFIG_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_CONFIG_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_config_t ipr_config_t;

#define IPR_CONFIG_ALIVE                0xFABB
#define IPR_CONFIG_DEAD                 0xDEAD

#define IPR_CONFIG_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_config\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_CONFIG_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_config at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_CONFIG_ALIVE, self->object_tag);\
        ipr_config_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_xml.h"
#   include "ipr_file.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_CONFIG_ACTIVE
#   if defined (ICL_IMPORT_IPR_CONFIG_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_CONFIG_UNSAFE
#   define INCLUDE_IPR_CONFIG_UNSAFE
#   define INCLUDE_IPR_CONFIG_ACTIVE

#   include "ipr_xml.h"
#   include "ipr_file.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_config_animating;

//  Structure of the ipr_config object

struct _ipr_config_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    dbyte
        object_tag;                     //  Object validity marker
ipr_xml_t
    *xml_root;                      //  Current configuration tree
ipr_xml_t
    *xml_item;                      //  Current XML item in tree
Bool
    duplicated,                     //  Duplicated tree?
    located;                        //  Did last locate work?
char
    *cur_value;                     //  Last-accessed XML item value
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_config_new(filename,required)  ipr_config_new_ (__FILE__, __LINE__, filename, required)
ipr_config_t *
    ipr_config_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * filename,                        //  Name of configuration file
Bool required                           //  Complain if file not found
);

ipr_config_t *
    ipr_config_dup (
ipr_config_t * self                     //  Reference to object
);

int
    ipr_config_load (
ipr_config_t * self,                    //  Reference to object
char * filename,                        //  Name of configuration file
Bool required                           //  Complain if file not found
);

int
    ipr_config_load_bucket (
ipr_config_t * self,                    //  Reference to object
ipr_bucket_t * bucket                   //  Bucket of XML data
);

int
    ipr_config_save (
ipr_config_t * self,                    //  Reference to object
char * filename                         //  Name of configuration file
);

int
    ipr_config_locate (
ipr_config_t * self,                    //  Reference to object
char * path,                            //  Path to item
char * name                             //  Name selector
);

int
    ipr_config_next (
ipr_config_t * self                     //  Reference to object
);

char *
    ipr_config_item (
ipr_config_t * self                     //  Reference to object
);

char *
    ipr_config_value (
ipr_config_t * self                     //  Reference to object
);

char *
    ipr_config_get (
ipr_config_t * self,                    //  Reference to object
char * name,                            //  Attribute name
char * dflt                             //  Default value
);

long
    ipr_config_getn (
ipr_config_t * self,                    //  Reference to object
char * name                             //  Attribute name
);

char *
    ipr_config_getp (
ipr_config_t * self,                    //  Reference to object
char * path,                            //  Path to item
char * name,                            //  Attribute name
char * dflt                             //  Default value
);

char *
    ipr_config_put (
ipr_config_t * self,                    //  Reference to object
char * name,                            //  Attribute name
char * value                            //  Attribute value
);

char *
    ipr_config_putp (
ipr_config_t * self,                    //  Reference to object
char * path,                            //  Path to item
char * name,                            //  Attribute name
char * value                            //  Attribute value
);

void
    ipr_config_selftest (
void);

void
    ipr_config_terminate (
void);

#define ipr_config_show(item,opcode,trace_file)  ipr_config_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_config_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_config_destroy(self)        ipr_config_destroy_ (self, __FILE__, __LINE__)
void
    ipr_config_destroy_ (
ipr_config_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_config_read_lock (
ipr_config_t * self                     //  Reference to object
);

int
    ipr_config_write_lock (
ipr_config_t * self                     //  Reference to object
);

int
    ipr_config_unlock (
ipr_config_t * self                     //  Reference to object
);

void
    ipr_config_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_config_new_in_scope(self_p,_scope,filename,required)  ipr_config_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, filename, required)
void
    ipr_config_new_in_scope_ (
ipr_config_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * filename,                        //  Name of configuration file
Bool required                           //  Complain if file not found
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_CONFIG_ACTIVE
#   if defined (ICL_IMPORT_IPR_CONFIG_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_CONFIG_INLINE)
#   define INCLUDE_IPR_CONFIG_INLINE
#   define INCLUDE_IPR_CONFIG_ACTIVE

#   include "ipr_xml.h"
#   include "ipr_file.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_CONFIG_ACTIVE
#   if defined (ICL_IMPORT_IPR_CONFIG_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_CONFIG_ROOT
#   endif
# endif
#endif
