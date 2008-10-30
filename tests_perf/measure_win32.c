//
//  measure.c - Measures throughput and latency of OpenAMQ
//
//  Copyright (c) 1996-2007 iMatix Corporation
//  All rights reserved.
//  
//  This file is licensed under the BSD license as follows:
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//  
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation andor other materials provided with the
//    distribution.
//  * Neither the name of iMatix Corporation nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//  Name:     measure
//  Usage:    measure <server> <no-of-messages> <size-of-message>
//  Example:  measure localhost:5672 10000 512
//  Measures throughput and latency of AMQP broker

#include "base.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"

typedef struct
{
    char
        *host;
    amq_client_connection_t
        *connection_out,
        *connection_in;
    amq_client_session_t
        *session_out,
        *session_in;
    int
        message_count,
        message_size;
    apr_time_t
        *out_times,
        *in_times;
}  measure_context_t;

int latency_cmp (const void *x, const void *y)
{
    apr_interval_time_t
        xl = *(apr_interval_time_t*) x,
        yl = *(apr_interval_time_t*) y;

    if (xl < yl)
        return -1;
    if (xl > yl)
        return 1;
    return 0;
}

DWORD WINAPI sender_thread_func (LPVOID ctx)
{
    measure_context_t
        *context = (measure_context_t*) ctx;
    int
        counter;
    char
        *message_body;
    amq_content_basic_t
        *content;

    //  Create message body
    message_body = icl_mem_alloc (context->message_size);
    assert (message_body);
    memset (message_body, 0, context->message_size);
    
    for (counter = 0; counter != context->message_count; counter++) {

        //  Create the message itself
        content = amq_content_basic_new ();
        amq_content_basic_set_body (content, message_body,
            context->message_size, NULL);

        //  Store message send time
        context->out_times [counter] = apr_time_now ();

        //  Send the message
        amq_client_session_basic_publish (
            context->session_out,           //  session
            content,                        //  content to send
            0,                              //  ticket
            "amq.direct",                   //  exchange to send message to
            "rk",                           //  routing-key
            FALSE,                          //  mandatory
            FALSE);                         //  immediate

        //  Release the message
        amq_content_basic_unlink (&content);
    }

    //  Destroy message body
    icl_mem_free (message_body);

    return 0;
}

