/*---------------------------------------------------------------------------
 *  test_level0.c - code example for Level 0 AMQ Client API
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amqp_level0.h"
#include <stdio.h>
#include <stdlib.h>

apr_socket_t *sck;
int sender;
int tag;

apr_status_t handle_close_cb (
    void *hint,
    apr_uint16_t handle_id,
    apr_uint16_t reply_code,
    char *reply_text
    )
{
    fprintf (stderr, "Server closed the handle.\n%ld : %s\n",
        (long) reply_code, reply_text);
    return APR_SUCCESS;
}

apr_status_t channel_close_cb (
    void *hint,
    apr_uint16_t channel_id,
    apr_uint16_t reply_code,
    char *reply_text
    )
{
    fprintf (stderr, "Server closed the channel.\n%ld : %s\n",
        (long) reply_code, reply_text);
    return APR_SUCCESS;
}

apr_status_t connection_close_cb (
    void *hint,
    apr_uint16_t reply_code,
    char *reply_text
    )
{
    fprintf (stderr, "Server closed the connection.\n%ld : %s\n",
        (long) reply_code, reply_text);
    return APR_SUCCESS;
}

apr_status_t send_message(apr_uint16_t confirm_tag)
{
    apr_status_t result;
    apr_uint16_t value;
    char buffer [32768];
    char data [2];

    /* wait for 5 seconds */
    apr_sleep (5000000);

    /* random message value */
    value = rand () % 0xffff;

    *( (apr_uint16_t*) data) = htons (value);

    result = amqp_handle_send (sck, buffer, 32767, 1, confirm_tag, 0, 0, 0, 0, "",
        2, 0, 0, 0, "", "", "myMessage", 0, "", data);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "amqp_handle_send failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 32767) );
        return result;
    }

    fprintf (stderr, "Message with value %ld being sent.\n", (long) value);

    return APR_SUCCESS;            
}

apr_status_t handle_reply_cb (
    void *hint,
    apr_uint16_t handle_id,
    apr_uint16_t confirm_tag,
    apr_uint16_t reply_code,
    char *reply_text
    )
{
    apr_status_t result;
    char buffer [32768];

    if (confirm_tag == 3) fprintf (stderr, "Handle opened.\n");

    if (confirm_tag == 3 && sender == 0) {
        /* receiver : reply to handle open */
        result = amqp_handle_consume (sck, buffer, 32767, 1, 4, 10, 0, 0,
            "", "", 0, "", "");
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_handle_consume failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );
            return result;
        }
    }
    if (confirm_tag == 3 && sender == 1) {
        /* sender : reply to handle open */
        return send_message (confirm_tag + 1);
    }
    if (confirm_tag == 4 && sender == 0) {
        /* receiver : reply to handle consume */
        tag = confirm_tag;
    }
    if (confirm_tag >=4 && sender == 1) {
        /* sender : reply to handle send */
        fprintf (stderr, "Message sent.\n");
        return send_message (confirm_tag +1);
    }
    return APR_SUCCESS;
}

apr_status_t channel_reply_cb (
    void *hint,
    apr_uint16_t channel_id,
    apr_uint16_t confirm_tag,
    apr_uint16_t reply_code,
    char *reply_text
    )
{
    apr_status_t result;
    char buffer [32768];

    if (confirm_tag == 2) {
        /* reply to channel open */
        fprintf (stderr, "Channel opened.\n");

        result = amqp_handle_open (sck, buffer, 32767, 1, 1, 1, 3, 1, 1, 0, 0,
            "test-small", "", "", 0, "");
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_handle_open failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );
            return result;
        }
    }
    if (confirm_tag >= 5)
    {
        /* receiver : reply to channel ack */
        fprintf (stderr, "Message acknowledged.\n");
    }
    return APR_SUCCESS;
}

