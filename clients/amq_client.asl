<?xml version="1.0"?>
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

<protocol
    comment = "OpenAMQ server protocol methods"
    script  = "asl_gen"
    chassis = "client"
    >
<inherit name = "amq" />
<inherit name = "asl_client" />
<option name = "product_name" value = "OpenAMQ Kernel Client" />
<option name = "syncapi"      value = "1" />

<class name = "channel">
  <context>
    amq_sequence_table_t
        *sequences;                     //  Timestamp per connection id
  </context>
  <constructor>
    self->sequences = amq_sequence_table_new ();
  </constructor>
  <destructor>
    amq_sequence_table_destroy (&self->sequences);
  </destructor>
</class>

<class name = "basic">
  <action name = "get-ok">
    if (amq_client_config_sequence_check (amq_client_config)) {
        if (amq_sequence_check (session->sequences, content->sender_id, content->timestamp))
            icl_console_print ("W: message out-of-sequence, routing-key='%s' exchange='%s'",
                content->routing_key, content->exchange);
    }
    amq_client_session_push_arrived (
        session,
        content,
        method->exchange,
        method->routing_key,
        NULL,                           //  No consumer tag for Get-Ok
        method->delivery_tag);
  </action>

  <action name = "deliver">
    if (amq_client_config_sequence_check (amq_client_config)) {
        if (amq_sequence_check (session->sequences, content->sender_id, content->timestamp))
            icl_console_print ("W: message out-of-sequence, routing-key='%s' exchange='%s'",
                content->routing_key, content->exchange);
    }
    amq_client_session_push_arrived (
        session,
        content,
        method->exchange,
        method->routing_key,
        method->consumer_tag,
        method->delivery_tag);
  </action>

  <action name = "return">
    amq_client_session_push_returned (
        session, content, method->exchange, method->routing_key, NULL);
  </action>

  <action name = "publish">
    <local>
    smt_thread_t
        *thread;
    byte
        options;                        //  Encoded Direct Mode options
    </local>
    //
    if (amq_client_config_sequence_set (amq_client_config)) {
        amq_content_basic_set_timestamp (content, apr_time_now ());
        amq_content_basic_set_sender_id (content, self->connection->id);
    }
    if (amq_client_config_chrono_enabled (amq_client_config)
    &&  amq_client_config_chrono_density (amq_client_config) > randomof (1000))
        amq_content_basic_chrono_set (content);

    if (self->connection->direct) {
        if (exchange == NULL)
            exchange = "";                     //  Default exchange
        thread = amq_client_session_dp_lookup (self, exchange, DP_SINK);
        if (!thread) {
            thread = amq_client_session_dp_new (self, exchange, DP_SINK);
            assert (thread);
        }
        //  Options octet is [0][0][0][0][0][0][mandatory][immediate]
        options = ((mandatory & 1) << 1) | (immediate & 1);
        amq_content_basic_set_routing_key (content, exchange, routing_key, NULL);
        amq_client_agent_direct_out (thread, content, options);
        smt_thread_unlink (&thread);
    }
    else
  </action>
</class>

<class name = "queue">
  <action name = "declare" when = "after">
    <local>
    smt_thread_t
        *thread;
    </local>
    if (self->connection->direct && exclusive) {
        thread = amq_client_session_dp_lookup (self, self->queue, DP_FEED);
        if (!thread) {
            thread = amq_client_session_dp_new (self, self->queue, DP_FEED);
            assert (thread);
        }
        smt_thread_unlink (&thread);
    }
  </action>
  <action name = "delete" when = "after">
    if (self->connection->direct)
        amq_client_session_dp_destroy (self, queue, DP_FEED);
  </action>
</class>

</protocol>
