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
/*---------------------------------------------------------------------------
    ipr_bucket.h - ipr_bucket component

    Defines a bucket, being a buffer of variable size as chosen at creation
    time.  Buckets can be passed in messages and freed by the recipient.
    Buckets are cached so that heavy new/destroy use is very cheap.  We
    use reference counting so that buckets can be freely passed between
    objects.
    Generated from ipr_bucket.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_BUCKET_SAFE
#   define INCLUDE_IPR_BUCKET_SAFE
#   define INCLUDE_IPR_BUCKET_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_BUCKET_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_bucket_t ipr_bucket_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
#    define IPR_BUCKET_HISTORY_LENGTH 32
#endif

#define IPR_BUCKET_ALIVE                0xFABB
#define IPR_BUCKET_DEAD                 0xDEAD

#define IPR_BUCKET_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_bucket\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_BUCKET_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_bucket at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_BUCKET_ALIVE, self->object_tag);\
        ipr_bucket_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
extern volatile void *
    ipr_bucket_total_used;
#   ifdef __cplusplus
}
#   endif

#   include "ipr_mother.h"
#   include "ipr_bucket_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_BUCKET_ACTIVE
#   if defined (ICL_IMPORT_IPR_BUCKET_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_BUCKET_UNSAFE
#   define INCLUDE_IPR_BUCKET_UNSAFE
#   define INCLUDE_IPR_BUCKET_ACTIVE

#   include "ipr_mother.h"
#   include "ipr_bucket_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_bucket_animating;
#include "base_apr.h"

#define IPR_BUCKET_MAX_SIZE  2097152

//  Structure of the ipr_bucket object

struct _ipr_bucket_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_BUCKET)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_BUCKET_HISTORY_LENGTH];
    int
        history_line  [IPR_BUCKET_HISTORY_LENGTH];
    char
        *history_type [IPR_BUCKET_HISTORY_LENGTH];
    int
        history_links [IPR_BUCKET_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
ipr_mother_t
    *mother;
size_t
    cur_size;
size_t
    max_size;
byte
    *data;
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_bucket_new(size)            ipr_bucket_new_ (__FILE__, __LINE__, size)
ipr_bucket_t *
    ipr_bucket_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
size_t size                             //  Required length of data
);

void
    ipr_bucket_cache_purge (
void);

int
    ipr_bucket_empty (
ipr_bucket_t * self                     //  Reference to object
);

int
    ipr_bucket_fill (
ipr_bucket_t * self,                    //  Reference to object
void * data,                            //  Address of data to copy from
size_t size                             //  Amount of data to copy
);

int
    ipr_bucket_fill_null (
ipr_bucket_t * self,                    //  Reference to object
size_t size                             //  Amount of data to copy
);

int
    ipr_bucket_fill_random (
ipr_bucket_t * self,                    //  Reference to object
size_t fill_size                        //  Amount of random data to insert
);

int
    ipr_bucket_fill_repeat (
ipr_bucket_t * self,                    //  Reference to object
byte * data,                            //  Address of data to copy from
size_t size,                            //  Size of source data
size_t fill_size                        //  Amount of data to fill
);

int
    ipr_bucket_cat (
ipr_bucket_t * self,                    //  Reference to object
void * source,                          //  Address of data to copy from
size_t size                             //  Amount of data to copy
);

int
    ipr_bucket_load (
ipr_bucket_t * self,                    //  Reference to object
FILE * file                             //  Open file to read from
);

int
    ipr_bucket_save (
ipr_bucket_t * self,                    //  Reference to object
FILE * file                             //  Open file to write to
);

byte *
    ipr_bucket_find (
ipr_bucket_t * self,                    //  Reference to bucket
size_t offset,                          //  Start searching at this offset
byte * pattern,                         //  Pattern to look for
size_t patsize                          //  Size of pattern, in octets
);

int
    ipr_bucket_dump (
ipr_bucket_t * self,                    //  Reference to object
char * prefix                           //  Prefix for each line
);

static inline void
    ipr_bucket_increment_used (
size_t size                             //  The amount by which to increment the total
);

static inline void
    ipr_bucket_decrement_used (
size_t size                             //  The amount by which to decrement the total
);

size_t
    ipr_bucket_used (
void);

void
    ipr_bucket_selftest (
void);

#define ipr_bucket_show(item,opcode,trace_file)  ipr_bucket_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_bucket_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_bucket_destroy(self)        ipr_bucket_destroy_ (self, __FILE__, __LINE__)
void
    ipr_bucket_destroy_ (
ipr_bucket_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define ipr_bucket_link(self)           ipr_bucket_link_ (self, __FILE__, __LINE__)
ipr_bucket_t *
    ipr_bucket_link_ (
ipr_bucket_t * self,                    //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_bucket_unlink(self)         ipr_bucket_unlink_ (self, __FILE__, __LINE__)
void
    ipr_bucket_unlink_ (
ipr_bucket_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_bucket_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_bucket_new_in_scope(self_p,_scope,size)  ipr_bucket_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, size)
void
    ipr_bucket_new_in_scope_ (
ipr_bucket_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
size_t size                             //  Required length of data
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_BUCKET_ACTIVE
#   if defined (ICL_IMPORT_IPR_BUCKET_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_BUCKET_INLINE)
#   define INCLUDE_IPR_BUCKET_INLINE
#   define INCLUDE_IPR_BUCKET_ACTIVE

#   include "ipr_mother.h"
#   include "ipr_bucket_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    ipr_bucket_increment_used

    Type: Component method
    Increment the record of the total memory allocations.
    
    This is non-trivial because it must be thread-safe, and it must work on
    64 bit architectures.
    -------------------------------------------------------------------------
 */

static inline void
    ipr_bucket_increment_used (
    size_t size                         //  The amount by which to increment the total
)
{
void
    *old_used;

do
    old_used = (void *) ipr_bucket_total_used;
while (icl_atomic_casptr (&ipr_bucket_total_used, (byte *) old_used + size, old_used) != old_used);
}
/*  -------------------------------------------------------------------------
    ipr_bucket_decrement_used

    Type: Component method
    Decrement the record of the total memory allocations.
    
    This is non-trivial because it must be thread-safe, and it must work on
    64 bit architectures.
    -------------------------------------------------------------------------
 */

