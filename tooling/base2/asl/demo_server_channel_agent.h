/*---------------------------------------------------------------------------
    demo_server_channel_agent.h - prototypes for demo_server_channel_agent.

    Generated from demo_server_channel_agent.smt by smt_gen.gsl using GSL/4.

    
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
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_DEMO_SERVER_CHANNEL_AGENT_SAFE
#   define INCLUDE_DEMO_SERVER_CHANNEL_AGENT_SAFE
#   define INCLUDE_DEMO_SERVER_CHANNEL_AGENT_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_DEMO_SERVER_CHANNEL_AGENT_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif

#   include "demo_server_channel.h"

#   undef INCLUDE_DEMO_SERVER_CHANNEL_AGENT_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_CHANNEL_AGENT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_DEMO_SERVER_CHANNEL_AGENT_UNSAFE
#   define INCLUDE_DEMO_SERVER_CHANNEL_AGENT_UNSAFE
#   define INCLUDE_DEMO_SERVER_CHANNEL_AGENT_ACTIVE

#   include "demo_server_channel.h"
#ifdef __cplusplus
extern "C" {
#endif

int
demo_server_channel_agent_consume (
    smt_thread_t * thread,
    demo_server_method_t * method);

int
demo_server_channel_agent_cancel (
    smt_thread_t * thread,
    demo_server_method_t * method);

int
demo_server_channel_agent_destroy (
    smt_thread_t * thread,
    char *                file,
    size_t                line);

int  demo_server_channel_agent_init (void);
void demo_server_channel_agent_term (void);
void demo_server_channel_agent_animate (Bool animate);
smt_thread_t *demo_server_channel_agent_class_thread_new (
    demo_server_channel_t * self);

#ifdef __cplusplus
}
#endif


#   undef INCLUDE_DEMO_SERVER_CHANNEL_AGENT_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_CHANNEL_AGENT_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_DEMO_SERVER_CHANNEL_AGENT_INLINE)
#   define INCLUDE_DEMO_SERVER_CHANNEL_AGENT_INLINE
#   define INCLUDE_DEMO_SERVER_CHANNEL_AGENT_ACTIVE

#   include "demo_server_channel.h"

#   undef INCLUDE_DEMO_SERVER_CHANNEL_AGENT_ACTIVE
#   if defined (ICL_IMPORT_DEMO_SERVER_CHANNEL_AGENT_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_DEMO_SERVER_CHANNEL_AGENT_ROOT
#   endif
# endif
#endif
