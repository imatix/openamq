/*  -------------------------------------------------------------------------
 *
 *  openamqd - OpenAMQ server
 *
 *  Copyright (c) 2004 JPMorgan Inc.
 *  -------------------------------------------------------------------------*/

#include "amq_core.h"


/*  Register your modules here
    Function returns zero if OK, <> 0 if there was an error                  */

int
amq_user_modules (void)
{
    return (0);
}

int
main (int argc, char *argv [])
{
    amq_set_server_name ("openamqd");
    amq_set_server_text ("OpenAMQ server daemon, implementing AMQP/Fast protocol");
    return (amq_server_core (argc, argv));
}
