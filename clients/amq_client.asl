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
    icl_shortstr_cpy (content->exchange,    method->exchange);
    icl_shortstr_cpy (content->routing_key, method->routing_key);

    amq_content_basic_list_queue (session->arrived_basic_list, content);
    msg_count = amq_content_basic_list_count (session->arrived_basic_list);

    if (amq_client_config_high_water (amq_client_config)
    && msg_count == amq_client_config_high_water (amq_client_config)
    && icl_atomic_cas32 (&session->flow_stopped, TRUE, FALSE) == FALSE)
        amq_client_session_channel_flow (session, FALSE);
  </action>

  <action name = "deliver">
  <local>
    int
        msg_count;
  </local>
    //
    icl_shortstr_cpy (content->exchange,     method->exchange);
    icl_shortstr_cpy (content->routing_key,  method->routing_key);
    icl_shortstr_cpy (content->consumer_tag, method->consumer_tag);

    amq_content_basic_list_queue (session->arrived_basic_list, content);
    msg_count = amq_content_basic_list_count (session->arrived_basic_list);

    if (amq_client_config_high_water (amq_client_config)
    && msg_count == amq_client_config_high_water (amq_client_config)
    && icl_atomic_cas32 (&session->flow_stopped, TRUE, FALSE) == FALSE)
        amq_client_session_channel_flow (session, FALSE);
  </action>

  <action name = "return">
    icl_shortstr_cpy (content->exchange,    method->exchange);
    icl_shortstr_cpy (content->routing_key, method->routing_key);

    amq_content_basic_list_queue (session->returned_basic_list, content);
    content->returned = TRUE;

    if (!session->silent)
        icl_console_print ("W: basic message was returned: %d - %s",
            session->reply_code, session->reply_text);
  </action>
</class>

</protocol>
