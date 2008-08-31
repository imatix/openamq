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

<class name = "basic">
  <action name = "get-ok">
  <local>
    int
        msg_count;
  </local>
    //
    amq_content_basic_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    amq_content_basic_list_queue (session->arrived_basic_list, self->content);
    msg_count = amq_content_basic_list_count (session->arrived_basic_list);

    if (amq_client_config_arrived_high_water (amq_client_config) 
    && msg_count == amq_client_config_arrived_high_water (amq_client_config) 
    && icl_atomic_cas32 (&session->flow_stopped, TRUE, FALSE) == FALSE)
        amq_client_session_channel_flow (session, FALSE);
  </action>

  <action name = "deliver">
  <local>
    int
        msg_count;
  </local>
    //
    amq_content_basic_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    amq_content_basic_list_queue (session->arrived_basic_list, self->content);
    msg_count = amq_content_basic_list_count (session->arrived_basic_list);
    
    if (amq_client_config_arrived_high_water (amq_client_config) 
    && msg_count == amq_client_config_arrived_high_water (amq_client_config) 
    && icl_atomic_cas32 (&session->flow_stopped, TRUE, FALSE) == FALSE)
        amq_client_session_channel_flow (session, FALSE);
  </action>

  <action name = "return">
    amq_content_basic_set_routing_key (
        self->content, method->exchange, method->routing_key, 0);
    amq_content_basic_list_queue (session->returned_basic_list, self->content);
    ((amq_content_basic_t *) self->content)->returned = TRUE;

    if (!session->silent)
        icl_console_print ("W: basic message was returned: %d - %s",
            session->reply_code, session->reply_text);
  </action>

  <action name = "publish">
    <local>
    smt_thread_t
        *thread;
    </local>
    if (self->connection->direct) {
        thread = amq_client_session_dp_lookup (self, exchange, DP_SINK);
        if (!thread)
            thread = amq_client_session_dp_new (self, exchange, DP_SINK);
        if (thread) {
            amq_content_basic_set_routing_key (content, exchange, routing_key, NULL);
            amq_client_agent_direct_out (thread, content);
        }
        else
            icl_console_print ("E: cannot create new direct protocol");
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
        if (!thread)
            thread = amq_client_session_dp_new (self, self->queue, DP_FEED);
    }
  </action>
  <action name = "delete" when = "after">
    if (self->connection->direct)
        amq_client_session_dp_destroy (self, queue, DP_FEED);
  </action>
</class>

</protocol>
