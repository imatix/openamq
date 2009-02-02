<?xml?>
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
<class
    name    = "zyre_backend"
    comment = "Zyre backend portal"
    script  = "icl_gen"
    >
<doc>
    This class enables the creation of backend modules that implement RestMS
    on a messaging server.  Modules are portal servers that inherit the
    zyre_backend_back class and implement the requests defined here.
    The client for a backend module implements the response methods, and must
    be an async class.
</doc>

<inherit class = "ipr_portal">
    <option name = "front_end" value = "async" />
    <option name = "back_end" value = "async" />
</inherit>

<import class = "zyre_classes" />

<data>
    <request name = "start">
      <doc>
        Start the peering. You must call this method to start the peering.
      </doc>
    </request>

    <request name = "stop">
      <doc>
        Stop the peering. If you want to restart the peering, use the start
        method.
      </doc>
    </request>

    <request name = "pipe create">
      <doc>
        Add a pipe to the set of defined pipes and create the pipe on the
        remote server.
      </doc>
      <field name = "pipe type" type = "char *" />
      <field name = "pipe name" type = "char *" />
      <possess>
        pipe_type = icl_mem_strdup (pipe_type);
        pipe_name = icl_mem_strdup (pipe_name);
      </possess>
      <release>
        icl_mem_free (pipe_type);
        icl_mem_free (pipe_name);
      </release>
    </request>

    <request name = "pipe delete">
      <doc>
        Remove a pipe from the set of defined pipes and delete the pipe on the
        remote server.
      </doc>
      <field name = "pipe name" type = "char *" />
      <possess>
        pipe_name = icl_mem_strdup (pipe_name);
      </possess>
      <release>
        icl_mem_free (pipe_name);
      </release>
    </request>

    <request name = "feed create">
      <doc>
        Add a feed to the set of defined feeds and create the feed on the
        remote server.
      </doc>
      <field name = "feed type" type = "char *" />
      <field name = "feed name" type = "char *" />
      <possess>
        feed_type = icl_mem_strdup (feed_type);
        feed_name = icl_mem_strdup (feed_name);
      </possess>
      <release>
        icl_mem_free (feed_type);
        icl_mem_free (feed_name);
      </release>
    </request>

    <request name = "feed delete">
      <doc>
        Remove a feed from the set of defined feeds and delete the feed
        on the remote server.
      </doc>
      <field name = "feed name" type = "char *" />
      <possess>
        feed_name = icl_mem_strdup (feed_name);
      </possess>
      <release>
        icl_mem_free (feed_name);
      </release>
    </request>

    <request name = "join create">
      <doc>
        Add a join to the set of defined joins and create the join on the remote
        server.  The pipe and feed must already exist.  Does not validate the
        pipe type, this must be done by the caller.
      </doc>
      <field name = "pipe name" type = "char *" />
      <field name = "feed name" type = "char *" />
      <field name = "address" type = "char *">Address to join</field>
      <possess>
        pipe_name = icl_mem_strdup (pipe_name);
        feed_name = icl_mem_strdup (feed_name);
        address = icl_mem_strdup (address);
      </possess>
      <release>
        icl_mem_free (pipe_name);
        icl_mem_free (feed_name);
        icl_mem_free (address);
      </release>
    </request>

    <request name = "join delete">
      <doc>
        Remove a join from the set of defined joins and delete all resources for
        the join on the remote server.  Neither the pipe nor feed need exist.
      </doc>
      <field name = "pipe name" type = "char *" />
      <field name = "feed name" type = "char *" />
      <field name = "address" type = "char *">Address to join</field>
      <possess>
        pipe_name = icl_mem_strdup (pipe_name);
        feed_name = icl_mem_strdup (feed_name);
        address = icl_mem_strdup (address);
      </possess>
      <release>
        icl_mem_free (pipe_name);
        icl_mem_free (feed_name);
        icl_mem_free (address);
      </release>
    </request>

    <request name = "address post">
      <doc>
        Publishes a message to the specified address@feed on the backend server.
        If the feed is an exchange, the address is used as the message routing
        key and the feed is the exchange for the publish request.  If the feed
        is a shared queue, the address is used as the message id and the default
        exchange is used as the exchange for the publish request.  Does not
        validate the feed type, this must be done by the caller.
      </doc>
      <field name = "address" type = "char *" />
      <field name = "feed name" type = "char *" />
      <field name = "content" type = "amq_content_basic_t *">Content to publish</field>
      <possess>
        address = icl_mem_strdup (address);
        feed_name = icl_mem_strdup (feed_name);
        content = amq_content_basic_link (content);
      </possess>
      <release>
        icl_mem_free (address);
        icl_mem_free (feed_name);
        amq_content_basic_unlink (&content);
      </release>
    </request>

    <request name = "forward">
      <doc>
        Forwards a message content to the remote peer.
      </doc>
      <field name = "exchange" type = "char *">Exchange for publish</field>
      <field name = "routing key" type = "char *">Routing key for publish</field>
      <field name = "content" type = "amq_content_basic_t *">Content to publish</field>
      <field name = "mandatory" type = "Bool">Mandatory routing</field>
      <field name = "immediate" type = "Bool">Immediate delivery</field>
      <possess>
        exchange = icl_mem_strdup (exchange);
        routing_key = icl_mem_strdup (routing_key);
        content = amq_content_basic_link (content);
      </possess>
      <release>
        icl_mem_free (exchange);
        icl_mem_free (routing_key);
        amq_content_basic_unlink (&content);
      </release>
    </request>

    <request name = "online partial">
      <doc>
        Signals a partial online to the back-end connection.  The meaning
        of this method depends on the messaging protocol.  For AMQP, it
        tells the back-end that the connection has been opened, and that it
        should now open a channel.
      </doc>
    </request>

    <request name = "online final">
      <doc>
        Signals to the back-end module that the back end connection is up
        and running.
      </doc>
    </request>

    <!-- Responses back to the client class -->

    <response name = "online">
      <doc>
        Signals to the client that the back end connection is up and running.
        Provides the client with the name of the backend's reply queue for
        reply messages.
      </doc>
      <field name = "reply queue" type = "char *" />
      <possess>
        reply_queue = icl_mem_strdup (reply_queue);
      </possess>
      <release>
        icl_mem_strfree (&reply_queue);
      </release>
    </response>

    <response name = "offline">
      <doc>
        Signals to the client that the back end connection has gone down due
        either to remote, or local disconnection.
      </doc>
    </response>

    <response name = "arrived">
      <doc>
        Delivers an arrived content to the client.  Provides the whole method
        so we can access both the content and its envelope.
      </doc>
      <field name = "method" type = "zyre_peer_method_t *" />
      <possess>
        method = zyre_peer_method_link (method);
      </possess>
      <release>
        zyre_peer_method_unlink (&method);
      </release>
    </response>

    <response name = "returned">
      <doc>
        Delivers a returned (failed) content to the client.
      </doc>
      <field name = "method" type = "zyre_peer_method_t *" />
      <possess>
        method = zyre_peer_method_link (method);
      </possess>
      <release>
        zyre_peer_method_unlink (&method);
      </release>
    </response>
</data>

</class>
