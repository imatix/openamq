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
//  Usage:    measure <server> <instance-name> <no-of-messages> <size-of-message>
//  Example:  measure localhost:5672 test1 10000 512
//  Measures throughput and latency of AMQP broker

#include "base.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"

typedef struct
{
    char
        *instance_name;    //  Name of measure instance
    amq_client_session_t
        *session;          //  Session to send messages on
    int
        message_count,     //  How many messages to send
        message_size;      //  Size of individual message
    apr_time_t
        **times;           //  Pointer to the field to store send times in
}  sender_context_t;

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

int throughput_cmp (const void *x, const void *y)
{
    long
        xl = *(long*) x,
        yl = *(long*) y;

    if (xl < yl)
        return -1;
    if (xl > yl)
        return 1;
    return 0;
}

void * APR_THREAD_FUNC sender_thread_func (apr_thread_t *thread, void *ctx)
{
    sender_context_t
        *context = (sender_context_t*) ctx;
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
        (*context->times) [counter] = apr_time_now ();

        //  Send the message
        amq_client_session_basic_publish (
            context->session,               //  session
            content,                        //  content to send
            0,                              //  ticket
            "amq.direct",                   //  exchange to send message to
            context->instance_name,         //  routing-key
            FALSE,                          //  mandatory
            FALSE);                         //  immediate

        //  Release the message
        amq_content_basic_unlink (&content);
    }

    //  Destroy message body
    icl_mem_free (message_body);

    return NULL;
}

//  Time slice for measiring the throughput is 1/100 of second
#define TIME_SLICE 10000L
#define TIME_SLICES_PER_SECOND (1000000L / TIME_SLICE)

