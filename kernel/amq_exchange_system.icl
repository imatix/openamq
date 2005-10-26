<?xml?>
<class
    name      = "amq_exchange_system"
    comment   = "system exchange class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements system functions:

    - AMQ Console (console exchange).

System messages are not routed to queues but are processed according
to the exchange name and in some cases, the routing key.
</doc>

<public>
//  This ID is used to number all objects that are exposed via the
//  Console.
//
extern qbyte
    amq_object_id;                      //  Global object ID
</public>

<private>
qbyte
    amq_object_id = 0;
</private>

<inherit class = "amq_exchange_base" />

<method name = "compile">
    icl_console_print ("E: cannot bind queue to a system exchange");
</method>

<method name = "publish">
    <local>
    asl_reader_t
        reader;                         //  Body reader
    ipr_bucket_t
        *bucket;
    </local>

    
    amq_content_basic_set_reader (content, &reader, 32000);
    bucket = amq_content_basic_replay_body (content, &reader);

    icl_console_print ("Data: %s", bucket->data);
    ipr_bucket_destroy (&bucket);
    delivered = TRUE;
</method>

</class>
