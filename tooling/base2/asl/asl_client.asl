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
    comment = "ASL standard client actions"
    abstract = "1"
    >

<class name = "connection">
  <context>
    icl_shortstr_t
        server_host;                    //  Reported by server
    icl_shortstr_t
        server_port;                    //  Just the port number
    icl_shortstr_t
        server_product;
    icl_shortstr_t
        server_version;
    icl_shortstr_t
        server_platform;
    icl_shortstr_t
        server_copyright;
    icl_shortstr_t
        server_information;
    icl_shortstr_t
        server_instance;
    icl_shortstr_t
        server_identifier;
    icl_shortstr_t
        id;                             //  Server-assigned ID
    Bool
        server_direct;                  //  Server proposes Direct Mode?
  </context>

  <action name = "start">
    <local>
    asl_field_list_t
        *fields;                        //  Decoded responses
    char
        *port;                          //  Port number from host string
    </local>
    //
    fields = asl_field_list_new (method->server_properties);
    if (fields) {
        asl_field_list_cpy (fields, connection->server_host,        "host");
        asl_field_list_cpy (fields, connection->server_instance,    "instance");
        asl_field_list_cpy (fields, connection->server_product,     "product");
        asl_field_list_cpy (fields, connection->server_version,     "version");
        asl_field_list_cpy (fields, connection->server_platform,    "platform");
        asl_field_list_cpy (fields, connection->server_copyright,   "copyright");
        asl_field_list_cpy (fields, connection->server_information, "information");
        asl_field_list_cpy (fields, connection->server_identifier,  "identifier");
        asl_field_list_cpy (fields, connection->id,                 "connection");
        connection->server_direct = asl_field_list_integer (fields, "direct");

        port = strchr (connection->server_host, ':');
        if (port)
            icl_shortstr_cpy (connection->server_port, port + 1);

        asl_field_list_destroy (&fields);
    }
  </action>

</class>

</protocol>
