<?xml?>
<!--
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
 -->
<class
    name      = "ipr_net"
    comment   = "Network module"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    The network module provides various higher-level network access
    functions built on the APR socket layer.
</doc>
<inherit class = "icl_base" />
<import class = "ipr_token" />

<method name = "get addr">
    <doc>
    Returns the first network address of the current machine.  Copies
    the network address into a short string provided by the caller.
    </doc>
    <argument name = "dest" type = "char *">Network address</argument>
    <local>
    icl_shortstr_t
        buffer;                         //  Holds host name
    apr_pool_t
        *pool;                          //  APR working pool
    apr_sockaddr_t
        *sockaddr;                      //  The associated socket addresses
    char
        *addr;                          //  Formatted IP address
    </local>
    //
    icl_apr_assert (apr_pool_create (&pool, icl_global_pool));
    apr_pool_tag    (pool, "$(selfname)_$(method.name)");
    apr_gethostname (buffer, ICL_SHORTSTR_MAX, pool);
    apr_sockaddr_info_get (&sockaddr, buffer, APR_UNSPEC, 0, 0, pool);
    if (sockaddr) {
        apr_sockaddr_ip_get (&addr, sockaddr);
        icl_shortstr_cpy (dest, addr);
    }
    else
        icl_shortstr_cpy (dest, "127.0.0.1");
    apr_pool_destroy (pool);
</method>

<method name = "get hostname">
    <doc>
    Returns a string containing the local host name. Copies the host
    name into a short string provided by the caller.
    </doc>
    <argument name = "dest" type = "char *">Network address</argument>
    <local>
    apr_pool_t
        *pool;                          //  APR working pool
    </local>
    //
    icl_apr_assert (apr_pool_create (&pool, icl_global_pool));
    apr_pool_tag    (pool, "$(selfname)_$(method.name)");
    apr_gethostname (dest, ICL_SHORTSTR_MAX, pool);
    apr_pool_destroy (pool);
</method>

<method name = "validate addr" return = "rc">
    <doc>
    Validates a network address of the form host:port. Port is optional. 
    Host is obligatory.
    </doc>
    <argument name = "netaddr" type = "char *">Network address to validate</argument>
    <declare name = "rc" type = "Bool">True if network address is valid</declare>
    <local>
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
    </local>
    //
    icl_apr_assert (apr_pool_create (&pool, icl_global_pool));
    apr_pool_tag    (pool, "$(selfname)_$(method.name)");
    rv = apr_parse_addr_port (&addr, &scope_id, &port, netaddr, pool);

    //  Addresses with scope are considered invalid ie: [1.2.3.5%eth0]
    if (rv == APR_SUCCESS && scope_id == NULL && addr != NULL) {
        rv = apr_sockaddr_info_get (&sa, addr, APR_UNSPEC, port, APR_IPV4_ADDR_OK, pool);
        rc = (rv == APR_SUCCESS);
    }
    else
        rc = FALSE;

    apr_pool_destroy (pool);
</method>

<method name = "ping" return = "rc">
    <doc>
    Checks whether a remote server is online or not.  This method is
    synchronous and can take some time to complete.  The hostname is
    an IP address or DNS name, optionally followed by : and a port
    number.  If no port is specified, uses the default port.
    </doc>
    <argument name = "hostname" type = "char *">Host and port to test</argument>
    <argument name = "defport" type = "char *">Default port</argument>
    <declare name = "rc" type = "Bool">True if server is accessible</declare>
    <local>
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
    </local>
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
    apr_pool_tag (pool, "$(selfname)_$(method.name)");
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
</method>

<method name = "ping list" return = "token">
    <doc>
    Checks a list of servers and returns the first server that is
    active (which accepts a socket connection).  Each server name
    is formatted as an IP address or DNS name optional followed by
    a colon and port number.  If the cleanup option is true, all
    inaccessible servers are removed from the list, up to the point
    where a successful connection is made.
    </doc>
    <argument name = "servers" type = "ipr_token_list_t *">List of servers</argument>
    <argument name = "defport" type = "char *">Default port</argument>
    <argument name = "cleanup" type = "Bool">Remove failed servers?</argument>
    <declare name = "token" type = "ipr_token_t *">Entry in token list</declare>
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
</method>

<method name = "lookup host" return = "hostname">
    <doc>
    Accepts a dotted IP address and returns a hostname, if resolvable.
    Note that this function is synchronous and can take some time to
    resolve distant addresses.
    </doc>
    <argument name = "hostname" type = "char *">Default port</argument>
    <argument name = "address"  type = "char *">Dotted IP address</argument>
    <local>
    struct sockaddr_in
        sin;
    struct hostent
        *he;
    </local>
    sin.sin_addr.s_addr = inet_addr (address);
    he = gethostbyaddr ((void *) &sin.sin_addr.s_addr, sizeof (sin.sin_addr.s_addr), AF_INET);
    if (he)
        icl_shortstr_cpy (hostname, he->h_name);
    else
        icl_shortstr_cpy (hostname, address);
</method>

<method name = "fmt display" return = "external">
    <doc>
    Formats an IP address for display, using the format 'external (internal)'
    where the external address is either the translated DNS name or the
    internal address stripped of any port information.  The internal address
    is a dotted number with optional :port suffix.  The opt_xlat argument
    can be set to FALSE to disable (possibly slow) address lookups.
    </doc>
    <argument name = "external" type = "char *">Fully-formatted string</argument>
    <argument name = "internal" type = "char *">Dotted IP address</argument>
    <argument name = "opt_xlat" type = "Bool">Do address lookups?</argument>
    <local>
    icl_shortstr_t
        host_name,
        dotted_addr;
    </local>
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
</method>

<method name = "cidr eq" return = "rc">
    <doc>
    Checks if an address falls within a CIDR range like 192.160.55.0/24.  
    See http://en.wikipedia.org/wiki/Classless_Inter-Domain_Routing.
    Returns TRUE or FALSE.
    </doc>
    <argument name = "address_str" type = "char *">Dotted IP address</argument>
    <argument name = "range_str" type = "char *">Dotted CIDR specification</argument>
    <declare name = "rc" type = "Bool" default = "FALSE" />
    <local>
    qbyte
        address = 0,
        range = 0;
    uint
        part1, part2, part3, part4,     //  Four parts of dotted IP address
        bits;                           //  Number of significant bits
    </local>
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
</method>

<method name = "selftest">
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
</method>

</class>


