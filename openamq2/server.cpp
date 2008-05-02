
#include <unistd.h>

#include "dispatcher.hpp"
#include "api_engine.hpp"
#include "poll_thread.hpp"
#include "amqp09_listener.hpp"
using namespace zmq;

int main ()
{
    // *************************************************************************
    //  Setup OpenAMQ/2.0 broker

    printf ("OpenAMQ/2.0a0 broker starting up\n");

    //  Only 1 thread needed. We can modify the code once we'll need to test
    //  on multicore boxes
    dispatcher_t dispatcher (1);
    poll_thread_t pt (&dispatcher);

    //  Register AMQP connection listener with 0MQ kernel
    poll_thread_t *pt_array [] = {&pt};
    amqp09_listener_t::create (&pt, "0.0.0.0", 5672, 1, pt_array);

    printf (" - ready for AMQP/0-9 connections\n");

    //  Hibernate setup thread
    sleep (24 * 3600);
}
