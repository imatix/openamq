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
    name      = "amq_exchange_system"
    comment   = "system exchange class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
This class implements these system services (specified by the routing key):

  - amq.console - AMQ Console service
</doc>

<inherit class = "amq_exchange_base" />

<method name = "compile">
    smt_log_print (amq_broker->alert_log, "E: cannot bind queue to the system exchange");
</method>

<method name = "publish">
    //
    //  Examine routing key, and use that to pass content to internal
    //  service.  Since we have just one service (amq.console), the
    //  lookup is hard-coded, but in future we may use the compile
    //  method to allow arbitrary system services to register.
    //
    if (streq (routing_key, "amq.console")) {
        amq_console_accept (amq_console, content, group);
        delivered++;
    }
    else
        smt_log_print (amq_broker->alert_log,
            "E: unknown system routing key '%s' rejected", routing_key);
</method>

</class>
