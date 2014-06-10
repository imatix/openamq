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
    name    = "ipr_demo_portal_client"
    comment = "Demonstration of portal system - client class"
    script  = "icl_gen"
    >

<inherit class = "ipr_demo_portal_front" />

<import class = "ipr_demo_portal_server" />

<context>
    ipr_demo_portal_t
        *portal;                        //  Client's portal
</context>

<method name = "destroy">
    ipr_demo_portal_destroy (&self->portal);
</method>

<method name = "mogrify">
    icl_console_print ("CLIENT MOGRIFY: %s", string);
</method>

<method name = "selftest">
    <local>
    ipr_demo_portal_client_t
        *client;
    ipr_demo_portal_server_t
        *server;
    ipr_demo_portal_t
        *portal,
        *factory;
    ipr_looseref_t
        *looseref;                      //  Portal reference
    </local>
    //
    //  Create an explicit server object and talk to it via a portal
    client = ipr_demo_portal_client_new ();
    server = ipr_demo_portal_server_new ();
    portal = ipr_demo_portal_server__ipr_demo_portal_new (server, "A");
    ipr_demo_portal_client__ipr_demo_portal_bind (client, portal);
    ipr_demo_portal_request_print (portal, "Test explicit server");
    ipr_demo_portal_server_destroy (&server);
    ipr_demo_portal_client_destroy (&client);
    ipr_demo_portal_destroy (&portal);

    //
    //  Create an implicit server object and talk to it via a portal
    client = ipr_demo_portal_client_new ();
    portal = ipr_demo_portal_server__ipr_demo_portal_new (NULL, "B");
    ipr_demo_portal_client__ipr_demo_portal_bind (client, portal);
    assert (ipr_demo_portal_request_print (portal, "Test implicit server") == 666);
    ipr_demo_portal_destroy (&portal);
    ipr_demo_portal_client_destroy (&client);

    //
    //  Implicit test but destroy server & portal via client
    client = ipr_demo_portal_client_new ();
    portal = ipr_demo_portal_server__ipr_demo_portal_new (NULL, "C");
    ipr_demo_portal_client__ipr_demo_portal_bind (client, portal);
    ipr_demo_portal_request_print (portal, "Test indirect destroy");
    ipr_demo_portal_unlink (&portal);
    ipr_demo_portal_client_destroy (&client);

    //
    //  Test portal lookup via client looseref list
    client = ipr_demo_portal_client_new ();
    portal = ipr_demo_portal_server__ipr_demo_portal_new (NULL, "D");
    ipr_demo_portal_client__ipr_demo_portal_bind (client, portal);
    ipr_demo_portal_unlink (&portal);
    //  This can be done in a different part of the app
    looseref = ipr_looseref_list_first (client->ipr_demo_portal_list);
    assert (looseref);
    portal = (ipr_demo_portal_t *) (looseref->object);
    assert (streq (portal->name, "D"));
    ipr_demo_portal_request_print (portal, "Test portal lookup");
    ipr_demo_portal_client_destroy (&client);

    //  Use portal property in client object
    client = ipr_demo_portal_client_new ();
    client->portal = ipr_demo_portal_server__ipr_demo_portal_new (NULL, "D");
    ipr_demo_portal_client__ipr_demo_portal_bind (client, client->portal);
    ipr_demo_portal_request_print (portal, "Using portal property");
    ipr_demo_portal_destroy (&client->portal);
    ipr_demo_portal_client_destroy (&client);
    
    //  Use a portal factory to create a new server portal
    //   - First, create a factory and bind it to ourselves
    client = ipr_demo_portal_client_new ();
    factory = ipr_demo_portal_server__ipr_demo_portal_factory ("C");
    ipr_demo_portal_client__ipr_demo_portal_bind (client, factory);
    //   - Next, invoke the factory method to get a new server and portal
    portal = ipr_demo_portal_factory (factory, "D");
    ipr_demo_portal_client__ipr_demo_portal_bind (client, portal);
    //   - Use the server portal
    ipr_demo_portal_request_print (portal, "portal factory test");
    //   - Shut-down the portals
    ipr_demo_portal_destroy (&portal);
    ipr_demo_portal_destroy (&factory);
    ipr_demo_portal_client_destroy (&client);
</method>

</class>