apr_status_t connection_reply_cb (
    void *hint,
    apr_uint16_t confirm_tag,
    apr_uint16_t reply_code,
    char *reply_text
    )
{
    apr_status_t result;
    char buffer [32768];

    if (confirm_tag == 1) {
        /* reply to connection open */
        result = amqp_channel_open (sck, buffer, 32767, 1, 2, 0, 0, 0, "", "");
        if (result != APR_SUCCESS) {
            fprintf (stderr, "amqp_channel_open failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 32767) );
            return result;
        }
        fprintf (stderr, "Connected to server.\n");
    }
    return APR_SUCCESS;
}
    
apr_status_t connection_challenge_cb (
    void *hint,
    apr_byte_t version,
    char *mechanisms,
    apr_size_t challenges_size,
    char *challenges
    )
{
    apr_status_t result;
    char buffer [32768];

    result = amqp_connection_response (sck, buffer, 32767, "plain", 0, "");
    if (result != APR_SUCCESS) {
        fprintf (stderr, "amqp_connection_response failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 32767) );
        return result;
    }
    
    return APR_SUCCESS;
}

apr_status_t connection_tune_cb (
    void *hint,
    apr_uint32_t frame_max,
    apr_uint16_t channel_max,
    apr_uint16_t handle_max,
    apr_uint16_t heartbeat,
    apr_size_t options_size,
    char *options
    )
{
    apr_status_t result;
    char buffer [32768];
    char data [2];
    data [0] = 1;
    data [1] = 2;

    result = amqp_connection_tune (sck, buffer, 32767, 32000, 255, 255, 0, 
        options_size, options);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "amqp_connection_tune failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 32767) );
        return result;
    }
    result = amqp_connection_open (sck, buffer, 32767, 1, "/test",
        sender ? "sender" : "receiver", 0, "");
    if (result != APR_SUCCESS) {
        fprintf (stderr, "amqp_connection_open failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 32767) );
        return result;
    }
    return APR_SUCCESS;
}

apr_status_t handle_notify_cb (
    void *hint,
    apr_uint16_t handle_id,
    apr_uint32_t message_nbr,
    apr_byte_t partial,
    apr_byte_t out_of_band,
    apr_byte_t recovery,
    apr_byte_t delivered,
    apr_byte_t redelivered,
    apr_byte_t streaming,
    char *dest_name,
    apr_uint32_t body_size,
    apr_byte_t persistent,
    apr_byte_t priority,
    apr_uint32_t expiration,
    char *mime_type,
    char *encoding,
    char *identifier,
    apr_size_t headers_size,
    char *headers,
    char *data
    )
{
    apr_status_t result;
    char buffer [32768];

    apr_uint16_t value = ntohs (*( (apr_uint16_t*) data) );
    fprintf (stderr, "Message with value %ld received.\n", (long) value);

    tag++;

    result = amqp_channel_ack (sck, buffer, 32767, 1, tag, message_nbr);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "amqp_channel_ack failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 32767) );
        return result;
    }
    return APR_SUCCESS;
}

int main (int argc, const char *const argv[], const char *const env[]) 
{
    apr_status_t result;
    apr_pool_t *pool = NULL;
    char *addr = NULL;
    char *scope_id = NULL;
    apr_port_t port;
    apr_sockaddr_t *sockaddr = NULL;
    char buffer [2000];
    amqp_callbacks_t callbacks;
    memset ( (void*) &callbacks, 0, sizeof (amqp_callbacks_t) );
    
    if (argc != 2) {
        fprintf (stderr, "Use '%s -sender' to start sender client,"
            " -receiver to start receiver client.\n", argv[0]);
        return -1;
    }
    
    if (strcmp (argv[1], "-sender") == 0) sender = 1;
    else if (strcmp (argv[1], "-receiver") == 0) sender = 0;
    else {
        fprintf (stderr, "Use '%s -sender' to start sender client,"
            " -receiver to start receiver client.\n", argv[0]);
        return -1;
    }

    fprintf (stderr, "Connecting to server.\n");

    result = apr_app_initialize (&argc, &argv, &env);
    if(result != APR_SUCCESS) {
        fprintf (stderr, "apr_app_initialize failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_pool_initialize();
    if(result != APR_SUCCESS) {
        fprintf (stderr, "apr_pool_initialize failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_pool_create(&pool, NULL);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "apr_pool_create failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_socket_create (&sck, APR_INET, SOCK_STREAM, APR_PROTO_TCP, pool);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "apr_socket_create failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_parse_addr_port (&addr, &scope_id, &port, "127.0.0.1:7654", pool);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "apr_parse_addr_port failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_sockaddr_info_get(&sockaddr, addr, AF_UNSPEC, port, APR_IPV4_ADDR_OK, pool);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "apr_sockaddr_info_get failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = apr_socket_connect(sck, sockaddr);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "apr_socket_connect failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    result = amqp_init(sck);
    if (result != APR_SUCCESS) {
        fprintf (stderr, "amqp_init failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        goto err;
    }

    callbacks.connection_challenge = connection_challenge_cb;
    callbacks.connection_tune = connection_tune_cb;
    callbacks.handle_notify = handle_notify_cb;
    callbacks.connection_reply = connection_reply_cb;
    callbacks.channel_reply = channel_reply_cb;
    callbacks.handle_reply = handle_reply_cb;
    callbacks.connection_close = connection_close_cb;
    callbacks.channel_close = channel_close_cb;
    callbacks.handle_close = handle_close_cb;

    while (1)
    {
        result=amqp_recv (sck, buffer, 2000, &callbacks, NULL);
        if (result != APR_SUCCESS && result != APR_TIMEUP) {
            fprintf (stderr, "amqp_recv failed.\n%ld : %s\n",
                (long) result, amqp_strerror (result, buffer, 2000) );
            goto err;
        }
        apr_sleep (100000);
    }

err:
    
    fprintf (stderr, "Terminating client.\n");

    result=amqp_term (sck);
    if (result != APR_SUCCESS)
        fprintf (stderr, "apr_term failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );
        
    result = apr_socket_close (sck);
    if (result != APR_SUCCESS)
        fprintf (stderr, "apr_socket_close failed.\n%ld : %s\n",
            (long) result, amqp_strerror (result, buffer, 2000) );

    apr_terminate();

    return -1;
}

