/*  -------------------------------------------------------------------------
 *
 *  amqpsrv - AMQP/Fast prototype server
 *
 *  Copyright (c) 2004 JPMorgan Inc.
 *  -------------------------------------------------------------------------*/

#include "amq_core.h"
#include "amq_server_agent.h"


/*  Register your modules here
    Function returns zero if OK, <> 0 if there was an error                  */

int
amq_user_modules (void)
{
    if (amq_server_agent_init ())
        return (1);
    return (0);
}

int
main (int argc, char *argv [])
{
    return (amq_server_core (argc, argv));
}