int main (int argc, char *argv [])
{
    apr_status_t
        rc;
    apr_pool_t
        *pool;
    icl_longstr_t
        *auth_data;
    int
        messages_received = 0,
        counter;
    apr_interval_time_t
        total_time,
        latency,
        min_latency = -1,
        max_latency = -1,
        sum_latency = 0,
        avg_latency,
        med_latency,
        dev_latency = 0,
        *latencies;
    long
        total_throughput,
        sender_throughput,
        receiver_throughput;
    amq_content_basic_t
        *content;
    measure_context_t
        context;
    FILE
        *out;
    HANDLE
        sender_thread;

    //  Get command line parameters
    if (argc != 4) {
        printf ("Usage: measure <server> <no-of-messages> <size-of-message>\n");
        exit (1);
    }
    context.host = argv [1];
    context.message_count = atoi (argv [2]);
    context.message_size = atoi (argv [3]);

    //  Initialise system
    icl_system_initialise (argc, argv);

    //  Create memory pool
    rc = apr_pool_create (&pool, NULL);
    assert (rc == APR_SUCCESS);

    //  Allocate out & in time fields
    context.out_times = icl_mem_alloc (context.message_count * sizeof (apr_time_t));
    assert (context.out_times);
    context.in_times = icl_mem_alloc (context.message_count * sizeof (apr_time_t));
    assert (context.in_times);

    //  Open connection & channel
    auth_data = amq_client_connection_auth_plain ("guest", "guest");
    context.connection_out = amq_client_connection_new (
            context.host, "/", auth_data, "measure_out", 0, 30000);
    assert (context.connection_out);
    context.session_out = amq_client_session_new (context.connection_out);
    assert (context.session_out);
    context.connection_in = amq_client_connection_new (
            context.host, "/", auth_data, "measure_in", 0, 30000);
    assert (context.connection_in);
    context.session_in = amq_client_session_new (context.connection_in);
    assert (context.session_in);
    icl_longstr_destroy (&auth_data);

    //  Create wiring
    amq_client_session_queue_declare (
        context.session_in,             //  session
        0,                              //  ticket
        NULL,                           //  queue name
        FALSE,                          //  passive
        FALSE,                          //  durable
        TRUE,                           //  exclusive
        TRUE,                           //  auto-delete
        NULL);                          //  arguments
    amq_client_session_queue_bind (
        context.session_in,             //  session
        0,                              //  ticket
        NULL,                           //  queue
        "amq.direct",                   //  exchange
        "rk",                           //  routing-key
        NULL);                          //  arguments
    amq_client_session_basic_consume (
        context.session_in,             //  session
        0,                              //  ticket
        NULL,                           //  queue
        NULL,                           //  consumer-tag
        TRUE,                           //  no-local
        TRUE,                           //  no-ack
        TRUE,                           //  exclusive
        NULL);                          //  arguments

    //  Start the sending thread
    sender_thread = CreateThread (NULL, 0, sender_thread_func,
        (LPVOID) &context, 0, NULL);
    assert (sender_thread);

    //  Get received messages
    while (1) {

        while (1) {

            //  Get next message
            content = amq_client_session_basic_arrived (context.session_in);
            if (!content)
                break;

            //  Store the receival time
            context.in_times [messages_received] = apr_time_now ();

            //  Update message count
            messages_received++;
            if (messages_received == context.message_count)
                break;

            //  Destroy the message
            amq_content_basic_unlink (&content);
        }
        if (messages_received == context.message_count)
            break;

        //  Wait while next message arrives
        amq_client_session_wait (context.session_in, 0);

        //  Exit the loop if Ctrl+C is encountered
        if (!context.connection_in->alive)
            exit (1);
    }

    //  Wait while sending thread exits
    WaitForSingleObject (sender_thread, INFINITE);

    //  Compute statistics and create CSV file

    total_time = context.in_times [context.message_count - 1] -
        context.out_times [0];

    latencies = icl_mem_alloc
        (context.message_count * sizeof (apr_interval_time_t));
    assert (latencies);

    for (counter = 0; counter != context.message_count; counter++) {
          latency = context.in_times [counter] - context.out_times [counter];
        latencies [counter] = latency;
        if (latency < min_latency || min_latency == -1)
            min_latency = latency;
        if (latency > max_latency || max_latency == -1)
            max_latency = latency;
        sum_latency += latency;
    }
    avg_latency = sum_latency / context.message_count;
    qsort (latencies, context.message_count, sizeof (apr_interval_time_t),
        latency_cmp);
    med_latency = latencies [context.message_count / 2];

    out = fopen ("out.csv", "w");
    assert (out);

    for (counter = 0; counter != context.message_count; counter++) {
          latency = context.in_times [counter] - context.out_times [counter];

        dev_latency += ((latency - med_latency) < 0 ?
            -(latency - med_latency) : (latency - med_latency));
        fprintf (out, "%ld,%ld\n", (long) latency, (long) med_latency);
    }
    dev_latency /= context.message_count;

    fclose (out);

    icl_mem_free (latencies);

    
    //  Compute throughputs
    total_throughput =
        (context.in_times [context.message_count - 1] - context.out_times [0]) ?
        ((context.message_count * 1000000L) /
        (context.in_times [context.message_count - 1] - context.out_times [0])) : 0;
    sender_throughput = 
        (context.out_times [context.message_count - 1] - context.out_times [0]) ?
        ((context.message_count * 1000000L) /
        (context.out_times [context.message_count - 1] - context.out_times [0])) : 0;
    receiver_throughput =
        (context.in_times [context.message_count - 1] - context.in_times [0]) ?
        ((context.message_count * 1000000L) /
        (context.in_times [context.message_count - 1] - context.in_times [0])) : 0;
    
    //  Print out the statistics
    printf ("Total time: %ld microseconds\n", (long) total_time);
    printf ("Minimal latency: %ld microseconds\n", (long) min_latency);
    printf ("Maximal latency: %ld microseconds\n", (long) max_latency);
    printf ("Average latency: %ld microseconds\n", (long) avg_latency);
    printf ("Median latency: %ld microseconds\n", (long) med_latency);
    printf ("Average peak: %ld microseconds\n", (long) dev_latency);
    printf ("Total througput: %ld messages/second\n", total_throughput);
    printf ("Sender througput: %ld messages/second\n", sender_throughput);
    printf ("Receiver througput: %ld messages/second\n", receiver_throughput);

    //  Close the connections & channels
    amq_client_session_destroy (&context.session_in);
    amq_client_connection_destroy (&context.connection_in);
    amq_client_session_destroy (&context.session_out);
    amq_client_connection_destroy (&context.connection_out);

    //  Deallocate allocated objects
    icl_mem_free (context.out_times);
    icl_mem_free (context.in_times);

    //  Destroy memory pool
    apr_pool_destroy (pool);

    //  Uninitialise system
    icl_system_terminate ();

    return 0;
}
