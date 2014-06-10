<?xml version="1.0"?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

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
    comment = "ASL demo client"
    script  = "asl_gen"
    chassis = "client"
    >
<option name = "product_name" value = "ASL Demo Client" />
<option name = "syncapi"      value = "1" />
<inherit name = "demo" />
<inherit name = "asl_client" />

<!--
    Actions on requests are handled in session class, self = session
    If you need these to happen at response, put when = "after"
    Actions on responses are handled in method class, self = method
 -->

<class name = "basic">
  <action name = "publish">
    <local>
    smt_thread_t
        *thread;
    </local>
    if (self->connection->direct) {
        thread = demo_client_session_dp_lookup (self, exchange, DP_SINK);
        if (!thread) {
            thread = demo_client_session_dp_new (self, exchange, DP_SINK);
            assert (thread);
        }
        demo_content_basic_set_routing_key (content, exchange, routing_key, NULL);
        demo_client_agent_direct_out (thread, content, 0);
        smt_thread_unlink (&thread);
    }
    else
  </action>

  <action name = "deliver">
    demo_client_session_push_arrived (
        session, content, method->exchange, method->routing_key, NULL, 0);
  </action>

  <action name = "get-ok">
    demo_client_session_push_arrived (
        session, content, method->exchange, method->routing_key, NULL, 0);
  </action>
</class>

<class name = "queue">
  <action name = "declare" when = "after">
    <local>
    smt_thread_t
        *thread;
    </local>
    if (self->connection->direct && exclusive) {
        thread = demo_client_session_dp_lookup (self, queue, DP_FEED);
        if (!thread) {
            thread = demo_client_session_dp_new (self, queue, DP_FEED);
            assert (thread);
        }
        smt_thread_unlink (&thread);
    }
  </action>
  <action name = "delete" when = "after">
    if (self->connection->direct)
        demo_client_session_dp_destroy (self, queue, DP_FEED);
  </action>
</class>

</protocol>
