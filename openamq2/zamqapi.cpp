//  Client library intended to be called from C programs
//  Part of OpenAMQ/2.0 WireAPI layer

#include <unistd.h>
#include <iostream>
using namespace std;

#include "dispatcher.hpp"
#include "api_engine.hpp"
#include "poll_thread.hpp"
#include "amqp09_client_engine.hpp"
#include "msg.hpp"
#include "pipe.hpp"

//  Prototypes for function defined herein

#include "zamqapi.h"

using namespace zmq;

static dispatcher_t *dispatcher;
static amqp09_client_engine_t *engine;
static api_engine_t *api;
static poll_thread_t *pt;

int zamq_open (char *host)
{
    assert (host);
    
    // *************************************************************************
    //  Setting up OpenAMQ/2.0 client
    dispatcher = new dispatcher_t (2);

    //  This instance of 0MQ will run with 2 threads - a client (API) thread
    //  and a worker (poll) thread.    
    api = new api_engine_t (dispatcher);
    pt = new poll_thread_t (dispatcher);

    //  Create AMQP client engine and register it with 0MQ kernel
    //  Exchange and routing key parameters are neccassary to interoperate
    //  with OpenAMQ/1.2 broker. OpenAMQ/2.0 broker ignores them at the moment.
    engine = amqp09_client_engine_t::create (
             pt, false, host, 5672, 8192, 8192,
             "amq.direct", "rk", "amq.direct", "rk");

    //  Wait till local bindings between API thread and working thread
    //  come into effect (otherwise we may lose some messages)
    sleep (1);
    return (0);
}

int zamq_close (void)
{
    // delete dispatcher;
    // delete api;
    // delete pt;
    
    //  We don't have any synchronisation with the sending threads, so this 
    //  lets them finish any unsent messages.  It's a hack and will fail if
    //  the amount of pending data is too high.
    sleep (1);
    return (0);
}

int zamq_send (void *content, size_t length)
{
    void *msg_out = msg_alloc (length);
    memcpy (msg_data (msg_out), content, msg_size (msg_out));
    api->send (msg_out);
    return (0);
}
    
int zamq_receive (void *content, size_t max_length, int block)
{
    int rc;
        
    void *msg_in = api->receive ((bool) block);
    if (msg_in) {
        assert (msg_size (msg_in) <= max_length);
        memcpy (content, msg_data (msg_in), msg_size (msg_in)); 
        rc = msg_size (msg_in);
    }
    else
        rc = 0;                         //  Size of message = zero

    msg_dealloc (msg_in);
    return (rc);
}

