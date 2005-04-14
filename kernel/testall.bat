@echo off
if not %1.==. goto testone
    call testall amq_vhost_table
:-    call testall amq_vhost
    call testall amq_connection
    call testall amq_channel
    call testall amq_handle
    call testall amq_consumer
    call testall amq_message
    call testall amq_message_db
    call testall amq_bucket
    goto exit

:testone
    echo Testing %1...
    %1_test

:exit

