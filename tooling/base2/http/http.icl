<?xml?>
<!--
    Copyright (c) 2007 iMatix Corporation

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
    name      = "http"
    comment   = "Generates http include file"
    version   = "1.0"
    script    = "icl_gen"
    import    = "1"
    >

<option name = "selftest" value = "0" />

<import class = "http_access_context" />
<import class = "http_access_dos" />
<import class = "http_access_logger" />
<import class = "http_access_module" />
<import class = "http_access_policy" />
<import class = "http_access_refresh" />
<import class = "http_address" />
<import class = "http_agent" />
<import class = "http_config" />
<import class = "http_driver_cgi" />
<import class = "http_driver_context" />
<import class = "http_driver_file" />
<import class = "http_driver_module" />
<import class = "http_driver_null" />
<import class = "http_request" />
<import class = "http_static" />
<import class = "http_response" />
<import class = "http_server" />

</class>
