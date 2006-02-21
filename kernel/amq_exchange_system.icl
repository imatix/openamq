<?xml?>
<class
    name      = "amq_exchange_system"
    comment   = "system exchange class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
This class implements these system services (specified by the routing
key):

  - amq.console - AMQ Console service
  - amq.cluster - AMQ Cluster service
</doc>

<inherit class = "amq_exchange_base" />

<method name = "compile">
    asl_log_print (amq_broker->alert_log, "E: cannot bind queue to the system exchange");
</method>

<method name = "publish">
    //
    //  Examine routing key, and use that to pass content to internal
    //  service.  Since we have just one service (amq.console), the
    //  lookup is hard-coded, but in future we may use the compile
    //  method to allow arbitrary system services to register.
    //    
    if (method->class_id == AMQ_SERVER_BASIC) {
        if (streq (routing_key, "amq.console")) {
            amq_console_accept (amq_console, basic_content);
            delivered = TRUE;
        }
        else
            asl_log_print (amq_broker->alert_log,
                "E: unknown system routing key '%s' rejected", routing_key);
    }
    else
        asl_log_print (amq_broker->alert_log,
            "E: system exchange only accepts Basic contents");
</method>

</class>
