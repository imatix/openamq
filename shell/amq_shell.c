/*---------------------------------------------------------------------------
 *
 *  amq_shell.c - OpenAMQ Console Management Shell
 *
 *  Copyright (c) 2007 iMatix Corporation
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or (at
 *  your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  For information on alternative licensing for OEMs, please contact
 *  iMatix Corporation.
 *
 *  For documentation and updates see http://www.openamq.org/.
 *
 *---------------------------------------------------------------------------*/

#include "icl.h"
#include "amq_mgt_console.h"
#include "amq_mgt_broker.h"
#include "version.h"

#define NOWARRANTY \
"This is free software; see the source for copying conditions.  There is NO\n" \
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n" \
    "\n"

#define USAGE                                                                \
    "Syntax: amq_shell [options...]\n"                                       \
    "   Management Console for OpenAMQ Brokers\n"                            \
    "\n"                                                                     \
    "General options:\n"                                                     \
    "  -s hostname      Broker hostname and :port (localhost)\n"             \
    "  -V virtualhost   Specify cluster virtual host\n"                      \
    "  -u user          User name for console access (guest)\n"              \
    "  -p password      Password for console access (guest)\n"               \
    "  -e \"commands\"    Run shell commands, delimited by ;\n"              \
    "  -x filename      Save all status data as XML\n"                       \
    "  -t level         Set trace level (default = 0)\n"                     \
    "  -a               Show IP addresses, not host name\n"                  \
    "  -b               Show broker status and then exit\n"                  \
    "  -r               Report all active local brokers\n"                   \
    "  -q               Show all broker queues and exit\n"                   \
    "  -c               Show all broker connections and exit\n"              \
    "  -d               Show date and time in shell output\n"                \
    "  -v               Show version information\n"                          \
    "  -h               Show summary of command-line options\n"              \
    "\n"                                                                     \
    "The order of arguments is not important. Switches and filenames\n"      \
    "are case sensitive. See documentation for detailed information.\n"      \
    "\n"

static void s_do_authenticated_port_scan (
    char *s_opt_host, char *s_opt_vhost, char *s_opt_user, char *s_opt_pass);

