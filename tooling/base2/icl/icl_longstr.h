/*---------------------------------------------------------------------------
    icl_longstr.h - icl_longstr component

    We define the icl_longstr_t type in order to provide a secure way of
    storing and serialising long strings.  A "long string" is defined
    as one that can be serialised as a 32-bit length indicator
    plus 0-4Gb-1 octets of data. The icl_longstr_t type is held in memory
    as descriptor consisting of a size field and a data reference.
    Generated from icl_longstr.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_ICL_LONGSTR_SAFE
#   define INCLUDE_ICL_LONGSTR_SAFE
#   define INCLUDE_ICL_LONGSTR_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_LONGSTR_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_longstr_t icl_longstr_t;
#define IPR_LONGSTR(s)     { sizeof(s), (s) }
#define ICL_LONGSTR_ALIVE               0xFABB
#define ICL_LONGSTR_DEAD                0xDEAD
#define ICL_LONGSTR_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_longstr\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_LONGSTR_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_longstr at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_LONGSTR_ALIVE, self->object_tag);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
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
#   undef INCLUDE_ICL_LONGSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_LONGSTR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_LONGSTR_UNSAFE
#   define INCLUDE_ICL_LONGSTR_UNSAFE
#   define INCLUDE_ICL_LONGSTR_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_longstr_animating;

//  Structure of the icl_longstr object

struct _icl_longstr_t {
qbyte
    object_tag;                     //  Object validity marker

size_t cur_size;                    //  Current size of data part
size_t max_size;                    //  Maximum allocated size
byte  *data;                        //  Data part follows here
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define icl_longstr_new(data,size)      icl_longstr_new_ (__FILE__, __LINE__, data, size)
icl_longstr_t *
    icl_longstr_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
void * data,                            //  Data to copy
size_t size                             //  Length of data
);

#define icl_longstr_destroy(self)       icl_longstr_destroy_ (self, __FILE__, __LINE__)
void
    icl_longstr_destroy_ (
icl_longstr_t * ( * self_p ),           //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_longstr_resize(self,size)   icl_longstr_resize_ (self, size, __FILE__, __LINE__)
int
    icl_longstr_resize_ (
icl_longstr_t * self,                   //  Reference to object
size_t size,                            //  New length of data
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

int
    icl_longstr_eq (
icl_longstr_t * self,                   //  First string
icl_longstr_t * second                  //  String to compare with
);

icl_longstr_t *
    icl_longstr_dup (
icl_longstr_t * self                    //  Source string
);

int
    icl_longstr_fmt (
icl_longstr_t * self,                   //  Reference to object
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

int
    icl_longstr_fmt_v (
icl_longstr_t * self,                   //  Reference to object
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

int
    icl_longstr_cat (
icl_longstr_t * self,                   //  Reference to object
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

int
    icl_longstr_cat_v (
icl_longstr_t * self,                   //  Reference to object
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

byte *
    icl_longstr_find (
icl_longstr_t * self,                   //  Reference to longstr
size_t offset,                          //  Start searching at this offset
byte * pattern,                         //  Pattern to look for
size_t patsize                          //  Size of pattern, in octets
);

void
    icl_longstr_selftest (
void);

void
    icl_longstr_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_LONGSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_LONGSTR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_LONGSTR_INLINE)
#   define INCLUDE_ICL_LONGSTR_INLINE
#   define INCLUDE_ICL_LONGSTR_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_LONGSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_LONGSTR_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_LONGSTR_ROOT
#   endif
# endif
#endif
