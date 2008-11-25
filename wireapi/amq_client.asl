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
    amq_client_session_push_arrived (
        session, content, method->exchange, method->routing_key, NULL);
  </action>

  <action name = "deliver">
    amq_client_session_push_arrived (
        session, content, method->exchange, method->routing_key, method->consumer_tag);
  </action>

  <action name = "return">
    amq_client_session_push_returned (
        session, content, method->exchange, method->routing_key, NULL);
  </action>

  <action name = "publish">
    <local>
    smt_thread_t
        *thread;
    </local>
    //
    if (amq_client_config_chrono_enabled (amq_client_config)
    &&  amq_client_config_chrono_density (amq_client_config) > randomof (1000))
        amq_content_basic_chrono_set (content);

    if (self->connection->direct) {
        if (exchange == NULL)
            exchange = "";                     //  Default exchange
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