void compute_statistics (
    char *instance_name,
    int message_count,
    int message_size,
    apr_time_t *out_times,
    apr_time_t *in_times)
{
    //  Controlling variable for various loops
    int
        counter;

    //  Fields for normalised results
    apr_interval_time_t
        *outtn,
        *intn;

    //  Field for unsorted & sorted latencies
    apr_interval_time_t
        *latencies,
        *sorted_latencies;

    //  Throughput fields
    int
        time_slices;
    long
        *sth,         //  Sender throughputs
        *rth,         //  Receiver throughputs
        *sorted_sth,  //  Sorted sender throughputs
        *sorted_rth;  //  Sorted receiver throughputs

    //  Aggregates
    apr_interval_time_t
        latency_avg,
        latency_med,
        latency_dev;
    long
        sth_avg,
        sth_med,
        sth_dev,
        rth_avg,
        rth_med,
        rth_dev;

    //  Output file
    char
        filename [256];
    FILE
        *out;

    //  Initialise auxiliary fields
    outtn = icl_mem_alloc (message_count * sizeof (apr_interval_time_t));
    assert (outtn);
    intn = icl_mem_alloc (message_count * sizeof (apr_interval_time_t));
    assert (intn);
    latencies = icl_mem_alloc (message_count * sizeof (apr_interval_time_t));
    assert (latencies);
    sorted_latencies = icl_mem_alloc (message_count *
        sizeof (apr_interval_time_t));
    assert (sorted_latencies);

    //  Normalise measured results
    //  Time of sending the first message is considered to be 0
    for (counter = 0; counter != message_count; counter++) {
        outtn [counter] = out_times [counter] - out_times [0];
        intn [counter] = in_times [counter] - out_times [0];
    }

    //  Fill in unsorted & sorted latency fields
    for (counter = 0; counter != message_count; counter++) {
        latencies [counter] = intn [counter] - outtn [counter];
        sorted_latencies [counter] = intn [counter] - outtn [counter];
    }
    qsort (sorted_latencies, message_count, sizeof (apr_interval_time_t),
        latency_cmp);

    //  Compute averge latency, median latency and standard deviation
    latency_med = sorted_latencies [message_count / 2];
    latency_avg = 0;
    latency_dev = 0;
    for (counter = 0; counter != message_count; counter++) {
        latency_avg += latencies [counter];
        latency_dev += ((latencies [counter] - latency_med > 0) ?
            (latencies [counter] - latency_med) :
            -(latencies [counter] - latency_med));
    }
    latency_avg /= message_count;
    latency_dev /= (message_count - 1);

    //  Allocate & fill in throughput fields
    time_slices = 
        (int) ((intn [message_count - 1] - outtn [0]) / TIME_SLICE) + 1;
    sth = icl_mem_alloc (time_slices * sizeof (long));
    assert (sth);
    memset (sth, 0, time_slices * sizeof (long));
    rth = icl_mem_alloc (time_slices * sizeof (long));
    assert (rth);
    memset (rth, 0, time_slices * sizeof (long));
    sorted_sth = icl_mem_alloc (time_slices * sizeof (long));
    assert (sorted_sth);
    memset (sorted_sth, 0, time_slices * sizeof (long));
    sorted_rth = icl_mem_alloc (time_slices * sizeof (long));
    assert (sorted_rth);
    memset (sorted_rth, 0, time_slices * sizeof (long));
    for (counter = 0; counter != message_count; counter++) {
        sth [outtn [counter] / TIME_SLICE] += TIME_SLICES_PER_SECOND;
        rth [intn [counter] / TIME_SLICE] += TIME_SLICES_PER_SECOND;
        sorted_sth [outtn [counter] / TIME_SLICE] += TIME_SLICES_PER_SECOND;
        sorted_rth [intn [counter] / TIME_SLICE] += TIME_SLICES_PER_SECOND;
    }
    qsort (sorted_sth, time_slices, sizeof (long), throughput_cmp);
    qsort (sorted_rth, time_slices, sizeof (long), throughput_cmp);

    //  Compute average throughput, median throughput and
    //  standard deviation for the sender
    sth_med = sth [time_slices / 2];
    sth_avg = 0;
    sth_dev = 0;
    for (counter = 0; counter != time_slices; counter++) {
        sth_avg += sth [counter];
        sth_dev += ((sth [counter] - sth_med > 0) ?
            (sth [counter] - sth_med) :
            -(sth [counter] - sth_med));
    }
    sth_avg /= time_slices;
    sth_dev /= (time_slices - 1);

    //  Compute average throughput, median throughput and
    //  standard deviation for the receiver
    rth_med = rth [time_slices / 2];
    rth_avg = 0;
    rth_dev = 0;
    for (counter = 0; counter != time_slices; counter++) {
        rth_avg += rth [counter];
        rth_dev += ((rth [counter] - rth_med > 0) ?
            (rth [counter] - rth_med) :
            -(rth [counter] - rth_med));
    }
    rth_avg /= time_slices;
    rth_dev /= (time_slices - 1);

    //  Print out the aggregates
    printf ("**************************************************************\n");
    printf ("PERFORMANCE STATISTICS (%s - %d messages %d bytes long)\n",
        instance_name, message_count, message_size);
    printf ("\n");
    printf ("Total time: %ld microseconds\n",
        (long) intn [message_count - 1]);
    printf ("Time slice: %ld microseconds\n", (long) TIME_SLICE);
    printf ("\n");
    printf ("Minimal latency: %ld microseconds\n",
        (long) sorted_latencies [0]);
    printf ("Maximal latency: %ld microseconds\n",
        (long) sorted_latencies [message_count - 1]);
    printf ("Median latency: %ld microseconds\n", (long) latency_med);
    printf ("Average latency: %ld microseconds\n", (long) latency_avg);
    printf ("Standard robust deviation of latency: %ld microseconds\n",
        (long) latency_dev);
    printf ("\n");
    printf ("Median sender throughput: %ld messages/second\n", (long) sth_med);
    printf ("Average sender throughput: %ld messages/second\n", (long) sth_avg);
    printf ("Standard robust deviation of sender throughput: "
        "%ld messages/second\n", (long) sth_dev);
    printf ("\n");
    printf ("Median receiver throughput: "
        "%ld messages/second\n", (long) rth_med);
    printf ("Average receiver throughput: "
        "%ld messages/second\n", (long) rth_avg);
    printf ("Standard robust deviation of receiver throughput: "
        "%ld messages/second\n", (long) rth_dev);
    printf ("**************************************************************\n");

    //  Save normalised in & out times into files
    sprintf (filename, "%s.inoutn.dat", instance_name);
    out = fopen (filename, "w");
    assert (out);
    for (counter = 0; counter != message_count; counter++)
        fprintf (out, "%ld %ld\n",
            (long) outtn [counter], (long) intn [counter]);
    fclose (out);

    //  Save the latencies into file
    sprintf (filename, "%s.latency.dat", instance_name);
    out = fopen (filename, "w");
    assert (out);
    for (counter = 0; counter != message_count; counter++)
        fprintf (out, "%ld\n", (long) latencies [counter]);
    fclose (out);

    //  Save the throughputs into file
    sprintf (filename, "%s.throughput.dat", instance_name);
    out = fopen (filename, "w");
    assert (out);
    for (counter = 0; counter != time_slices; counter++)
        fprintf (out, "%f %ld\n", ((double) counter) / TIME_SLICES_PER_SECOND,
            (long) sth [counter]);
    fprintf (out, "\n\n");
    for (counter = 0; counter != time_slices; counter++)
        fprintf (out, "%f %ld\n", ((double) counter) / TIME_SLICES_PER_SECOND,
            (long) rth [counter]);
    fclose (out);

    //  Save gnuplot file to print out the graphs
    sprintf (filename, "%s.gnuplot", instance_name);
    out = fopen (filename, "w");
    assert (out);
    fprintf (out, "set ylabel 'latency (microseconds)'\n");
    fprintf (out, "set xlabel 'message number'\n");
    fprintf (out, "set grid\n");
    fprintf (out, "set grid noxtics\n");
    fprintf (out, "set terminal png medium size 640,480\n");
    fprintf (out, "set output '%s.latency.png'\n", instance_name);
    fprintf (out, "plot '%s.latency.dat' t 'latency' with lines\n",
        instance_name);
    fprintf (out, "set ylabel 'throughput (messages/second)'\n");
    fprintf (out, "set xlabel 'time (seconds)'\n");
    fprintf (out, "set output '%s.throughput.png'\n", instance_name);
    fprintf (out, "plot '%s.throughput.dat' index 0 t 'sender' with lines, ",
        instance_name);
    fprintf (out, "'%s.throughput.dat' index 1 t 'receiver' with lines",
        instance_name);
    fclose (out);

    //  Destroy auxiliary fields
    icl_mem_free (outtn);
    icl_mem_free (intn);
    icl_mem_free (latencies);
    icl_mem_free (sorted_latencies);
    icl_mem_free (sth);
    icl_mem_free (rth);
    icl_mem_free (sorted_sth);
    icl_mem_free (sorted_rth);
}

