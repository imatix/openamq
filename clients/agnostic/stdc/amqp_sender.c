
#include "amqp_sender.h"

void *sender_thread(apr_thread_t *thread, void *data)
{
    while (1) {

        /*  connection level commands - ok, write them
            channel and hadnle level -
              find channel object
              make one step in OUT state machine
              send command
        */
    }
}