static inline void
    ipr_bucket_decrement_used (
    size_t size                         //  The amount by which to decrement the total
)
{
void
    *old_used;

do
    old_used = (void *) ipr_bucket_total_used;
while (icl_atomic_casptr (&ipr_bucket_total_used, (byte *) old_used - size, old_used) != old_used);
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_BUCKET_ACTIVE
#   if defined (ICL_IMPORT_IPR_BUCKET_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_BUCKET_ROOT
#   endif
# endif
#endif
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
/*---------------------------------------------------------------------------
    ipr_crc.h - ipr_crc component

    The CRC module calculates a CCITT cyclic-redundancy code for a
    block of data or a series of data.  Use to calculate the check
    sum for a block of data or a series of data items.  Works with
    data and strings.
    Generated from ipr_crc.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_CRC_SAFE
#   define INCLUDE_IPR_CRC_SAFE
#   define INCLUDE_IPR_CRC_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_CRC_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_crc_t ipr_crc_t;

#define IPR_CRC_ALIVE                   0xFABB
#define IPR_CRC_DEAD                    0xDEAD

#define IPR_CRC_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_crc\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_CRC_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_crc at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_CRC_ALIVE, self->object_tag);\
        ipr_crc_show (self, ICL_CALLBACK_DUMP, stderr);\
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
#   undef INCLUDE_IPR_CRC_ACTIVE
#   if defined (ICL_IMPORT_IPR_CRC_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_CRC_UNSAFE
#   define INCLUDE_IPR_CRC_UNSAFE
#   define INCLUDE_IPR_CRC_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_crc_animating;

//  Structure of the ipr_crc object

struct _ipr_crc_t {
dbyte
    object_tag;                     //  Object validity marker
    qbyte value;                        //  
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
int
    ipr_crc_calc (
ipr_crc_t * self,                       //  Reference to object
byte * data,                            //  Data to checksum
size_t size                             //  Length of data
);

int
    ipr_crc_calc_str (
ipr_crc_t * self,                       //  Reference to object
char * string                           //  String to checksum
);

void
    ipr_crc_selftest (
void);

qbyte
    ipr_crc_value (
ipr_crc_t * self                        //  Not documented
);

void
    ipr_crc_terminate (
void);

#define ipr_crc_new()                   ipr_crc_new_ (__FILE__, __LINE__)
ipr_crc_t *
    ipr_crc_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_crc_show(item,opcode,trace_file)  ipr_crc_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_crc_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_crc_destroy(self)           ipr_crc_destroy_ (self, __FILE__, __LINE__)
void
    ipr_crc_destroy_ (
ipr_crc_t * ( * self_p ),               //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    ipr_crc_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_crc_new_in_scope(self_p,_scope)  ipr_crc_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_crc_new_in_scope_ (
ipr_crc_t * * self_p,                   //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_CRC_ACTIVE
#   if defined (ICL_IMPORT_IPR_CRC_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_CRC_INLINE)
#   define INCLUDE_IPR_CRC_INLINE
#   define INCLUDE_IPR_CRC_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_CRC_ACTIVE
#   if defined (ICL_IMPORT_IPR_CRC_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_CRC_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_dict.h - ipr_dict component

Implements a symbol-table dictionary.  The dictionary holds string
values indexed by string keys.  The string values are duplicated.
All access to the dictionary is unlocked, and dictionary items do
not use links, so the dictionary should not be used from multiple
threads in parallel.
    Generated from ipr_dict.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_DICT_SAFE
#   define INCLUDE_IPR_DICT_SAFE
#   define INCLUDE_IPR_DICT_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_DICT_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_dict_t ipr_dict_t;

#define IPR_DICT_ALIVE                  0xFABB
#define IPR_DICT_DEAD                   0xDEAD

#define IPR_DICT_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_dict\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_DICT_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_dict at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_DICT_ALIVE, self->object_tag);\
        ipr_dict_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_regexp.h"
#   include "ipr_dict_table.h"
#   include "ipr_dict_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DICT_ACTIVE
#   if defined (ICL_IMPORT_IPR_DICT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_DICT_UNSAFE
#   define INCLUDE_IPR_DICT_UNSAFE
#   define INCLUDE_IPR_DICT_ACTIVE

#   include "ipr_regexp.h"
#   include "ipr_dict_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_dict_animating;

//  Structure of the ipr_dict object

struct _ipr_dict_t {
dbyte
    object_tag;                     //  Object validity marker
ipr_dict_table_t
    *table_head;                    //  Hash table container
ipr_dict_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
volatile ipr_dict_list_t *list_head;
volatile ipr_dict_t *list_prev;
volatile ipr_dict_t *list_next;
char
    *value;                         //  Text value for item
};

#   ifdef __cplusplus
}
#   endif
#   include "ipr_dict_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_dict_new(table,key,value)   ipr_dict_new_ (__FILE__, __LINE__, table, key, value)
ipr_dict_t *
    ipr_dict_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_dict_table_t * table,               //  Table to insert into
char * key,                             //  Hash key
char * value                            //  Not documented
);

int
    ipr_dict_update (
ipr_dict_t * self,                      //  Reference to object
char * value                            //  Not documented
);

Bool
    ipr_dict_assume (
ipr_dict_table_t * table,               //  Symbol table
char * name,                            //  Symbol name
char * value                            //  Symbol value
);

void
    ipr_dict_assume_fmt (
ipr_dict_table_t * table,               //  Symbol table
char * name,                            //  Symbol name
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

void
    ipr_dict_assume_fmt_v (
ipr_dict_table_t * table,               //  Symbol table
char * name,                            //  Symbol name
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

void
    ipr_dict_selftest (
void);

void
    ipr_dict_remove_from_all_containers (
ipr_dict_t * self                       //  The itemThe item
);

#define ipr_dict_show(item,opcode,trace_file)  ipr_dict_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_dict_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_dict_terminate (
void);

#define ipr_dict_destroy(self)          ipr_dict_destroy_ (self, __FILE__, __LINE__)
void
    ipr_dict_destroy_ (
ipr_dict_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    ipr_dict_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_dict_new_in_scope(self_p,_scope,table,key,value)  ipr_dict_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, key, value)
void
    ipr_dict_new_in_scope_ (
ipr_dict_t * * self_p,                  //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_dict_table_t * table,               //  Table to insert into
char * key,                             //  Hash key
char * value                            //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DICT_ACTIVE
#   if defined (ICL_IMPORT_IPR_DICT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_DICT_INLINE)
#   define INCLUDE_IPR_DICT_INLINE
#   define INCLUDE_IPR_DICT_ACTIVE

#   include "ipr_regexp.h"
#   include "ipr_dict_table.h"
#   include "ipr_dict_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_DICT_ACTIVE
#   if defined (ICL_IMPORT_IPR_DICT_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_DICT_ROOT
#   endif
# endif
#endif
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
/*---------------------------------------------------------------------------
    ipr_file.h - ipr_file component

Provides a general set of file and filename manipulation functions that
return information about a file specified by name. The maximum length of
a filename is set at 255 characters, to fit in an icl_shortstr_t. All
filenames should be defined using this type.
    Generated from ipr_file.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_FILE_SAFE
#   define INCLUDE_IPR_FILE_SAFE
#   define INCLUDE_IPR_FILE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_FILE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif

#   include "icl_shortstr.h"
#   include "ipr_bucket.h"
#   include "ipr_str.h"
#   include "ipr_finfo.h"
#   include "ipr_time.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_FILE_ACTIVE
#   if defined (ICL_IMPORT_IPR_FILE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_FILE_UNSAFE
#   define INCLUDE_IPR_FILE_UNSAFE
#   define INCLUDE_IPR_FILE_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_bucket.h"
#   include "ipr_str.h"
#   include "ipr_finfo.h"
#   include "ipr_time.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_file_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
Bool
    ipr_file_exists (
char * filename                         //  File to examine
);

int64_t
    ipr_file_size (
char * filename                         //  File to measure
);

apr_time_t
    ipr_file_time (
char * filename                         //  File to check
);

size_t
    ipr_file_lines (
char * filename                         //  File to check
);

apr_ino_t
    ipr_file_inode (
char * filename                         //  File to check
);

ipr_bucket_t *
    ipr_file_slurp (
char * filename                         //  File to read
);

int
    ipr_file_where (
char * filename,                        //  File to locate
char * path,                            //  Path to use
char * fullname                         //  Resulting name
);

int
    ipr_file_delete (
char * filename                         //  File to delete
);

int
    ipr_file_rename (
char * newname,                         //  Rename file to this
char * oldname                          //  Actual name of file
);

void
    ipr_file_truncate (
char * filename                         //  File to truncate
);

char *
    ipr_file_strip_path (
char * filename                         //  Filename to process
);

char *
    ipr_file_set_path (
char * filename,                        //  Filename to process
char * path                             //  Desired path
);

char *
    ipr_file_set_extension (
char * filename,                        //  Filename to process
char * extension                        //  Extension to add
);

char *
    ipr_file_move (
char * directory,                       //  Desired directory path
char * filename                         //  Filename to process
);

Bool
    ipr_file_is_readable (
char * filename                         //  File to examine
);

Bool
    ipr_file_is_writeable (
char * filename                         //  File to examine
);

Bool
    ipr_file_is_directory (
char * filename                         //  File to examine
);

ipr_finfo_t *
    ipr_file_get_info (
char * filename                         //  File to examine
);

char *
    ipr_file_digest (
char * filename                         //  File to digest
);

char *
    ipr_file_tmpfile (
void);

char *
    ipr_file_etag (
char * filename,                        //  File to digest
char * type                             //  ETag type: fixed, float
);

void
    ipr_file_selftest (
void);

void
    ipr_file_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_FILE_ACTIVE
#   if defined (ICL_IMPORT_IPR_FILE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_FILE_INLINE)
#   define INCLUDE_IPR_FILE_INLINE
#   define INCLUDE_IPR_FILE_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_bucket.h"
#   include "ipr_str.h"
#   include "ipr_finfo.h"
#   include "ipr_time.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_FILE_ACTIVE
#   if defined (ICL_IMPORT_IPR_FILE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_FILE_ROOT
#   endif
# endif
#endif
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
/*---------------------------------------------------------------------------
    ipr_hash.h - ipr_hash component

Implements a loose-reference hash with no locking, for use in lock-free agents.
The hash is indexed by a string key, and holds a void pointer reference.  Hash
items are reference counted, so use unlink and destroy appropriately.
    Generated from ipr_hash.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_HASH_SAFE
#   define INCLUDE_IPR_HASH_SAFE
#   define INCLUDE_IPR_HASH_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_HASH_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_hash_t ipr_hash_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
#    define IPR_HASH_HISTORY_LENGTH 32
#endif

#define IPR_HASH_ALIVE                  0xFABB
#define IPR_HASH_DEAD                   0xDEAD

#define IPR_HASH_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_hash\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_HASH_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_hash at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_HASH_ALIVE, self->object_tag);\
        ipr_hash_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_hash_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_HASH_ACTIVE
#   if defined (ICL_IMPORT_IPR_HASH_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_HASH_UNSAFE
#   define INCLUDE_IPR_HASH_UNSAFE
#   define INCLUDE_IPR_HASH_ACTIVE

#   include "ipr_hash_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_hash_animating;

//  Structure of the ipr_hash object

struct _ipr_hash_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_HASH)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_HASH_HISTORY_LENGTH];
    int
        history_line  [IPR_HASH_HISTORY_LENGTH];
    char
        *history_type [IPR_HASH_HISTORY_LENGTH];
    int
        history_links [IPR_HASH_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
ipr_hash_table_t
    *table_head;                    //  Hash table container
ipr_hash_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
void *data;
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_hash_new(table,key,data)    ipr_hash_new_ (__FILE__, __LINE__, table, key, data)
ipr_hash_t *
    ipr_hash_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_hash_table_t * table,               //  Table to insert into
char * key,                             //  Hash key
void * data                             //  Not documented
);

void
    ipr_hash_insert (
ipr_hash_table_t * table,               //  Not documented
char * key,                             //  Not documented
void * data                             //  Not documented
);

void *
    ipr_hash_lookup (
ipr_hash_table_t * table,               //  Not documented
char * key                              //  Not documented
);

void
    ipr_hash_delete (
ipr_hash_table_t * table,               //  Not documented
char * key                              //  Not documented
);

void
    ipr_hash_selftest (
void);

void
    ipr_hash_remove_from_all_containers (
ipr_hash_t * self                       //  The item
);

#define ipr_hash_show(item,opcode,trace_file)  ipr_hash_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_hash_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_hash_terminate (
void);

#define ipr_hash_destroy(self)          ipr_hash_destroy_ (self, __FILE__, __LINE__)
void
    ipr_hash_destroy_ (
ipr_hash_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define ipr_hash_link(self)             ipr_hash_link_ (self, __FILE__, __LINE__)
ipr_hash_t *
    ipr_hash_link_ (
ipr_hash_t * self,                      //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_hash_unlink(self)           ipr_hash_unlink_ (self, __FILE__, __LINE__)
void
    ipr_hash_unlink_ (
ipr_hash_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_hash_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_hash_new_in_scope(self_p,_scope,table,key,data)  ipr_hash_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, key, data)
void
    ipr_hash_new_in_scope_ (
ipr_hash_t * * self_p,                  //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_hash_table_t * table,               //  Table to insert into
char * key,                             //  Hash key
void * data                             //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_HASH_ACTIVE
#   if defined (ICL_IMPORT_IPR_HASH_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_HASH_INLINE)
#   define INCLUDE_IPR_HASH_INLINE
#   define INCLUDE_IPR_HASH_ACTIVE

#   include "ipr_hash_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_HASH_ACTIVE
#   if defined (ICL_IMPORT_IPR_HASH_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_HASH_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_http.h - ipr_http component

    The HTTP module provides utility functions for HTTP encodings and
    other basic functionality.
    Generated from ipr_http.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_HTTP_SAFE
#   define INCLUDE_IPR_HTTP_SAFE
#   define INCLUDE_IPR_HTTP_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_HTTP_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif

#   include "ipr_regexp.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_HTTP_ACTIVE
#   if defined (ICL_IMPORT_IPR_HTTP_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_HTTP_UNSAFE
#   define INCLUDE_IPR_HTTP_UNSAFE
#   define INCLUDE_IPR_HTTP_ACTIVE

#   include "ipr_regexp.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_http_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
size_t
    ipr_http_encode_meta (
char * output,                          //  Not documented
char ** input,                          //  Not documented
size_t outmax,                          //  Not documented
Bool html                               //  Not documented
);

size_t
    ipr_http_decode_meta (
char * output,                          //  Not documented
char ** input,                          //  Not documented
size_t outmax                           //  Not documented
);

char
    ipr_http_decode_meta_charn (
char * input,                           //  Not documented
size_t length                           //  Not documented
);

char *
    ipr_http_unescape_uri (
char * uri                              //  URI to unescape
);

char *
    ipr_http_time_str (
char * string                           //  String to receive formatted time
);

void
    ipr_http_selftest (
void);

void
    ipr_http_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_HTTP_ACTIVE
#   if defined (ICL_IMPORT_IPR_HTTP_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_HTTP_INLINE)
#   define INCLUDE_IPR_HTTP_INLINE
#   define INCLUDE_IPR_HTTP_ACTIVE

#   include "ipr_regexp.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_HTTP_ACTIVE
#   if defined (ICL_IMPORT_IPR_HTTP_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_HTTP_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_index.h - ipr_index component

This class provides a mechanism for indexing objects.  An index is a number
from 0 to IPR_INDEX_MAX - 1. The class keeps indexes low by hashing across
a variable part of the index rather than across its full size.  Does not
support the retrieval of objects by their reference, only by index.
    Generated from ipr_index.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_INDEX_SAFE
#   define INCLUDE_IPR_INDEX_SAFE
#   define INCLUDE_IPR_INDEX_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_INDEX_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_index_t ipr_index_t;

#define IPR_INDEX_ALIVE                 0xFABB
#define IPR_INDEX_DEAD                  0xDEAD

#define IPR_INDEX_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_index\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_INDEX_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_index at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_INDEX_ALIVE, self->object_tag);\
        ipr_index_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_bits.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_INDEX_ACTIVE
#   if defined (ICL_IMPORT_IPR_INDEX_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_INDEX_UNSAFE
#   define INCLUDE_IPR_INDEX_UNSAFE
#   define INCLUDE_IPR_INDEX_ACTIVE

#   include "ipr_bits.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_index_animating;
#define IPR_INDEX_MAX   IPR_BITS_SIZE_BITS

//  Structure of the ipr_index object

struct _ipr_index_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    dbyte
        object_tag;                     //  Object validity marker
int   size;                         //  Number of index entries
void *data [IPR_INDEX_MAX];
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
int
    ipr_index_insert (
ipr_index_t * self,                     //  Reference to object
void * object                           //  Object to index
);

int
    ipr_index_delete (
ipr_index_t * self,                     //  Reference to object
int index                               //  Index entry to delete
);

void
    ipr_index_selftest (
void);

#define ipr_index_new()                 ipr_index_new_ (__FILE__, __LINE__)
ipr_index_t *
    ipr_index_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

qbyte
    ipr_index_count (
void);

void
    ipr_index_terminate (
void);

#define ipr_index_show(item,opcode,trace_file)  ipr_index_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_index_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_index_destroy(self)         ipr_index_destroy_ (self, __FILE__, __LINE__)
void
    ipr_index_destroy_ (
ipr_index_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_index_read_lock (
ipr_index_t * self                      //  Reference to object
);

int
    ipr_index_write_lock (
ipr_index_t * self                      //  Reference to object
);

int
    ipr_index_unlock (
ipr_index_t * self                      //  Reference to object
);

void
    ipr_index_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_index_new_in_scope(self_p,_scope)  ipr_index_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_index_new_in_scope_ (
ipr_index_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_INDEX_ACTIVE
#   if defined (ICL_IMPORT_IPR_INDEX_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_INDEX_INLINE)
#   define INCLUDE_IPR_INDEX_INLINE
#   define INCLUDE_IPR_INDEX_ACTIVE

#   include "ipr_bits.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_INDEX_ACTIVE
#   if defined (ICL_IMPORT_IPR_INDEX_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_INDEX_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_log.h - ipr_log component

    Generated from ipr_log.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_LOG_SAFE
#   define INCLUDE_IPR_LOG_SAFE
#   define INCLUDE_IPR_LOG_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_LOG_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Define portable syslog priority constants
//  We use the values defined for syslog when implemented
#define IPR_LOG_SYSERROR    3
#define IPR_LOG_SYSWARNING  4
#define IPR_LOG_SYSINFO     6
#define IPR_LOG_SYSDEBUG    7
#   ifdef __cplusplus
}
#   endif

#   include "icl_shortstr.h"
#   include "icl_longstr.h"
#   include "ipr_file.h"
#   include "ipr_process.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_LOG_ACTIVE
#   if defined (ICL_IMPORT_IPR_LOG_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_LOG_UNSAFE
#   define INCLUDE_IPR_LOG_UNSAFE
#   define INCLUDE_IPR_LOG_ACTIVE

#   include "icl_shortstr.h"
#   include "icl_longstr.h"
#   include "ipr_file.h"
#   include "ipr_process.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_log_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
int
    ipr_log_cycle (
char * filename,                        //  File to examine
char * directory,                       //  Archive directory, or NULL
char * command                          //  Command to run
);

int
    ipr_log_sysopen (
char * identifier                       //  Log identifier
);

void
    ipr_log_sysprint (
int priority,                           //  Log priority
char * string                           //  String to log
);

void
    ipr_log_sysclose (
void);

void
    ipr_log_selftest (
void);

void
    ipr_log_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_LOG_ACTIVE
#   if defined (ICL_IMPORT_IPR_LOG_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_LOG_INLINE)
#   define INCLUDE_IPR_LOG_INLINE
#   define INCLUDE_IPR_LOG_ACTIVE

#   include "icl_shortstr.h"
#   include "icl_longstr.h"
#   include "ipr_file.h"
#   include "ipr_process.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_LOG_ACTIVE
#   if defined (ICL_IMPORT_IPR_LOG_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_LOG_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_looseref.h - ipr_looseref component

    Allows classes to maintain lists of references to other classes without
    creating explicitly-typed classes for each case.  Uses a void * to refer
    to the target class.
    Generated from ipr_looseref.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_LOOSEREF_SAFE
#   define INCLUDE_IPR_LOOSEREF_SAFE
#   define INCLUDE_IPR_LOOSEREF_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_LOOSEREF_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_looseref_t ipr_looseref_t;

#define IPR_LOOSEREF_ALIVE              0xFABB
#define IPR_LOOSEREF_DEAD               0xDEAD

#define IPR_LOOSEREF_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_looseref\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_LOOSEREF_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_looseref at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_LOOSEREF_ALIVE, self->object_tag);\
        ipr_looseref_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_looseref_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_LOOSEREF_ACTIVE
#   if defined (ICL_IMPORT_IPR_LOOSEREF_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_LOOSEREF_UNSAFE
#   define INCLUDE_IPR_LOOSEREF_UNSAFE
#   define INCLUDE_IPR_LOOSEREF_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_looseref_animating;

//  Structure of the ipr_looseref object

struct _ipr_looseref_t {
dbyte
    object_tag;                     //  Object validity marker
volatile ipr_looseref_list_t *list_head;
volatile ipr_looseref_t *list_prev;
volatile ipr_looseref_t *list_next;
void
    *object,                        //  Target of reference
    *argument;                      //  Application-specific argument
};

#   ifdef __cplusplus
}
#   endif
#   include "ipr_looseref_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_looseref_new()              ipr_looseref_new_ (__FILE__, __LINE__)
ipr_looseref_t *
    ipr_looseref_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

ipr_looseref_t *
    ipr_looseref_queue (
ipr_looseref_list_t * list,             //  List to attach to
void * object                           //  Target object reference
);

ipr_looseref_t *
    ipr_looseref_push (
ipr_looseref_list_t * list,             //  List to attach to
void * object                           //  Target object reference
);

void *
    ipr_looseref_pop (
ipr_looseref_list_t * list              //  List to attach to
);

void
    ipr_looseref_selftest (
void);

void
    ipr_looseref_remove_from_all_containers (
ipr_looseref_t * self                   //  The item
);

#define ipr_looseref_show(item,opcode,trace_file)  ipr_looseref_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_looseref_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_looseref_terminate (
void);

#define ipr_looseref_destroy(self)      ipr_looseref_destroy_ (self, __FILE__, __LINE__)
void
    ipr_looseref_destroy_ (
ipr_looseref_t * ( * self_p ),          //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    ipr_looseref_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_looseref_new_in_scope(self_p,_scope)  ipr_looseref_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_looseref_new_in_scope_ (
ipr_looseref_t * * self_p,              //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_LOOSEREF_ACTIVE
#   if defined (ICL_IMPORT_IPR_LOOSEREF_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_LOOSEREF_INLINE)
#   define INCLUDE_IPR_LOOSEREF_INLINE
#   define INCLUDE_IPR_LOOSEREF_ACTIVE

#   include "ipr_looseref_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_LOOSEREF_ACTIVE
#   if defined (ICL_IMPORT_IPR_LOOSEREF_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_LOOSEREF_ROOT
#   endif
# endif
#endif
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
/*---------------------------------------------------------------------------
    ipr_net.h - ipr_net component

    The network module provides various higher-level network access
    functions built on the APR socket layer.
    Generated from ipr_net.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_NET_SAFE
#   define INCLUDE_IPR_NET_SAFE
#   define INCLUDE_IPR_NET_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_NET_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif

#   include "ipr_token.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_NET_ACTIVE
#   if defined (ICL_IMPORT_IPR_NET_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_NET_UNSAFE
#   define INCLUDE_IPR_NET_UNSAFE
#   define INCLUDE_IPR_NET_ACTIVE

#   include "ipr_token.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_net_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    ipr_net_get_addr (
char * dest                             //  Network address
);

void
    ipr_net_get_hostname (
char * dest                             //  Network address
);

Bool
    ipr_net_validate_addr (
char * netaddr                          //  Network address to validate
);

Bool
    ipr_net_ping (
char * hostname,                        //  Host and port to test
char * defport                          //  Default port
);

ipr_token_t *
    ipr_net_ping_list (
ipr_token_list_t * servers,             //  List of servers
char * defport,                         //  Default port
Bool cleanup                            //  Remove failed servers?
);

char *
    ipr_net_lookup_host (
char * hostname,                        //  Default port
char * address                          //  Dotted IP address
);

char *
    ipr_net_fmt_display (
char * external,                        //  Fully-formatted string
char * internal,                        //  Dotted IP address
Bool opt_xlat                           //  Do address lookups?
);

Bool
    ipr_net_cidr_eq (
char * address_str,                     //  Dotted IP address
char * range_str                        //  Dotted CIDR specification
);

void
    ipr_net_selftest (
void);

void
    ipr_net_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_NET_ACTIVE
#   if defined (ICL_IMPORT_IPR_NET_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_NET_INLINE)
#   define INCLUDE_IPR_NET_INLINE
#   define INCLUDE_IPR_NET_ACTIVE

#   include "ipr_token.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_NET_ACTIVE
#   if defined (ICL_IMPORT_IPR_NET_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_NET_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_nonce.h - ipr_nonce component

Nonces are held in a hash table.  The nonce value is the key into the
table.  This class generates the nonce value.
    Generated from ipr_nonce.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_NONCE_SAFE
#   define INCLUDE_IPR_NONCE_SAFE
#   define INCLUDE_IPR_NONCE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_NONCE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_nonce_t ipr_nonce_t;

#define IPR_NONCE_ALIVE                 0xFABB
#define IPR_NONCE_DEAD                  0xDEAD

#define IPR_NONCE_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_nonce\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_NONCE_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_nonce at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_NONCE_ALIVE, self->object_tag);\
        ipr_nonce_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_str.h"
#   include "ipr_nonce_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_NONCE_ACTIVE
#   if defined (ICL_IMPORT_IPR_NONCE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_NONCE_UNSAFE
#   define INCLUDE_IPR_NONCE_UNSAFE
#   define INCLUDE_IPR_NONCE_ACTIVE

#   include "ipr_str.h"
#   include "ipr_nonce_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_nonce_animating;

//  Structure of the ipr_nonce object

struct _ipr_nonce_t {
dbyte
    object_tag;                     //  Object validity marker
ipr_nonce_table_t
    *table_head;                    //  Hash table container
ipr_nonce_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
int64_t
    count;                          //  Client nonce count value
apr_time_t
    expiry;                         //  Calculated expiry time
size_t
    quota;                          //  Hits remaining
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_nonce_new(table,key,ttl,quota)  ipr_nonce_new_ (__FILE__, __LINE__, table, key, ttl, quota)
ipr_nonce_t *
    ipr_nonce_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_nonce_table_t * table,              //  Table to insert into
char * key,                             //  Hash keyNonce value or NULL
size_t ttl,                             //  Time to live, seconds
size_t quota                            //  Lifespan, in hits
);

ipr_nonce_t *
    ipr_nonce_assume (
ipr_nonce_table_t * table,              //  Nonce table
char * key,                             //  Nonce value
size_t ttl,                             //  Time to live, seconds
size_t quota                            //  Lifespan, in hits
);

int
    ipr_nonce_expired (
ipr_nonce_t * self                      //  Reference to object
);

void
    ipr_nonce_selftest (
void);

void
    ipr_nonce_remove_from_all_containers (
ipr_nonce_t * self                      //  The item
);

#define ipr_nonce_show(item,opcode,trace_file)  ipr_nonce_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_nonce_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_nonce_terminate (
void);

#define ipr_nonce_destroy(self)         ipr_nonce_destroy_ (self, __FILE__, __LINE__)
void
    ipr_nonce_destroy_ (
ipr_nonce_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    ipr_nonce_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_nonce_new_in_scope(self_p,_scope,table,key,ttl,quota)  ipr_nonce_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, key, ttl, quota)
void
    ipr_nonce_new_in_scope_ (
ipr_nonce_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_nonce_table_t * table,              //  Table to insert into
char * key,                             //  Hash keyNonce value or NULL
size_t ttl,                             //  Time to live, seconds
size_t quota                            //  Lifespan, in hits
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_NONCE_ACTIVE
#   if defined (ICL_IMPORT_IPR_NONCE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_NONCE_INLINE)
#   define INCLUDE_IPR_NONCE_INLINE
#   define INCLUDE_IPR_NONCE_ACTIVE

#   include "ipr_str.h"
#   include "ipr_nonce_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_NONCE_ACTIVE
#   if defined (ICL_IMPORT_IPR_NONCE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_NONCE_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_path.h - ipr_path component

Provides a general set of path manipulation functions.
    Generated from ipr_path.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_PATH_SAFE
#   define INCLUDE_IPR_PATH_SAFE
#   define INCLUDE_IPR_PATH_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_PATH_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif

#   include "ipr_file.h"
#   include "ipr_str.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PATH_ACTIVE
#   if defined (ICL_IMPORT_IPR_PATH_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_PATH_UNSAFE
#   define INCLUDE_IPR_PATH_UNSAFE
#   define INCLUDE_IPR_PATH_ACTIVE

#   include "ipr_file.h"
#   include "ipr_str.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_path_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
Bool
    ipr_path_create (
char * path_to_create                   //  Path to create
);

Bool
    ipr_path_remove (
char * path                             //  Directory path to remove
);

char *
    ipr_path_resolve (
char * old_path                         //  Directory path to resolve
);

void
    ipr_path_selftest (
void);

void
    ipr_path_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PATH_ACTIVE
#   if defined (ICL_IMPORT_IPR_PATH_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_PATH_INLINE)
#   define INCLUDE_IPR_PATH_INLINE
#   define INCLUDE_IPR_PATH_ACTIVE

#   include "ipr_file.h"
#   include "ipr_str.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PATH_ACTIVE
#   if defined (ICL_IMPORT_IPR_PATH_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_PATH_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_password.h - ipr_password component

    Password tables are held as dictionaries.  Supports passwords in
    either basic or digest format as defined by RFC 2617. ipr_password
    items do not use links.  This class is threadsafe and can be used
    from concurrent threads.
    Generated from ipr_password.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_PASSWORD_SAFE
#   define INCLUDE_IPR_PASSWORD_SAFE
#   define INCLUDE_IPR_PASSWORD_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_PASSWORD_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_password_t ipr_password_t;

#define IPR_PASSWORD_ALIVE              0xFABB
#define IPR_PASSWORD_DEAD               0xDEAD

#define IPR_PASSWORD_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_password\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_PASSWORD_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_password at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_PASSWORD_ALIVE, self->object_tag);\
        ipr_password_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#define IPR_PASSWD_BASIC        1       //  Basic encoding
#define IPR_PASSWD_DIGEST       2       //  Digest encoding
#   ifdef __cplusplus
}
#   endif

#   include "ipr_regexp.h"
#   include "ipr_nonce.h"
#   include "ipr_password_table.h"
#   include "ipr_password_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PASSWORD_ACTIVE
#   if defined (ICL_IMPORT_IPR_PASSWORD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_PASSWORD_UNSAFE
#   define INCLUDE_IPR_PASSWORD_UNSAFE
#   define INCLUDE_IPR_PASSWORD_ACTIVE

#   include "ipr_regexp.h"
#   include "ipr_nonce.h"
#   include "ipr_password_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_password_animating;

//  Structure of the ipr_password object

struct _ipr_password_t {
#if (defined (BASE_THREADSAFE))
    icl_mutex_t *
        mutex;
#endif
    dbyte
        object_tag;                     //  Object validity marker
ipr_password_table_t
    *table_head;                    //  Hash table container
ipr_password_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
volatile ipr_password_list_t *list_head;
volatile ipr_password_t *list_prev;
volatile ipr_password_t *list_next;
char
    *hash;                          //  Password hash
int
    type;                           //  IPR_PASSWD_ value
//  Guard data
size_t
    hits,                           //  Total hits
    hit_quota,                      //  Hits remaining, or 0
    misses,                         //  Total misses
    miss_quota;                     //  Misses remaining, or 0
apr_time_t
    expiry,                         //  Expiration time, or 0
    hit_time,                       //  Time of last hit
    miss_time;                      //  Time of last miss
icl_shortstr_t
    hit_from,                       //  Last good IP address
    miss_from;                      //  Last bad IP address
Bool
    expired;                        //  Run out of luck...
};

#   ifdef __cplusplus
}
#   endif
#   include "ipr_password_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_password_new(table,key,hash)  ipr_password_new_ (__FILE__, __LINE__, table, key, hash)
ipr_password_t *
    ipr_password_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_password_table_t * table,           //  Table to insert into
char * key,                             //  Hash key
char * hash                             //  Not documented
);

int
    ipr_password_update (
ipr_password_t * self,                  //  Reference to object
char * hash                             //  Not documented
);

ipr_password_t *
    ipr_password_assume (
ipr_password_table_t * table,           //  Not documented
char * key,                             //  Not documented
char * hash                             //  Not documented
);

int
    ipr_password_guard_set (
ipr_password_t * self,                  //  Reference to object
size_t hit_quota,                       //  Not documented
size_t miss_quota,                      //  Not documented
size_t time_to_live                     //  Not documented
);

int
    ipr_password_guard_get (
ipr_password_t * self,                  //  Reference to object
char * options                          //  name=value ...
);

int
    ipr_password_guard_put (
ipr_password_t * self,                  //  Reference to object
FILE * file                             //  Not documented
);

int
    ipr_password_hit (
ipr_password_t * self,                  //  Reference to object
char * address                          //  Not documented
);

int
    ipr_password_miss (
ipr_password_t * self,                  //  Reference to object
char * address                          //  Not documented
);

void
    ipr_password_selftest (
void);

void
    ipr_password_remove_from_all_containers (
ipr_password_t * self                   //  The itemThe item
);

#define ipr_password_show(item,opcode,trace_file)  ipr_password_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_password_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_password_terminate (
void);

#define ipr_password_destroy(self)      ipr_password_destroy_ (self, __FILE__, __LINE__)
void
    ipr_password_destroy_ (
ipr_password_t * ( * self_p ),          //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_password_lock (
ipr_password_t * self                   //  Reference to object
);

int
    ipr_password_unlock (
ipr_password_t * self                   //  Reference to object
);

void
    ipr_password_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_password_new_in_scope(self_p,_scope,table,key,hash)  ipr_password_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, key, hash)
void
    ipr_password_new_in_scope_ (
ipr_password_t * * self_p,              //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_password_table_t * table,           //  Table to insert into
char * key,                             //  Hash key
char * hash                             //  Not documented
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PASSWORD_ACTIVE
#   if defined (ICL_IMPORT_IPR_PASSWORD_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_PASSWORD_INLINE)
#   define INCLUDE_IPR_PASSWORD_INLINE
#   define INCLUDE_IPR_PASSWORD_ACTIVE

#   include "ipr_regexp.h"
#   include "ipr_nonce.h"
#   include "ipr_password_table.h"
#   include "ipr_password_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PASSWORD_ACTIVE
#   if defined (ICL_IMPORT_IPR_PASSWORD_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_PASSWORD_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_process.h - ipr_process component

    Generated from ipr_process.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_PROCESS_SAFE
#   define INCLUDE_IPR_PROCESS_SAFE
#   define INCLUDE_IPR_PROCESS_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_PROCESS_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_process_t ipr_process_t;

#define IPR_PROCESS_ALIVE               0xFABB
#define IPR_PROCESS_DEAD                0xDEAD

#define IPR_PROCESS_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_process\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_PROCESS_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_process at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_PROCESS_ALIVE, self->object_tag);\
        ipr_process_show (self, ICL_CALLBACK_DUMP, stderr);\
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
#   undef INCLUDE_IPR_PROCESS_ACTIVE
#   if defined (ICL_IMPORT_IPR_PROCESS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_PROCESS_UNSAFE
#   define INCLUDE_IPR_PROCESS_UNSAFE
#   define INCLUDE_IPR_PROCESS_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_process_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_process_new(command,in_file,out_file,err_file)  ipr_process_new_ (__FILE__, __LINE__, command, in_file, out_file, err_file)
ipr_process_t *
    ipr_process_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * command,                         //  Full command-line to run
char * in_file,                         //  Provide standard input, or empty/NULL
char * out_file,                        //  Capture standard output, or empty/NULL
char * err_file                         //  Capture standard error, or empty/NULL
);

int
    ipr_process_start (
ipr_process_t * self,                   //  Reference to object
char * directory                        //  Working directory, or null
);

int
    ipr_process_wait (
ipr_process_t * self,                   //  Reference to object
Bool sync                               //  Wait for process to end?
);

int
    ipr_process_kill (
ipr_process_t * self                    //  Reference to object
);

int
    ipr_process_server (
char * workdir,                         //  Where server runs, or NULL/""
char * lockfile,                        //  For exclusive execution
int argc,                               //  Original command-line arguments
char ** argv,                           //  Original command-line arguments
char ** sswitch                         //  Filter these options from argv
);

int
    ipr_process_id (
void);

void
    ipr_process_selftest (
void);

Bool
    ipr_process_finished (
ipr_process_t * self                    //  Not documented
);

int
    ipr_process_exitcode (
ipr_process_t * self                    //  Not documented
);

void
    ipr_process_terminate (
void);

#define ipr_process_show(item,opcode,trace_file)  ipr_process_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_process_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_process_destroy(self)       ipr_process_destroy_ (self, __FILE__, __LINE__)
void
    ipr_process_destroy_ (
ipr_process_t * ( * self_p ),           //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    ipr_process_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_process_new_in_scope(self_p,_scope,command,in_file,out_file,err_file)  ipr_process_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, command, in_file, out_file, err_file)
void
    ipr_process_new_in_scope_ (
ipr_process_t * * self_p,               //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * command,                         //  Full command-line to run
char * in_file,                         //  Provide standard input, or empty/NULL
char * out_file,                        //  Capture standard output, or empty/NULL
char * err_file                         //  Capture standard error, or empty/NULL
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PROCESS_ACTIVE
#   if defined (ICL_IMPORT_IPR_PROCESS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_PROCESS_INLINE)
#   define INCLUDE_IPR_PROCESS_INLINE
#   define INCLUDE_IPR_PROCESS_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_PROCESS_ACTIVE
#   if defined (ICL_IMPORT_IPR_PROCESS_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_PROCESS_ROOT
#   endif
# endif
#endif
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
/*---------------------------------------------------------------------------
    ipr_sasl.h - ipr_sasl component

    Provides functions for working with SASL (simple authentication and
    security layer).
    Generated from ipr_sasl.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_SASL_SAFE
#   define INCLUDE_IPR_SASL_SAFE
#   define INCLUDE_IPR_SASL_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_SASL_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_SASL_ACTIVE
#   if defined (ICL_IMPORT_IPR_SASL_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_SASL_UNSAFE
#   define INCLUDE_IPR_SASL_UNSAFE
#   define INCLUDE_IPR_SASL_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_sasl_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
icl_longstr_t *
    ipr_sasl_plain_encode (
char * login,                           //  User login name
char * password                         //  User password
);

int
    ipr_sasl_plain_decode (
icl_longstr_t * auth_data,              //  Authentication data
char * login,                           //  User login name
char * password                         //  User password
);

void
    ipr_sasl_selftest (
void);

void
    ipr_sasl_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_SASL_ACTIVE
#   if defined (ICL_IMPORT_IPR_SASL_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_SASL_INLINE)
#   define INCLUDE_IPR_SASL_INLINE
#   define INCLUDE_IPR_SASL_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_SASL_ACTIVE
#   if defined (ICL_IMPORT_IPR_SASL_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_SASL_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_stat.h - ipr_stat component

    The statistics module is designed to capture information (raw integers)
    and process these into useful information.  The sample size is arbitrarily
    large.  Samples are double-precision floating point.
    Generated from ipr_stat.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_STAT_SAFE
#   define INCLUDE_IPR_STAT_SAFE
#   define INCLUDE_IPR_STAT_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_STAT_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_stat_t ipr_stat_t;

#define IPR_STAT_ALIVE                  0xFABB
#define IPR_STAT_DEAD                   0xDEAD

#define IPR_STAT_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_stat\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_STAT_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_stat at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_STAT_ALIVE, self->object_tag);\
        ipr_stat_show (self, ICL_CALLBACK_DUMP, stderr);\
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
#   undef INCLUDE_IPR_STAT_ACTIVE
#   if defined (ICL_IMPORT_IPR_STAT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_STAT_UNSAFE
#   define INCLUDE_IPR_STAT_UNSAFE
#   define INCLUDE_IPR_STAT_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_stat_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_stat_new()                  ipr_stat_new_ (__FILE__, __LINE__)
ipr_stat_t *
    ipr_stat_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

int
    ipr_stat_record (
ipr_stat_t * self,                      //  Reference to object
double value                            //  Sample value
);

double
    ipr_stat_min (
ipr_stat_t * self                       //  Reference to object
);

double
    ipr_stat_max (
ipr_stat_t * self                       //  Reference to object
);

int
    ipr_stat_count (
ipr_stat_t * self                       //  Reference to object
);

double
    ipr_stat_sum (
ipr_stat_t * self                       //  Reference to object
);

double
    ipr_stat_mean (
ipr_stat_t * self                       //  Reference to object
);

double
    ipr_stat_var (
ipr_stat_t * self                       //  Reference to object
);

double
    ipr_stat_dev (
ipr_stat_t * self                       //  Reference to object
);

void
    ipr_stat_selftest (
void);

void
    ipr_stat_terminate (
void);

#define ipr_stat_show(item,opcode,trace_file)  ipr_stat_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_stat_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_stat_destroy(self)          ipr_stat_destroy_ (self, __FILE__, __LINE__)
void
    ipr_stat_destroy_ (
ipr_stat_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    ipr_stat_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_stat_new_in_scope(self_p,_scope)  ipr_stat_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_stat_new_in_scope_ (
ipr_stat_t * * self_p,                  //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_STAT_ACTIVE
#   if defined (ICL_IMPORT_IPR_STAT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_STAT_INLINE)
#   define INCLUDE_IPR_STAT_INLINE
#   define INCLUDE_IPR_STAT_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_STAT_ACTIVE
#   if defined (ICL_IMPORT_IPR_STAT_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_STAT_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_str.h - ipr_str component

    Provides various string manipulation functions.
    Generated from ipr_str.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_STR_SAFE
#   define INCLUDE_IPR_STR_SAFE
#   define INCLUDE_IPR_STR_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_STR_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif

#   include "icl_shortstr.h"
#   include "ipr_regexp.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_STR_ACTIVE
#   if defined (ICL_IMPORT_IPR_STR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_STR_UNSAFE
#   define INCLUDE_IPR_STR_UNSAFE
#   define INCLUDE_IPR_STR_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_regexp.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_str_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
char *
    ipr_str_skip (
char * string                           //  Input string
);

char *
    ipr_str_wipe (
char * string,                          //  Input string
char ch                                 //  Character to use
);

char *
    ipr_str_lower (
char * string                           //  Input string
);

char *
    ipr_str_upper (
char * string                           //  Input string
);

char *
    ipr_str_subch (
char * string,                          //  Input string
char from,                              //  Character to look for
char to                                 //  Character to replace with
);

char *
    ipr_str_crop (
char * string                           //  Input string
);

char *
    ipr_str_block (
char * string,                          //  Input string
char content,                           //  Character to repeat
size_t count                            //  Number of repetitions
);

char *
    ipr_str_format (
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

char *
    ipr_str_format_v (
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

char *
    ipr_str_filter (
char * string                           //  Input string
);

int
    ipr_str_match (
char * string1,                         //  First string
char * string2                          //  Second string
);

Bool
    ipr_str_prefixed (
char * string,                          //  Input string
char * prefix                           //  Prefix string
);

char *
    ipr_str_defix (
char * string,                          //  Input string
char * prefix                           //  Prefix to remove
);

char *
    ipr_str_clean (
char * string,                          //  Input string
char from,                              //  Char to look for
char to                                 //  Char to replace with
);

Bool
    ipr_str_lexeq (
char * string1,                         //  Input string 1
char * string2                          //  Input string 2
);

int
    ipr_str_lexcmp (
char * string1,                         //  Input string 1
char * string2                          //  Input string 2
);

char *
    ipr_str_soundex (
char * string,                          //  Input string
char * encoding                         //  Resulting soundex
);

Bool
    ipr_str_matches (
char * string,                          //  Input string
char * pattern                          //  Input pattern
);

Bool
    ipr_str_lex_matches (
char * string,                          //  Input string
char * pattern                          //  Input pattern
);

char *
    ipr_str_find (
char * haystack,                        //  String to search in
char * needle                           //  String to search for
);

char *
    ipr_str_random (
char * string,                          //  String to fill
char * pattern                          //  Pattern
);

Bool
    ipr_str_numeric (
char * string                           //  Input string 1
);

char *
    ipr_str_md5 (
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

char *
    ipr_str_md5_v (
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

void
    ipr_str_selftest (
void);

void
    ipr_str_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_STR_ACTIVE
#   if defined (ICL_IMPORT_IPR_STR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_STR_INLINE)
#   define INCLUDE_IPR_STR_INLINE
#   define INCLUDE_IPR_STR_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_regexp.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_STR_ACTIVE
#   if defined (ICL_IMPORT_IPR_STR_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_STR_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_symbol.h - ipr_symbol component

Implements a symbol table that holds arbitrary data, keyed by name.
Can be used directly, or inherited to create specific symbol table
structures.
    Generated from ipr_symbol.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_SYMBOL_SAFE
#   define INCLUDE_IPR_SYMBOL_SAFE
#   define INCLUDE_IPR_SYMBOL_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_SYMBOL_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_symbol_t ipr_symbol_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
#    define IPR_SYMBOL_HISTORY_LENGTH 32
#endif

#define IPR_SYMBOL_ALIVE                0xFABB
#define IPR_SYMBOL_DEAD                 0xDEAD

#define IPR_SYMBOL_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_symbol\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_SYMBOL_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_symbol at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_SYMBOL_ALIVE, self->object_tag);\
        ipr_symbol_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_regexp.h"
#   include "ipr_symbol_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_SYMBOL_ACTIVE
#   if defined (ICL_IMPORT_IPR_SYMBOL_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_SYMBOL_UNSAFE
#   define INCLUDE_IPR_SYMBOL_UNSAFE
#   define INCLUDE_IPR_SYMBOL_ACTIVE

#   include "ipr_regexp.h"
#   include "ipr_symbol_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_symbol_animating;

//  Structure of the ipr_symbol object

struct _ipr_symbol_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_SYMBOL)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_SYMBOL_HISTORY_LENGTH];
    int
        history_line  [IPR_SYMBOL_HISTORY_LENGTH];
    char
        *history_type [IPR_SYMBOL_HISTORY_LENGTH];
    int
        history_links [IPR_SYMBOL_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
ipr_symbol_table_t
    *table_head;                    //  Hash table container
ipr_symbol_t
    *table_next;                    //  Next item in the hash slot
qbyte
    table_index;                    //  Index of item in table
icl_shortstr_t
    key;                            //  Item's original key
void
    *data;                          //  Data value for item
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_symbol_new(table,key,data)  ipr_symbol_new_ (__FILE__, __LINE__, table, key, data)
ipr_symbol_t *
    ipr_symbol_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_symbol_table_t * table,             //  Table to insert into
char * key,                             //  Hash key
void * data                             //  Data for symbol
);

void
    ipr_symbol_assume (
ipr_symbol_table_t * table,             //  Symbol table
char * name,                            //  Symbol name
void * data                             //  Data for symbol
);

void
    ipr_symbol_selftest (
void);

void
    ipr_symbol_remove_from_all_containers (
ipr_symbol_t * self                     //  The item
);

#define ipr_symbol_show(item,opcode,trace_file)  ipr_symbol_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_symbol_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_symbol_terminate (
void);

#define ipr_symbol_destroy(self)        ipr_symbol_destroy_ (self, __FILE__, __LINE__)
void
    ipr_symbol_destroy_ (
ipr_symbol_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

int
    ipr_symbol_read_lock (
ipr_symbol_t * self                     //  Reference to object
);

int
    ipr_symbol_write_lock (
ipr_symbol_t * self                     //  Reference to object
);

int
    ipr_symbol_unlock (
ipr_symbol_t * self                     //  Reference to object
);

#define ipr_symbol_link(self)           ipr_symbol_link_ (self, __FILE__, __LINE__)
ipr_symbol_t *
    ipr_symbol_link_ (
ipr_symbol_t * self,                    //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_symbol_unlink(self)         ipr_symbol_unlink_ (self, __FILE__, __LINE__)
void
    ipr_symbol_unlink_ (
ipr_symbol_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_symbol_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_symbol_new_in_scope(self_p,_scope,table,key,data)  ipr_symbol_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, table, key, data)
void
    ipr_symbol_new_in_scope_ (
ipr_symbol_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
ipr_symbol_table_t * table,             //  Table to insert into
char * key,                             //  Hash key
void * data                             //  Data for symbol
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_SYMBOL_ACTIVE
#   if defined (ICL_IMPORT_IPR_SYMBOL_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_SYMBOL_INLINE)
#   define INCLUDE_IPR_SYMBOL_INLINE
#   define INCLUDE_IPR_SYMBOL_ACTIVE

#   include "ipr_regexp.h"
#   include "ipr_symbol_table.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_SYMBOL_ACTIVE
#   if defined (ICL_IMPORT_IPR_SYMBOL_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_SYMBOL_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_token.h - ipr_token component

Provides string tokenisation functions.  Use this to split a string
into a list of words (delimited by spaces) so that each word can be
manipulated separately.
    Generated from ipr_token.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_TOKEN_SAFE
#   define INCLUDE_IPR_TOKEN_SAFE
#   define INCLUDE_IPR_TOKEN_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_TOKEN_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_token_t ipr_token_t;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
#    define IPR_TOKEN_HISTORY_LENGTH 32
#endif

#define IPR_TOKEN_ALIVE                 0xFABB
#define IPR_TOKEN_DEAD                  0xDEAD

#define IPR_TOKEN_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_token\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_TOKEN_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_token at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_TOKEN_ALIVE, self->object_tag);\
        ipr_token_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "icl_shortstr.h"
#   include "ipr_str.h"
#   include "ipr_token_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TOKEN_ACTIVE
#   if defined (ICL_IMPORT_IPR_TOKEN_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_TOKEN_UNSAFE
#   define INCLUDE_IPR_TOKEN_UNSAFE
#   define INCLUDE_IPR_TOKEN_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_str.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_token_animating;

//  Structure of the ipr_token object

struct _ipr_token_t {
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [IPR_TOKEN_HISTORY_LENGTH];
    int
        history_line  [IPR_TOKEN_HISTORY_LENGTH];
    char
        *history_type [IPR_TOKEN_HISTORY_LENGTH];
    int
        history_links [IPR_TOKEN_HISTORY_LENGTH];
#endif
    dbyte
        object_tag;                     //  Object validity marker
volatile ipr_token_list_t *list_head;
volatile ipr_token_t *list_prev;
volatile ipr_token_t *list_next;
icl_shortstr_t
    value;                          //  Item value
};

#   ifdef __cplusplus
}
#   endif
#   include "ipr_token_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
ipr_token_list_t *
    ipr_token_split (
char * string                           //  String to parse
);

ipr_token_list_t *
    ipr_token_split_rich (
char * string,                          //  String to parse
char * delims                           //  Delimiters
);

void
    ipr_token_selftest (
void);

#define ipr_token_new()                 ipr_token_new_ (__FILE__, __LINE__)
ipr_token_t *
    ipr_token_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_token_remove_from_all_containers (
ipr_token_t * self                      //  The item
);

#define ipr_token_show(item,opcode,trace_file)  ipr_token_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_token_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

void
    ipr_token_terminate (
void);

#define ipr_token_destroy(self)         ipr_token_destroy_ (self, __FILE__, __LINE__)
void
    ipr_token_destroy_ (
ipr_token_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

#define ipr_token_link(self)            ipr_token_link_ (self, __FILE__, __LINE__)
ipr_token_t *
    ipr_token_link_ (
ipr_token_t * self,                     //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_token_unlink(self)          ipr_token_unlink_ (self, __FILE__, __LINE__)
void
    ipr_token_unlink_ (
ipr_token_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

void
    ipr_token_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_token_new_in_scope(self_p,_scope)  ipr_token_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    ipr_token_new_in_scope_ (
ipr_token_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TOKEN_ACTIVE
#   if defined (ICL_IMPORT_IPR_TOKEN_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_TOKEN_INLINE)
#   define INCLUDE_IPR_TOKEN_INLINE
#   define INCLUDE_IPR_TOKEN_ACTIVE

#   include "icl_shortstr.h"
#   include "ipr_str.h"
#   include "ipr_token_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TOKEN_ACTIVE
#   if defined (ICL_IMPORT_IPR_TOKEN_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_TOKEN_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_tree.h - ipr_tree component

    Implements an XML tree builder - an easy way to construct XML trees
    for delivery to applications.  To load or modify XML trees, use the
    ipr_xml class interface.
    Generated from ipr_tree.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_TREE_SAFE
#   define INCLUDE_IPR_TREE_SAFE
#   define INCLUDE_IPR_TREE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_TREE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _ipr_tree_t ipr_tree_t;

#define IPR_TREE_ALIVE                  0xFABB
#define IPR_TREE_DEAD                   0xDEAD

#define IPR_TREE_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL ipr_tree\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != IPR_TREE_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "ipr_tree at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, IPR_TREE_ALIVE, self->object_tag);\
        ipr_tree_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "ipr_xml.h"
#   include "ipr_xml_attr.h"
#   include "ipr_bucket.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TREE_ACTIVE
#   if defined (ICL_IMPORT_IPR_TREE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_TREE_UNSAFE
#   define INCLUDE_IPR_TREE_UNSAFE
#   define INCLUDE_IPR_TREE_ACTIVE

#   include "ipr_xml.h"
#   include "ipr_xml_attr.h"
#   include "ipr_bucket.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_tree_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define ipr_tree_new(name)              ipr_tree_new_ (__FILE__, __LINE__, name)
ipr_tree_t *
    ipr_tree_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * name                             //  The name of the root item
);

int
    ipr_tree_open (
ipr_tree_t * self,                      //  Reference to object
char * name                             //  The name of the new branch
);

int
    ipr_tree_leaf (
ipr_tree_t * self,                      //  Reference to object
char * name,                            //  The name of the new leaf
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

int
    ipr_tree_leaf_v (
ipr_tree_t * self,                      //  Reference to object
char * name,                            //  The name of the new leaf
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

int
    ipr_tree_shut (
ipr_tree_t * self                       //  Reference to object
);

icl_longstr_t *
    ipr_tree_save_xml (
ipr_tree_t * self                       //  Reference to tree
);

icl_longstr_t *
    ipr_tree_save_json (
ipr_tree_t * self                       //  Reference to tree
);

void
    ipr_tree_selftest (
void);

void
    ipr_tree_terminate (
void);

#define ipr_tree_show(item,opcode,trace_file)  ipr_tree_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    ipr_tree_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_tree_destroy(self)          ipr_tree_destroy_ (self, __FILE__, __LINE__)
void
    ipr_tree_destroy_ (
ipr_tree_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    ipr_tree_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define ipr_tree_new_in_scope(self_p,_scope,name)  ipr_tree_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, name)
void
    ipr_tree_new_in_scope_ (
ipr_tree_t * * self_p,                  //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
char * name                             //  The name of the root item
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TREE_ACTIVE
#   if defined (ICL_IMPORT_IPR_TREE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_TREE_INLINE)
#   define INCLUDE_IPR_TREE_INLINE
#   define INCLUDE_IPR_TREE_ACTIVE

#   include "ipr_xml.h"
#   include "ipr_xml_attr.h"
#   include "ipr_bucket.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TREE_ACTIVE
#   if defined (ICL_IMPORT_IPR_TREE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_TREE_ROOT
#   endif
# endif
#endif
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
/*===========================================================================

    XML loading and saving functions

    Provides functions to load and save XML files.  An XML file
    is held in memory as a tree of nodes, of type ipr_xml_t.  The
    XML functions do not currently accept DTDs in the XML data.

    Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 *===========================================================================*/

#ifndef ICL_XMLL_INCLUDED               //  Allow multiple inclusions
#define ICL_XMLL_INCLUDED


//- Function prototypes ----------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//  Error values

#define IPR_XML_NOERROR         0           //  No errors
#define IPR_XML_FILEERROR       1           //  Error in file i/o
#define IPR_XML_LOADERROR       2           //  Error loading XML


//  Function prototypes

int   ipr_xml_seems_to_be (char *path, char *filename);
char *ipr_xml_error       (void);

int   ipr_xml_load_file   (ipr_xml_t **item, char *path, char *filename, Bool extend);
int   ipr_xml_save_file   (ipr_xml_t  *item, char *filename);

int   ipr_xml_load_string (ipr_xml_t **item, char *xmlstring, Bool extend);
char *ipr_xml_save_string (ipr_xml_t  *item);

int   ipr_xml_load_bucket (ipr_xml_t **item, ipr_bucket_t *bucket, Bool extend);
ipr_bucket_t *ipr_xml_save_bucket (ipr_xml_t  *item);


//  Macros

#define ipr_xml_load_extended(item, path, filename)  ipr_xml_load_file (item, path, filename, TRUE)
#define ipr_xml_load(item, path, filename)           ipr_xml_load_file (item, path, filename, FALSE)


#ifdef __cplusplus
}
#endif

