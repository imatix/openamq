/*---------------------------------------------------------------------------
    ipr_regexp.h - ipr_regexp component

    Provides an interface to the PCRE library.  This is a snippet
    from the PCRE documentation:

    There are two different sets of meta-characters: those  that
    are  recognized anywhere in the pattern except within square
    brackets, and those that are recognized in square  brackets.
    Outside square brackets, the meta-characters are as follows:

           general escape character with several uses
    ^      assert start of subject (or line, in multiline mode)
    $      assert end of subject (or line, in multiline mode)
    .      match any character except newline (by default)
    [      start character class definition
    |      start of alternative branch
    (      start subpattern
    )      end subpattern
    ?      extends the meaning of (
           also 0 or 1 quantifier
           also quantifier minimizer
    *      0 or more quantifier
    +      1 or more quantifier
    {      start min/max quantifier

    Part of a pattern that is in square brackets is called a
    "character class".  In a character class the only meta-
    characters are:

           general escape character
    ^      negate the class, but only if the first character
    -      indicates character range
    ]      terminates the character class
    Generated from ipr_regexp.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_REGEXP_SAFE
#   define INCLUDE_IPR_REGEXP_SAFE
#   define INCLUDE_IPR_REGEXP_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_REGEXP_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_regexp_t ipr_regexp_t;

#define IPR_REGEXP_ALIVE                0xFABB
#define IPR_REGEXP_DEAD                 0xDEAD

#define IPR_REGEXP_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_regexp\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_REGEXP_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_regexp at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_REGEXP_ALIVE, self->object_tag);\
        ipr_regexp_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#include "pcre.h"
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
#   undef INCLUDE_IPR_REGEXP_ACTIVE
#   if defined (ICL_IMPORT_IPR_REGEXP_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_REGEXP_UNSAFE
#   define INCLUDE_IPR_REGEXP_UNSAFE
#   define INCLUDE_IPR_REGEXP_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_regexp_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_regexp_new(pattern)         ipr_regexp_new_ (__FILE__, __LINE__, pattern)
ipr_regexp_t *
    ipr_regexp_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * pattern                          //  Not documented
);

int
    ipr_regexp_compile (
ipr_regexp_t * self,                    //  Reference to object
char * pattern,                         //  Not documented
Bool csafe                              //  Not documented
);

int
    ipr_regexp_match (
ipr_regexp_t * self,                    //  Reference to object
char * string,                          //  Not documented
...                                     //  Not documented
);

int
    ipr_regexp_match_v (
ipr_regexp_t * self,                    //  Reference to object
char * string,                          //  Not documented
va_list argptr                          //  Not documented
);

int
    ipr_regexp_eq (
char * regexp,                          //  Regular expression
char * string                           //  String to compare
);

void
    ipr_regexp_selftest (
void);

void
    ipr_regexp_terminate (
void);

#define ipr_regexp_show(item,opcode,trace_file)  ipr_regexp_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_regexp_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_regexp_destroy(self)        ipr_regexp_destroy_ (self, __FILE__, __LINE__)
void
    ipr_regexp_destroy_ (
ipr_regexp_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_regexp_read_lock (
ipr_regexp_t * self                     //  Reference to object
);

int
    ipr_regexp_write_lock (
ipr_regexp_t * self                     //  Reference to object
);

int
    ipr_regexp_unlock (
ipr_regexp_t * self                     //  Reference to object
);

void
    ipr_regexp_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_regexp_new_in_scope(self_p,_scope,pattern)  ipr_regexp_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, pattern)
void
    ipr_regexp_new_in_scope_ (
ipr_regexp_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * pattern                          //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_REGEXP_ACTIVE
#   if defined (ICL_IMPORT_IPR_REGEXP_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_REGEXP_INLINE)
#   define INCLUDE_IPR_REGEXP_INLINE
#   define INCLUDE_IPR_REGEXP_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_REGEXP_ACTIVE
#   if defined (ICL_IMPORT_IPR_REGEXP_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_REGEXP_ROOT
#   endif
# endif
#endif