int main (int argc, char *argv [])
{
    //  Command line arguments
    char
        *host,
        *instance_name;
    int
        message_count,
        message_size;

    //  Managing sender thread
    apr_status_t
        rc,
        thread_rc;
    apr_pool_t
        *pool;
    apr_threadattr_t
        *thread_attr;
    apr_thread_t
        *sender_thread;
    sender_context_t
        sender_context;

    //  OpenAMQ related variables
    amq_client_connection_t
        *connection_out,
        *connection_in;
    amq_client_session_t
        *session_out,
        *session_in;
    icl_longstr_t
        *auth_data;
    amq_content_basic_t
        *content;

    //  Controlling receiver loop
    int
        messages_received;

    //  Fields to store results to
    apr_time_t
        *out_times,
        *in_times;

    //  Get command line parameters
    if (argc != 5) {
        printf ("Usage: measure <server> <no-of-messages> <size-of-message>\n");
        exit (1);
    }
    host = argv [1];
    instance_name = argv [2];
    message_count = atoi (argv [3]);
    message_size = atoi (argv [4]);

    //  Initialise system
    icl_system_initialise (argc, argv);

    //  Create memory pool
    rc = apr_pool_create (&pool, NULL);
    assert (rc == APR_SUCCESS);

    //  Set clock into hi resolution mode
    //  (we want to monitor with microsecond precision)
    apr_time_clock_hires (pool);

    //  Allocate out & in time fields
    out_times = icl_mem_alloc (message_count * sizeof (apr_time_t));
    assert (out_times);
    in_times = icl_mem_alloc (message_count * sizeof (apr_time_t));
    assert (in_times);

    //  Open connection & channel
    auth_data = amq_client_connection_auth_plain ("guest", "guest");
    connection_out = amq_client_connection_new (
            host, "/", auth_data, "measure_out", 0, 30000);
    assert (connection_out);
    session_out = amq_client_session_new (connection_out);
    assert (session_out);
    connection_in = amq_client_connection_new (
            host, "/", auth_data, "measure_in", 0, 30000);
    assert (connection_in);
    session_in = amq_client_session_new (connection_in);
    assert (session_in);
    icl_longstr_destroy (&auth_data);

    //  Create wiring
    amq_client_session_queue_declare (
        session_in,                     //  session
        0,                              //  ticket
        instance_name,                  //  queue name
        FALSE,                          //  passive
        FALSE,                          //  durable
        TRUE,                           //  exclusive
        TRUE,                           //  auto-delete
        NULL);                          //  arguments
    amq_client_session_queue_bind (
        session_in,                     //  session
        0,                              //  ticket
        instance_name,                  //  queue
        "amq.direct",                   //  exchange
        instance_name,                  //  routing-key
        NULL);                          //  arguments
    amq_client_session_basic_consume (
        session_in,                     //  session
        0,                              //  ticket
        instance_name,                  //  queue
        NULL,                           //  consumer-tag
        TRUE,                           //  no-local
        TRUE,                           //  no-ack
        TRUE,                           //  exclusive
        NULL);                          //  arguments

    //  Start the sending thread
    sender_context.instance_name = instance_name;
    sender_context.session = session_out;
    sender_context.message_count = message_count;
    sender_context.message_size = message_size;
    sender_context.times = &out_times;
    rc = apr_threadattr_create (&thread_attr, pool);
    assert (rc == APR_SUCCESS);
    rc = apr_threadattr_detach_set (thread_attr, 0);
    assert (rc == APR_SUCCESS);
    rc = apr_thread_create (&sender_thread, thread_attr, sender_thread_func,
        (void*) &sender_context, pool);
    assert (rc == APR_SUCCESS);

    //  Get received messages
    messages_received = 0;
    while (1) {

        while (1) {

            //  Get next message
            content = amq_client_session_basic_arrived (session_in);
            if (!content)
                break;

            //  Store the receival time
            in_times [messages_received] = apr_time_now ();

            //  Update message count
            messages_received++;
            if (messages_received == message_count)
                break;

            //  Destroy the message
            amq_content_basic_unlink (&content);
        }
        if (messages_received == message_count)
            break;

        //  Wait while next message arrives
        amq_client_session_wait (session_in, 0);

        //  Exit the loop if Ctrl+C is encountered
        if (!connection_in->alive)
            exit (1);
    }

    //  Wait while sending thread exits
    rc = apr_thread_join (&thread_rc, sender_thread);
    assert (rc == APR_SUCCESS);

    //  Compute statistics
    compute_statistics (instance_name, message_count, message_size,
        out_times, in_times);

    //  Close the connections & channels
    amq_client_session_destroy (&session_in);
    amq_client_connection_destroy (&connection_in);
    amq_client_session_destroy (&session_out);
    amq_client_connection_destroy (&connection_out);

    //  Deallocate allocated objects
    icl_mem_free (out_times);
    icl_mem_free (in_times);

    //  Destroy memory pool
    apr_pool_destroy (pool);

    //  Uninitialise system
    icl_system_terminate ();

    return 0;
}
