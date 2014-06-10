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
