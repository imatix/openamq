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
    name    = "smt_demo_portal_client"
    comment = "Demonstration of portal system - client class"
    script  = "smt_object_gen"
    target  = "smt"
    >

<inherit class = "smt_demo_portal_front" />

<import class = "smt_demo_portal_server" />

<method name = "mogrify">
    <action>
    icl_console_print ("CLIENT MOGRIFY: %s", string);
    </action>
</method>

<method name = "test 1" template = "function">
    <local>
    smt_demo_portal_t
        *portal;
    smt_demo_portal_server_t
        *server;
    </local>
    //
    //  Create an explicit server object and talk to it via a portal
    server = smt_demo_portal_server_new ();
    portal = smt_demo_portal_server__smt_demo_portal_new (server, "A");
    smt_demo_portal_client__smt_demo_portal_bind (self, portal);
    smt_demo_portal_request_print (portal, "Test explicit server");
    apr_sleep (100000);
    smt_demo_portal_unlink (&portal);
    smt_demo_portal_server_unlink (&server);
</method>

<method name = "test 2" template = "function">
    <local>
    smt_demo_portal_t
        *portal;
    </local>
    //
    //  Create an implicit server object and talk to it via a portal
    portal = smt_demo_portal_server__smt_demo_portal_new (NULL, "A");
    smt_demo_portal_client__smt_demo_portal_bind (self, portal);
    smt_demo_portal_request_print (portal, "Test implicit server");
    apr_sleep (100000);
    smt_demo_portal_unlink (&portal);
</method>

<method name = "test 3" template = "function">
    <local>
    smt_demo_portal_t
        *factory,
        *portal;
    </local>
    //
    //  Use a portal factory to create a new server portal
    //  First, create a factory and bind it to ourselves
    factory = smt_demo_portal_server__smt_demo_portal_factory ("C");
    smt_demo_portal_client__smt_demo_portal_bind (self, factory);

    //  Next, invoke the factory method to get a new server and portal
    portal = smt_demo_portal_factory (factory, "D");
    smt_demo_portal_client__smt_demo_portal_bind (self, portal);

    //  Use the server portal
    smt_demo_portal_request_print (portal, "test step 3");

    //  Let SMT run a little
    apr_sleep (100000);

    //  Shut-down the portals
    smt_demo_portal_unlink (&portal);
    smt_demo_portal_unlink (&factory);
</method>

<method name = "selftest">
    <local>
    smt_demo_portal_client_t
        *client;
    </local>
    //
    client = smt_demo_portal_client_new ();
    smt_demo_portal_client_test_1 (client);
    smt_wait (0);
    smt_demo_portal_client_unlink (&client);
    smt_wait (0);
</method>

</class>
