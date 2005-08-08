<?xml?>
<class
    name      = "amq_exchange_system"
    comment   = "system exchange class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements the system exchange.  System messages are not
routed but are processed according to destination.
</doc>

<inherit class = "amq_exchange_base" />

<method name = "compile">
</method>

<method name = "publish">
    <local>
    ipr_bucket_t
        *bucket;
    </local>
    //
    bucket = amq_content_basic_replay_body (content);
    icl_console_print ("System destination: %s", destination);
    icl_console_print ("Data: %s", bucket->data);
    ipr_bucket_destroy (&bucket);
    delivered = TRUE;
</method>

</class>
