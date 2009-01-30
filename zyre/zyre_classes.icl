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
    name      = "zyre_classes"
    comment   = "Generates include files"
    version   = "1.0"
    script    = "icl_gen"
    import    = "1"
    >
<option name = "selftest" value = "0" />

<import class = "zyre_amqp_feed" />
<import class = "zyre_amqp_join" />
<import class = "zyre_amqp_pipe" />
<import class = "zyre_backend_amqp" />
<import class = "zyre_config" />
<import class = "zyre_content" />
<import class = "zyre_digest_amqp" />
<import class = "zyre_domain" />
<import class = "zyre_feed" />
<import class = "zyre_join" />
<import class = "zyre_message" />
<import class = "zyre_peer_agent" />
<import class = "zyre_peer_config" />
<import class = "zyre_pipe" />
<import class = "zyre_resource" />
<import class = "zyre_restms" />
<import class = "zyre_static" />

</class>
