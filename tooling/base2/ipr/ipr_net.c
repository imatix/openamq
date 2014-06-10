/*---------------------------------------------------------------------------
    ipr_net.c - ipr_net component

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
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "ipr_net.h"                    //  Definitions for our class

//  Shorthands for class methods

#define self_get_addr       ipr_net_get_addr
#define self_get_hostname   ipr_net_get_hostname
#define self_validate_addr  ipr_net_validate_addr
#define self_ping           ipr_net_ping
#define self_ping_list      ipr_net_ping_list
#define self_lookup_host    ipr_net_lookup_host
#define self_fmt_display    ipr_net_fmt_display
#define self_cidr_eq        ipr_net_cidr_eq
#define self_selftest       ipr_net_selftest
#define self_show_animation  ipr_net_show_animation

Bool
    ipr_net_animating = TRUE;           //  Animation enabled by default
/*  -------------------------------------------------------------------------
    ipr_net_get_addr

    Type: Component method
    Returns the first network address of the current machine.  Copies
    the network address into a short string provided by the caller.
    -------------------------------------------------------------------------
 */

void
    ipr_net_get_addr (
    char * dest                         //  Network address
)
{
icl_shortstr_t
    buffer;                         //  Holds host name
apr_pool_t
    *pool;                          //  APR working pool
apr_sockaddr_t
    *sockaddr;                      //  The associated socket addresses
char
    *addr;                          //  Formatted IP address

//
icl_apr_assert (apr_pool_create (&pool, icl_global_pool));
apr_pool_tag    (pool, "ipr_net_get addr");
apr_gethostname (buffer, ICL_SHORTSTR_MAX, pool);
apr_sockaddr_info_get (&sockaddr, buffer, APR_UNSPEC, 0, 0, pool);
if (sockaddr) {
    apr_sockaddr_ip_get (&addr, sockaddr);
    icl_shortstr_cpy (dest, addr);
}
else
    icl_shortstr_cpy (dest, "127.0.0.1");
apr_pool_destroy (pool);
}
/*  -------------------------------------------------------------------------
    ipr_net_get_hostname

    Type: Component method
    Returns a string containing the local host name. Copies the host
    name into a short string provided by the caller.
    -------------------------------------------------------------------------
 */

void
    ipr_net_get_hostname (
    char * dest                         //  Network address
)
{
apr_pool_t
    *pool;                          //  APR working pool

//
icl_apr_assert (apr_pool_create (&pool, icl_global_pool));
apr_pool_tag    (pool, "ipr_net_get hostname");
apr_gethostname (dest, ICL_SHORTSTR_MAX, pool);
apr_pool_destroy (pool);
}
/*  -------------------------------------------------------------------------
    ipr_net_validate_addr

    Type: Component method
    Validates a network address of the form host:port. Port is optional. 
    Host is obligatory.
    -------------------------------------------------------------------------
 */