int main (int argc, char *argv[])
{
    int
        argn;                           //  Argument number
    Bool
        args_ok = TRUE,                 //  Were the arguments okay?
        s_opt_broker = FALSE,           //  -b means show broker status
        s_opt_clients = FALSE,          //  -c means show clients
        s_opt_report = FALSE,           //  -r means report brokers
        s_opt_addr = FALSE,             //  -a means show addresses
        s_opt_date = FALSE;             //  -d means show date, time
    char
        *s_opt_host = NULL,             //  -s specifies server name
        *s_opt_user = NULL,             //  -u specifies user name
        *s_opt_pass = NULL,             //  -p specifies password
        *s_opt_vhost = NULL,            //  -V specifies virtual host
        *s_opt_command = NULL,          //  -e specifies command string
        *s_opt_trace = NULL,            //  -t specifies trace level
        *s_opt_xml  = NULL;             //  -x specifies XML filename
    char
        **argparm;                      //  Argument parameter to pick-up
    amq_mgt_console_t
        *console;
    ipr_token_list_t
        *tokens;                        //  Command tokens
    FILE
        *xml_data = NULL;               //  XML capture stream

    argparm = NULL;                     //  Argument parameter to pick-up
    for (argn = 1; argn < argc; argn++) {
        //  If argparm is set, we have to collect an argument parameter
        if (argparm) {
            if (*argv [argn] != '-') {  //  Parameter can't start with '-'
                *argparm = argv [argn];
                argparm = NULL;
            }
            else {
                args_ok = FALSE;
                break;
            }
        }
        else
        if (*argv [argn] == '-') {
            switch (argv [argn][1]) {
                case 's':
                    argparm = &s_opt_host;
                    break;
                case 'u':
                    argparm = &s_opt_user;
                    break;
                case 'p':
                    argparm = &s_opt_pass;
                    break;
                case 'V':
                    argparm = &s_opt_vhost;
                    break;
                case 'e':
                    argparm = &s_opt_command;
                    break;
                case 't':
                    argparm = &s_opt_trace;
                    break;
                case 'x':
                    argparm = &s_opt_xml;
                    break;
                case 'b':
                    s_opt_broker = TRUE;
                    break;
                case 'c':
                    s_opt_clients = TRUE;
                    break;
                case 'r':
                    s_opt_report = TRUE;
                    break;
                case 'a':
                    s_opt_addr = TRUE;
                    break;
                case 'd':
                    s_opt_date = TRUE;
                    break;
                case 'v':
                    printf (PRODUCT "\n");
                    printf (COPYRIGHT "\n");
                    printf (BUILDMODEL "\n");
                    printf ("Built on: " BUILDDATE "\n");
#if defined (CCOPTS)
                    printf ("Compiler: " CCOPTS "\n");
#endif
                    exit (EXIT_SUCCESS);
                case 'h':
                    printf (PRODUCT "\n");
                    printf (BUILDMODEL "\n\n");
                    printf (COPYRIGHT "\n");
                    printf (NOWARRANTY);
                    printf (USAGE);
                    exit (EXIT_SUCCESS);
                //  Anything else is an error
                default:
                    args_ok = FALSE;
            }
        }
        else {
            args_ok = FALSE;
            break;
        }
    }
    //  Initialise global class so we can use the console
    icl_system_initialise (argc, argv);

    printf ("amq_shell/%s - Management Console for OpenAMQ Brokers\n", VERSION);
    printf (COPYRIGHT "\n\n");

    icl_console_mode (ICL_CONSOLE_DATE, s_opt_date);
    icl_console_mode (ICL_CONSOLE_TIME, s_opt_date);

    //  If there was a missing parameter or an argument error, quit
    if (argparm) {
        icl_console_print ("E: argument missing - use '-h' option for help");
        exit (EXIT_FAILURE);
    }
    else
    if (!args_ok) {
        icl_console_print ("E: invalid arguments - use '-h' option for help");
        exit (EXIT_FAILURE);
    }
    //  Default connection values
    if (!s_opt_host)
        s_opt_host = "localhost";
    if (!s_opt_user)
        s_opt_user = "guest";
    if (!s_opt_pass)
        s_opt_pass = "guest";
    if (!s_opt_vhost)
        s_opt_vhost = "/";
    if (!s_opt_trace)
        s_opt_trace = "0";

    if (s_opt_report)
        s_do_authenticated_port_scan (s_opt_host, s_opt_vhost, s_opt_user, s_opt_pass);

    //  Set global options
    g_opt_trace = atoi (s_opt_trace);
    g_opt_addr  = s_opt_addr;

    console = amq_mgt_console_new (s_opt_host, s_opt_vhost, s_opt_user, s_opt_pass);
    if (!console)
        exit (EXIT_FAILURE);

    icl_console_out ("Connected to %s/%s on %s\n",
        console->connection->server_product,
        console->connection->server_version,
        console->connection->server_host);

    if (s_opt_xml) {
        xml_data = fopen (s_opt_xml, "w");
        fprintf (xml_data, "<?xml?>\n");
        fprintf (xml_data, "<console_data>\n");
    }
    //  Either dump broker state and exit, or do full command line
    if (s_opt_broker) {
        amq_mgt_broker_print_properties (console->broker, xml_data);
        amq_mgt_broker_print_children   (console->broker, xml_data);
    }
    else
    if (s_opt_clients) {
        amq_mgt_connection_t
            *connection;                //  Child connection
        amq_mgt_broker_load (console->broker);

        connection = amq_mgt_broker_connection_first (console->broker);
        while (connection) {
            amq_mgt_connection_load (connection);
            amq_mgt_connection_print_summary (connection, xml_data);
            connection = amq_mgt_broker_connection_next (console->broker);
        }
    }
    else {
        if (s_opt_command)
            tokens = ipr_token_split_rich (s_opt_command, ";");
        else
            tokens = NULL;

        amq_mgt_broker_cmdline (console->broker, console->connection->server_host, 0, tokens, xml_data);
        ipr_token_list_destroy (&tokens);
    }
    if (s_opt_xml) {
        fprintf (xml_data, "</console_data>\n");
        fclose (xml_data);
    }
    amq_mgt_console_destroy (&console);

    icl_system_terminate ();            //  Terminate all classes
    return (EXIT_SUCCESS);
}

static void s_do_authenticated_port_scan (
    char *s_opt_host, char *s_opt_vhost, char *s_opt_user, char *s_opt_pass)
{
    int
        port;
    icl_longstr_t
        *auth_data;                     //  Authorisation data
    amq_client_connection_t
        *connection;
    icl_shortstr_t
        host;
    int
        brokers_found = 0;

    icl_console_out ("Scanning for all accessible brokers on %s (ports 5000-9999)", s_opt_host);
    auth_data = amq_client_connection_auth_plain (s_opt_user, s_opt_pass);
    for (port = 5000; port < 10000; port++) {
        icl_shortstr_fmt (host, "%s:%d", s_opt_host, port);
        connection = amq_client_connection_new (host, s_opt_vhost, auth_data, "amq_shell", 0, 1000);
        if (connection) {
            icl_console_out ("Found %s/%s on %s",
                connection->server_product, connection->server_version, host);
            brokers_found++;
            amq_client_connection_destroy (&connection);
        }
    }
    icl_longstr_destroy (&auth_data);
    if (!brokers_found)
        icl_console_out ("No accessible AMQP brokers found");
    exit (EXIT_SUCCESS);
}
