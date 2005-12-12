<?xml?>
<class
    name      = "amq_exchange_system"
    comment   = "system exchange class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements these system services (specified by the routing
key):

  - amq.console - AMQ Console service
</doc>

<inherit class = "amq_exchange_base" />

<method name = "compile">
    icl_console_print ("E: cannot bind queue to the system exchange");
</method>

<method name = "publish">
    //
    //  Examine routing key, and use that to pass content to internal
    //  service.  Since we have just one service (amq.console), the
    //  lookup is hard-coded, but in future we may use the compile
    //  method to allow arbitrary system services to register.
    //    
    if (class_id == AMQ_SERVER_BASIC) {
        if (streq (routing_key, "amq.console")) {
            amq_console_publish (amq_console, content);
            delivered = TRUE;
        }
        else
            icl_console_print ("E: unknown system routing key '%s' rejected", routing_key);
    }
    else
        icl_console_print ("E: system exchange only accepts Basic contents");
</method>

</class>
