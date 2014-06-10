/*---------------------------------------------------------------------------
    ipr_bits.h - ipr_bits component

Implements a bitstring object.
    Generated from ipr_bits.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_BITS_SAFE
#   define INCLUDE_IPR_BITS_SAFE
#   define INCLUDE_IPR_BITS_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_BITS_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_bits_t ipr_bits_t;

#define IPR_BITS_ALIVE                  0xFABB
#define IPR_BITS_DEAD                   0xDEAD

#define IPR_BITS_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_bits\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_BITS_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_bits at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_BITS_ALIVE, self->object_tag);\
        ipr_bits_show (self, ICL_CALLBACK_DUMP, stderr);\
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
#   undef INCLUDE_IPR_BITS_ACTIVE
#   if defined (ICL_IMPORT_IPR_BITS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_BITS_UNSAFE
#   define INCLUDE_IPR_BITS_UNSAFE
#   define INCLUDE_IPR_BITS_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_bits_animating;
//  For now the size of a bitstring is fixed; must be multiple of 8 bytes
#define IPR_BITS_SIZE       16384       //  Size in bytes
#define IPR_BITS_SIZE_BITS  IPR_BITS_SIZE * 8
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_bits_new()                  ipr_bits_new_ (__FILE__, __LINE__)
ipr_bits_t *
    ipr_bits_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

int
    ipr_bits_set (
ipr_bits_t * self,                      //  Reference to object
uint index                              //  Bit to set, starting at 0
);

int
    ipr_bits_clear (
ipr_bits_t * self,                      //  Reference to object
uint index                              //  Bit to set, starting at 0
);

int
    ipr_bits_test (
ipr_bits_t * self,                      //  Reference to object
uint index                              //  Not documented
);

int
    ipr_bits_reset (
ipr_bits_t * self                       //  Reference to object
);

int
    ipr_bits_ones (
ipr_bits_t * self                       //  Reference to object
);

int
    ipr_bits_and (
ipr_bits_t * self,                      //  Reference to object
ipr_bits_t * source                     //  Not documented
);

int
    ipr_bits_or (
ipr_bits_t * self,                      //  Reference to object
ipr_bits_t * source                     //  Not documented
);

int
    ipr_bits_xor (
ipr_bits_t * self,                      //  Reference to object
ipr_bits_t * source                     //  Not documented
);

int
    ipr_bits_not (
ipr_bits_t * self                       //  Reference to object
);

int
    ipr_bits_first (
ipr_bits_t * self                       //  Reference to object
);

int
    ipr_bits_last (
ipr_bits_t * self                       //  Reference to object
);

int
    ipr_bits_next (
ipr_bits_t * self,                      //  Reference to object
uint index                              //  Bit number to start from
);

int
    ipr_bits_prev (
ipr_bits_t * self,                      //  Reference to object
uint index                              //  Bit number to start from
);

int
    ipr_bits_next_zero (
ipr_bits_t * self,                      //  Reference to object
uint index                              //  Bit number to start from
);

int
    ipr_bits_prev_zero (
ipr_bits_t * self,                      //  Reference to object
uint index                              //  Bit number to start from
);

int
    ipr_bits_fput (
ipr_bits_t * self,                      //  Reference to object
FILE * file                             //  Not documented
);

int
    ipr_bits_fget (
ipr_bits_t * self,                      //  Reference to object
FILE * file                             //  Not documented
);

void
    ipr_bits_selftest (
void);

qbyte
    ipr_bits_count (
void);

void
    ipr_bits_terminate (
void);

#define ipr_bits_show(item,opcode,trace_file)  ipr_bits_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_bits_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_bits_destroy(self)          ipr_bits_destroy_ (self, __FILE__, __LINE__)
void
    ipr_bits_destroy_ (
ipr_bits_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_bits_read_lock (
ipr_bits_t * self                       //  Reference to object
);

int
    ipr_bits_write_lock (
ipr_bits_t * self                       //  Reference to object
);

int
    ipr_bits_unlock (
ipr_bits_t * self                       //  Reference to object
);

void
    ipr_bits_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_bits_new_in_scope(self_p,_scope)  ipr_bits_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_bits_new_in_scope_ (
ipr_bits_t * * self_p,                  //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_BITS_ACTIVE
#   if defined (ICL_IMPORT_IPR_BITS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_BITS_INLINE)
#   define INCLUDE_IPR_BITS_INLINE
#   define INCLUDE_IPR_BITS_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_BITS_ACTIVE
#   if defined (ICL_IMPORT_IPR_BITS_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_BITS_ROOT
#   endif
# endif
#endif