#endif                                  //  Included
/*---------------------------------------------------------------------------
    ipr_uid.h - ipr_uid component

    Provides functions to access and maniuplate the current process
    user and group IDs.
    Generated from ipr_uid.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_UID_SAFE
#   define INCLUDE_IPR_UID_SAFE
#   define INCLUDE_IPR_UID_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_UID_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif

#   include "icl_shortstr.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_UID_ACTIVE
#   if defined (ICL_IMPORT_IPR_UID_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_UID_UNSAFE
#   define INCLUDE_IPR_UID_UNSAFE
#   define INCLUDE_IPR_UID_ACTIVE

#   include "icl_shortstr.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_uid_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
int
    ipr_uid_user_name (
uid_t uid,                              //  UID to look up
char * name                             //  String to receive name
);

int
    ipr_uid_group_name (
uid_t gid,                              //  GID to look up
char * name                             //  String to receive name
);

int
    ipr_uid_set_real_user (
void);

int
    ipr_uid_set_root_user (
void);

int
    ipr_uid_set_real_group (
void);

int
    ipr_uid_set_root_group (
void);

int
    ipr_uid_get_uid (
void);

int
    ipr_uid_get_gid (
void);

int
    ipr_uid_run_as (
char * user,                            //  Name of user to switch to
char * group                            //  Name of group to switch to
);

void
    ipr_uid_selftest (
void);

void
    ipr_uid_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_UID_ACTIVE
#   if defined (ICL_IMPORT_IPR_UID_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_UID_INLINE)
#   define INCLUDE_IPR_UID_INLINE
#   define INCLUDE_IPR_UID_ACTIVE

#   include "icl_shortstr.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_UID_ACTIVE
#   if defined (ICL_IMPORT_IPR_UID_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_UID_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    ipr_time.h - ipr_time component

    Provides various date & time manipulation functions.
    Generated from ipr_time.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_IPR_TIME_SAFE
#   define INCLUDE_IPR_TIME_SAFE
#   define INCLUDE_IPR_TIME_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_IPR_TIME_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif

#   include "ipr_str.h"
#   include "ipr_regexp.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TIME_ACTIVE
#   if defined (ICL_IMPORT_IPR_TIME_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_IPR_TIME_UNSAFE
#   define INCLUDE_IPR_TIME_UNSAFE
#   define INCLUDE_IPR_TIME_ACTIVE

#   include "ipr_str.h"
#   include "ipr_regexp.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    ipr_time_animating;
typedef enum
{
    // YYYY                     (will render to 4 characters)
    ipr_date_format_year,
    // YYYY-MM                  (will render to 7 characters)
    ipr_date_format_month,
    // YYYY-MM-DD               (will render to 10 characters)
    ipr_date_format_day,
    // YYYY-MM-DDThh:mmTZD      (will render to 22 characters)
    ipr_date_format_minute,
    // YYYY-MM-DDThh:mm:ssTZD   (will render to 25 characters)
    ipr_date_format_second,
    // YYYY-MM-DDThh:mm:ss.sTZD (will render to 26 + precision characters)
    ipr_date_format_fraction
} ipr_date_format_t;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
char *
    ipr_time_iso8601 (
apr_time_t time,                        //  Time to render
ipr_date_format_t format,               //  Format to use
byte precision,                         //  Decimals for second fractions
Bool utc,                               //  Format as UTC?
char * buffer                           //  Rendered output
);

apr_time_t
    ipr_time_iso8601_decode (
char * iso8601                          //  ISO format date/time
);

char *
    ipr_time_mime (
apr_time_t time,                        //  Time to format
char * buffer                           //  Shortstring for result
);

apr_time_t
    ipr_time_mime_decode (
char * mime                             //  MIME-encoded date/time
);

long
    ipr_time_now (
void);

long
    ipr_time_zone (
void);

void
    ipr_time_selftest (
void);

void
    ipr_time_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TIME_ACTIVE
#   if defined (ICL_IMPORT_IPR_TIME_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_IPR_TIME_INLINE)
#   define INCLUDE_IPR_TIME_INLINE
#   define INCLUDE_IPR_TIME_ACTIVE

#   include "ipr_str.h"
#   include "ipr_regexp.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_IPR_TIME_ACTIVE
#   if defined (ICL_IMPORT_IPR_TIME_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_IPR_TIME_ROOT
#   endif
# endif
#endif
