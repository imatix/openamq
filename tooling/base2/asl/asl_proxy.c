/*  ASL proxy v0.1

    todo: port to APR socket i/o
 */
 
#include "sfl.h"
#define BUFFER_MAX      32000

int main (int argc, char *argv [])
{
    sock_t
        master_handle = 0,
        client_handle = 0,
        server_handle = 0;
    byte
        buffer [BUFFER_MAX];
    int
        rc;
    fd_set
        read_set,                       //  Sockets to check for input         
        error_set;
    struct timeval
        timeout;                        //  Timeout for select()               
    FILE
        *logfile;
    char
        *hostname,
        *port;

    hostname = "localhost";
    port     = "7654";

    sock_init ();
    ip_nonblock = FALSE;
    logfile = fopen ("asl_proxy.log", "w");
    printf ("I: asl_proxy/0.1 ready\n");
    printf ("I: proxying from localhost:1999 to localhost:7654...\n");
    master_handle = passive_TCP ("1999", 1);

    FOREVER {
        client_handle = accept_socket (master_handle);
        printf ("I: have client connection\n");
        printf ("I: making server connection... ");
        server_handle = connect_TCP (hostname, port);
        if (server_handle == -1) {
            printf ("server connect failed: %s\n", sockmsg ());
            exit (1);
        }
        else
            printf ("Ok\n");

        FOREVER {
            timeout.tv_sec  = 1;                
            timeout.tv_usec = 0;  
            memset (&read_set,  0, sizeof (fd_set));
            memset (&error_set, 0, sizeof (fd_set));
            FD_SET (client_handle, &error_set);
            FD_SET (client_handle, &read_set);
            FD_SET (server_handle, &error_set);
            FD_SET (server_handle, &read_set);

            rc = sock_select (
                max (server_handle, client_handle) + 1,
                &read_set,
                NULL,
                &error_set,
                &timeout);

            if (rc == SOCKET_ERROR) {
                printf ("E: error on select()\n");
                break;
            }
            else
            if (rc == 0)                        //  No input or output activity        
                ;
            else
            if (FD_ISSET (server_handle, &error_set)) {
                printf ("E: error on server handle\n");
                break;
            }
            else
            if (FD_ISSET (client_handle, &error_set)) {
                printf ("E: error on client handle\n");
                break;
            }
            else
            if (FD_ISSET (server_handle, &read_set)) {
                if ((rc = read_TCP (server_handle, buffer, BUFFER_MAX)) < 1) {
                    printf ("E: server broke connection\n");
                    break;
                }
                printf ("I: server sent %d bytes\n", rc);
                write_TCP (client_handle, buffer, rc);
              }
            else
            if (FD_ISSET (client_handle, &read_set))
              {
                if ((rc = read_TCP (client_handle, buffer, BUFFER_MAX)) < 1) {
                    printf ("E: client broke connection\n");
                    break;
                }
                printf ("I: client sent %d bytes\n", rc);
                write_TCP (server_handle, buffer, rc);
              }
          }
        close (server_handle);
        close (client_handle);
    }
    close (master_handle);
    sock_term ();
    return (0);
}