Bool
    ipr_net_validate_addr (
    char * netaddr                      //  Network address to validate
)
{
char
    *addr,                         //  The address parsed by APR
    *scope_id;                     //  The scope parsed by APR
apr_pool_t
    *pool;                         //  APR pool for host addresses
apr_port_t
    port;                          //  The port parsed from the address
apr_status_t                       //  APR return value of validation
    rv;
apr_sockaddr_t
    *sa;                           //  The socket used for resolving
    Bool
        rc;                             //  True if network address is valid

//
icl_apr_assert (apr_pool_create (&pool, icl_global_pool));
apr_pool_tag    (pool, "ipr_net_validate addr");
rv = apr_parse_addr_port (&addr, &scope_id, &port, netaddr, pool);

//  Addresses with scope are considered invalid ie: [1.2.3.5%eth0]
if (rv == APR_SUCCESS && scope_id == NULL && addr != NULL) {
    rv = apr_sockaddr_info_get (&sa, addr, APR_UNSPEC, port, APR_IPV4_ADDR_OK, pool);
    rc = (rv == APR_SUCCESS);
}
else
    rc = FALSE;

apr_pool_destroy (pool);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_net_ping

    Type: Component method
    Checks whether a remote server is online or not.  This method is
    synchronous and can take some time to complete.  The hostname is
    an IP address or DNS name, optionally followed by : and a port
    number.  If no port is specified, uses the default port.
    -------------------------------------------------------------------------
 */

Bool
    ipr_net_ping (
    char * hostname,                    //  Host and port to test
    char * defport                      //  Default port
)
{
icl_shortstr_t
    workname;
char
    *portspec;
dbyte
    port = 0;
apr_pool_t
    *pool;
apr_socket_t
    *sock;
apr_sockaddr_t
    *sockaddr;
    Bool
        rc;                             //  True if server is accessible

//
icl_shortstr_cpy (workname, hostname);
portspec = strchr (workname, ':');
if (portspec) {
    *portspec = 0;
    port = atoi (portspec + 1);
}
else
if (defport)
    port = atoi (defport);
else
    icl_console_print ("E: no port specified in ping attempt to '%s'", hostname);

//  Will this invoke (possibly very slow) name resolution?
icl_apr_assert (apr_pool_create (&pool, icl_global_pool));
apr_pool_tag (pool, "ipr_net_ping");
if (apr_sockaddr_info_get (
    &sockaddr, workname, APR_UNSPEC, port, 0, pool) != APR_SUCCESS)
    rc = FALSE;
else
if (apr_socket_create (
    &sock, sockaddr->family, SOCK_STREAM, 0, pool) != APR_SUCCESS)
    rc = FALSE;
else {
    //  Use a short timeout and try to connect to server
    apr_socket_timeout_set (sock, apr_time_from_sec (15));
    rc = (apr_socket_connect (sock, sockaddr) == 0);
    apr_socket_close (sock);
}
apr_pool_destroy (pool);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_net_ping_list

    Type: Component method
    Checks a list of servers and returns the first server that is
    active (which accepts a socket connection).  Each server name
    is formatted as an IP address or DNS name optional followed by
    a colon and port number.  If the cleanup option is true, all
    inaccessible servers are removed from the list, up to the point
    where a successful connection is made.
    -------------------------------------------------------------------------
 */

ipr_token_t *
    ipr_net_ping_list (
    ipr_token_list_t * servers,         //  List of servers
    char * defport,                     //  Default port
    Bool cleanup                        //  Remove failed servers?
)
{
    ipr_token_t *
        token;                          //  Entry in token list

//
token = ipr_token_list_first (servers);
while (token) {
    if (ipr_net_ping (token->value, defport))
        break;                      //  We've found an accessible host
    else {
        if (cleanup) {
            ipr_token_destroy (&token);
            token = ipr_token_list_first (servers);
        }
        else
            token = ipr_token_list_next (&token);
    }
}

    return (token);
}
/*  -------------------------------------------------------------------------
    ipr_net_lookup_host

    Type: Component method
    Accepts a dotted IP address and returns a hostname, if resolvable.
    Note that this function is synchronous and can take some time to
    resolve distant addresses.
    -------------------------------------------------------------------------
 */

char *
    ipr_net_lookup_host (
    char * hostname,                    //  Default port
    char * address                      //  Dotted IP address
)
{
struct sockaddr_in
    sin;
struct hostent
    *he;

sin.sin_addr.s_addr = inet_addr (address);
he = gethostbyaddr ((void *) &sin.sin_addr.s_addr, sizeof (sin.sin_addr.s_addr), AF_INET);
if (he)
    icl_shortstr_cpy (hostname, he->h_name);
else
    icl_shortstr_cpy (hostname, address);

    return (hostname);
}
/*  -------------------------------------------------------------------------
    ipr_net_fmt_display

    Type: Component method
    Formats an IP address for display, using the format 'external (internal)'
    where the external address is either the translated DNS name or the
    internal address stripped of any port information.  The internal address
    is a dotted number with optional :port suffix.  The opt_xlat argument
    can be set to FALSE to disable (possibly slow) address lookups.
    -------------------------------------------------------------------------
 */

char *
    ipr_net_fmt_display (
    char * external,                    //  Fully-formatted string
    char * internal,                    //  Dotted IP address
    Bool opt_xlat                       //  Do address lookups?
)
{
icl_shortstr_t
    host_name,
    dotted_addr;

//
//  If IP address is not valid, don't try to resolve it
if (ipr_regexp_eq ("^[0-9]+`.[0-9]+`.[0-9]+`.[0-9]+", internal)) {
    //  Get dotted IP address and truncate port specifier if present
    icl_shortstr_cpy (dotted_addr, internal);
    if (strchr (dotted_addr, ':'))
        *strchr (dotted_addr, ':') = 0;

    if (opt_xlat)
        icl_shortstr_cpy (host_name, dotted_addr);
    else
        ipr_net_lookup_host (host_name, dotted_addr);

    if (strneq (dotted_addr, host_name))
        icl_shortstr_fmt (external, "%s (%s)", host_name, internal);
    else
        icl_shortstr_fmt (external, internal);
}
else 
    icl_shortstr_cpy (external, internal);

    return (external);
}
/*  -------------------------------------------------------------------------
    ipr_net_cidr_eq

    Type: Component method
    Checks if an address falls within a CIDR range like 192.160.55.0/24.  
    See http://en.wikipedia.org/wiki/Classless_Inter-Domain_Routing.
    Returns TRUE or FALSE.
    -------------------------------------------------------------------------
 */

Bool
    ipr_net_cidr_eq (
    char * address_str,                 //  Dotted IP address
    char * range_str                    //  Dotted CIDR specification
)
{
qbyte
    address = 0,
    range = 0;
uint
    part1, part2, part3, part4,     //  Four parts of dotted IP address
    bits;                           //  Number of significant bits
    Bool
        rc = FALSE;                     //  Not documented

//
//  Get address and range as 32-bit values, do not validate
if (sscanf (range_str, "%u.%u.%u.%u/%u", 
    &part1, &part2, &part3, &part4, &bits) == 5) {
    range = ((part1 << 24) + (part2 << 16) + (part3 << 8) + part4) 
        >> (32 - bits);
    if (sscanf (address_str, "%u.%u.%u.%u", 
        &part1, &part2, &part3, &part4) == 4) {
        address = ((part1 << 24) + (part2 << 16) + (part3 << 8) + part4) 
            >> (32 - bits);
        if (address == range)
            rc = TRUE;
    }
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_net_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_net_selftest (
void)
{

icl_shortstr_t
    external;

assert (ipr_net_ping ("localhost:54321", "") == FALSE);
assert (ipr_net_validate_addr ("localhost") == TRUE);
assert (ipr_net_validate_addr ("localhost:54321") == TRUE);
assert (ipr_net_validate_addr ("10.0.0.1") == TRUE);
assert (ipr_net_validate_addr ("10.0.0.1:80") == TRUE);
assert (ipr_net_validate_addr ("address:port") == FALSE);
assert (ipr_net_validate_addr ("localhost:123456789") == FALSE);
assert (ipr_net_validate_addr (":80") == FALSE);
assert (ipr_net_validate_addr ("") == FALSE);

ipr_net_fmt_display (external, "", TRUE);
assert (streq (external, ""));
ipr_net_fmt_display (external, "0", TRUE);
assert (streq (external, "0"));
ipr_net_fmt_display (external, "invalid", TRUE);
assert (streq (external, "invalid"));
ipr_net_fmt_display (external, "127.0.0.1", TRUE);

assert (ipr_net_cidr_eq ("128.0.0.1", "128.0.0.1/0"));
assert (ipr_net_cidr_eq ("128.0.0.1", "128.0.0.1/32"));
assert (ipr_net_cidr_eq ("128.0.0.1", "128.0.1.1/16"));
assert (!ipr_net_cidr_eq ("128.0.0.1", "128.1.0.1/16"));
}
/*  -------------------------------------------------------------------------
    ipr_net_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_net_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_net_animating = enabled;
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_net_version_start        = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_net_component            = "ipr_net ";
char *EMBED__ipr_net_version              = "1.0 ";
char *EMBED__ipr_net_copyright            = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_net_filename             = "ipr_net.icl ";
char *EMBED__ipr_net_builddate            = "2011/03/01 ";
char *EMBED__ipr_net_version_end          = "VeRsIoNeNd:ipc";

