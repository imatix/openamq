/*---------------------------------------------------------------------------
    demo_constants.h - demo_constants component

This class defines a set of constants for the demo
protocol.
    Generated from demo_constants.icl by icl_gen using GSL/4.
    
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
# ifndef INCLUDE_DEMO_CONSTANTS_SAFE
#   define INCLUDE_DEMO_CONSTANTS_SAFE
#   define INCLUDE_DEMO_CONSTANTS_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_DEMO_CONSTANTS_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#define ASL_FRAME_METHOD                1
#define ASL_FRAME_HEADER                2
#define ASL_FRAME_BODY                  3
#define ASL_FRAME_OOB_METHOD            4
#define ASL_FRAME_OOB_HEADER            5
#define ASL_FRAME_OOB_BODY              6
#define ASL_FRAME_TRACE                 7
#define ASL_FRAME_HEARTBEAT             8
#define ASL_FRAME_MIN_SIZE              4096
#define ASL_FRAME_END                   206
#define ASL_REPLY_SUCCESS               200
#define ASL_NOT_DELIVERED               310
#define ASL_CONTENT_TOO_LARGE           311
#define ASL_CONNECTION_FORCED           320
#define ASL_INVALID_PATH                402
#define ASL_ACCESS_REFUSED              403
#define ASL_NOT_FOUND                   404
#define ASL_RESOURCE_LOCKED             405
#define ASL_FRAME_ERROR                 501
#define ASL_SYNTAX_ERROR                502
#define ASL_COMMAND_INVALID             503
#define ASL_CHANNEL_ERROR               504
#define ASL_RESOURCE_ERROR              506
#define ASL_NOT_ALLOWED                 530
#define ASL_NOT_IMPLEMENTED             540
#define ASL_INTERNAL_ERROR              541

#define ASL_SOFT_ERROR(r)               (  (r) == 310 \
                                        || (r) == 311 \
                                        || (r) == 403 \
                                        || (r) == 404 \
                                        || (r) == 405 \
                                         )

#define ASL_HARD_ERROR(r)               (  (r) == 320 \
                                        || (r) == 402 \
                                        || (r) == 501 \
                                        || (r) == 502 \
                                        || (r) == 503 \
                                        || (r) == 504 \
                                        || (r) == 506 \
                                        || (r) == 530 \
                                        || (r) == 540 \
                                        || (r) == 541 \
                                         )
#   ifdef __cplusplus
}
#   endif

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_CONSTANTS_ACTIVE
#   if defined (ICL_IMPORT_DEMO_CONSTANTS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_DEMO_CONSTANTS_UNSAFE
#   define INCLUDE_DEMO_CONSTANTS_UNSAFE
#   define INCLUDE_DEMO_CONSTANTS_ACTIVE


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
#   undef INCLUDE_DEMO_CONSTANTS_ACTIVE
#   if defined (ICL_IMPORT_DEMO_CONSTANTS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_DEMO_CONSTANTS_INLINE)
#   define INCLUDE_DEMO_CONSTANTS_INLINE
#   define INCLUDE_DEMO_CONSTANTS_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_DEMO_CONSTANTS_ACTIVE
#   if defined (ICL_IMPORT_DEMO_CONSTANTS_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_DEMO_CONSTANTS_ROOT
#   endif
# endif
#endif
