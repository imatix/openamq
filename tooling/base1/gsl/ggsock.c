/*===========================================================================*
 *                                                                           *
 *  ggsock.c - Socket object functions                                       *
 *                                                                           *
 *  Copyright (c) 1991-2009 iMatix Corporation                               *
 *                                                                           *
 *  ------------------ GPL Licensed Source Code ------------------           *
 *  iMatix makes this software available under the GNU General               *
 *  Public License (GPL) license for open source projects.  For              *
 *  details of the GPL license please see www.gnu.org or read the            *
 *  file license.gpl provided in this package.                               *
 *                                                                           *
 *  This program is free software; you can redistribute it and/or            *
 *  modify it under the terms of the GNU General Public License as           *
 *  published by the Free Software Foundation; either version 2 of           *
 *  the License, or (at your option) any later version.                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public                *
 *  License along with this program in the file 'license.gpl'; if            *
 *  not, write to the Free Software Foundation, Inc., 59 Temple              *
 *  Place - Suite 330, Boston, MA 02111-1307, USA.                           *
 *                                                                           *
 *  You can also license this software under iMatix's General Terms          *
 *  of Business (GTB) for commercial projects.  If you have not              *
 *  explicitly licensed this software under the iMatix GTB you may           *
 *  only use it under the terms of the GNU General Public License.           *
 *                                                                           *
 *  For more information, send an email to info@imatix.com.                  *
 *  --------------------------------------------------------------           *
 *===========================================================================*/

#include "ggpriv.h"                     /*  Project header file              */
#include "ggsock.h"                     /*  Include header file              */
#include "ggsock.d"                     /*  Include dialog data              */


/*- Definitions -------------------------------------------------------------*/

#define AGENT_NAME      "GGSOCK"        /*  Our public name                  */
#define NAME "sock"

#define QUEUE_LENGTH 10

/*- Function prototypes -----------------------------------------------------*/

static EVAL_FUNCTION
    eval_connect,
    eval_read,
    eval_write,
    eval_close,
    eval_passive,
    eval_accept,
    eval_error;

static int  start_socket_agent (void);
static void reply_readh_result (byte *body, int rc);

/*- Type definitions --------------------------------------------------------*/

typedef struct {
    THREAD
        *gsl_thread;
    RESULT_NODE
        *result,
        *buffer;
    char
        *ref_name;
    sock_t
        handle;
} TCB;

/*- Global variables used in this source file only --------------------------*/

static TCB
    *tcb;                               /*  Address thread context block     */

static QUEUE
    *sockq;

static PARM_LIST
    one_value_parm_list = {PARM_VALUE};

static PARM_LIST
    read_parm_list  = {PARM_VALUE, PARM_REFERENCE, PARM_VALUE};

static GSL_FUNCTION sock_functions [] = 
{
    /*  Don't forget to keep these in alphaetical order!                     */
    {"accept",  1, 2, 1, (void *) &one_value_parm_list, FALSE, eval_accept},
    {"close",   1, 2, 1, (void *) &one_value_parm_list, FALSE, eval_close},
    {"connect", 2, 3, 1, (void *) &one_value_parm_list, FALSE, eval_connect},
    {"error",   0, 0, 1, NULL,                          TRUE,  eval_error},
    {"passive", 1, 1, 1, (void *) &one_value_parm_list, TRUE,  eval_passive},
    {"read",    2, 4, 3, (void *) &read_parm_list,      FALSE, eval_read},
    {"write",   2, 3, 1, (void *) &one_value_parm_list, FALSE, eval_write}
};

#define ERROR_MSG_LENGTH 80

static char
    error_msg [ERROR_MSG_LENGTH + 1];

/*- Functions ---------------------------------------------------------------*/

int register_sock_functions (void)
{
    AGENT   *agent;                     /*  Handle for our agent             */
#   include "ggsock.i"                  /*  Include dialog interpreter       */

    declare_smtlib_shutdown   (shutdown_event, SMT_PRIORITY_MAX);

    declare_smtsock_readh_ok      (readh_ok_event,      0);
    declare_smtsock_readh_closed  (readh_closed_event,  0);
    declare_smtsock_readh_timeout (readh_timeout_event, 0);

    declare_smtsock_connect_ok    (connect_ok_event,    0);

    declare_smtsock_ok            (ok_event,      0);
    declare_smtsock_closed        (closed_event,  0);
    declare_smtsock_timeout       (timeout_event, 0);
    declare_smtsock_error         (error_event,   0);

    error_msg [0] = '\0';;
    sockq         = NULL;

    return object_register (NAME, sock_functions, tblsize (sock_functions), NULL);
}


static int
eval_connect (int argc, RESULT_NODE **argv,
              RESULT_NODE *result,
              THREAD *gsl_thread)
{
    RESULT_NODE
        *host    = argc > 0 ? argv [0] : NULL,
        *service = argc > 1 ? argv [1] : NULL,
        *timeout = argc > 2 ? argv [2] : NULL;
    THREAD  
       *thread;

    if (! arguments_are_defined (argc, argv, result))
      {
        lsend_ggcode_result (& gsl_thread-> queue-> qid, NULL,
                             NULL, NULL, NULL, 0);
        return 0;
      }

    if (start_socket_agent ())
        return -1;

    thread = thread_create (AGENT_NAME, "");
    tcb = thread-> tcb;

    tcb-> gsl_thread = gsl_thread;
    tcb-> result     = result;
    tcb-> handle     = 0;

    lsend_smtsock_connect (& sockq-> qid,
                           & thread-> queue-> qid,
                           NULL, NULL, NULL, 0,
                           (dbyte) (timeout? number_value (& timeout-> value) : 0),
                           "tcp",
                           string_value (& host-> value),
                           string_value (& service-> value),
                           0, 0, 0);
    return 0;
}


static int
start_socket_agent (void)
{
    THREAD
        *thread;

    if (! sockq)
      {
        if (agent_lookup (SMT_SOCKET) == NULL)
            smtsock_init ();
        if ( (thread = thread_lookup (SMT_SOCKET, "")) != NULL )
            sockq = thread-> queue;
        else
          {
            strcpy (object_error, "Unable to start SOCK agent.");
            return -1;
          }
      }
    return 0;
}


static int
eval_read (int argc, RESULT_NODE **argv,
           RESULT_NODE *result,
           THREAD *gsl_thread)
{
    RESULT_NODE
        *handle  = argc > 0 ? argv [0] : NULL,
        *buffer  = argc > 1 ? argv [1] : NULL,
        *min     = argc > 2 ? argv [2] : NULL,
        *timeout = argc > 3 ? argv [3] : NULL;
    Bool
        error;
    THREAD  
       *thread;

    error = FALSE;
    if (handle-> value. type == TYPE_UNDEFINED)
      {
        error = TRUE;
        result-> culprit = handle-> culprit;
      }
    if (min && min-> value. type == TYPE_UNDEFINED)
      {
        error = TRUE;
        result-> culprit = min-> culprit;
      }
    if (timeout && timeout-> value. type == TYPE_UNDEFINED)
      {
        error = TRUE;
        result-> culprit = timeout-> culprit;
      }

    if (start_socket_agent ())
        return -1;

    thread = thread_create (AGENT_NAME, "");
    tcb = thread-> tcb;

    tcb-> gsl_thread = gsl_thread;
    tcb-> result     = result;
    tcb-> buffer     = buffer;
    tcb-> ref_name   = string_value (& buffer-> name-> value);
    tcb-> handle     = 0;

    lsend_smtsock_readh (& sockq-> qid,
                         & thread-> queue-> qid,
                         NULL, NULL, NULL, 0,
                         (dbyte) (timeout ? number_value (&timeout-> value): 0),
                         (qbyte) number_value (& handle-> value),
                         (dbyte) 0xFFFF,                   /*  Max size???  */
                         min ?     (word) number_value (& min-> value)    :0,
                         0);

    return 0;
}


static int
eval_write (int argc, RESULT_NODE **argv,
            RESULT_NODE *result,
            THREAD *gsl_thread)
{
    RESULT_NODE
        *handle  = argc > 0 ? argv [0] : NULL,
        *buffer  = argc > 1 ? argv [1] : NULL,
        *timeout = argc > 2 ? argv [2] : NULL;
    THREAD  
       *thread;
    char
        *data;

    if (! arguments_are_defined (argc, argv, result))
      {
        lsend_ggcode_result (& gsl_thread-> queue-> qid, NULL,
                             NULL, NULL, NULL, 0);
        return 0;
      }

    if (start_socket_agent ())
        return -1;

    thread = thread_create (AGENT_NAME, "");
    tcb = thread-> tcb;

    tcb-> gsl_thread = gsl_thread;
    tcb-> result     = result;
    tcb-> handle     = 0;

    data = string_value (& buffer-> value);

    lsend_smtsock_writeh (& sockq-> qid,
                          & thread-> queue-> qid,
                          NULL, NULL, NULL, 0,
                          (dbyte) (timeout ? number_value (& timeout-> value) : 0),
                          (qbyte) number_value (& handle-> value),
                          (qbyte) strlen (data),
                          (byte *) data,
                          TRUE,
                          0);
    return 0;
}


static int
eval_close (int argc, RESULT_NODE **argv,
            RESULT_NODE *result,
            THREAD *gsl_thread)
{
    RESULT_NODE
        *handle  = argc > 0 ? argv [0] : NULL,
        *timeout = argc > 1 ? argv [1] : NULL;
    THREAD  
       *thread;

    if (! arguments_are_defined (argc, argv, result))
      {
        lsend_ggcode_result (& gsl_thread-> queue-> qid, NULL,
                             NULL, NULL, NULL, 0);
        return 0;
      }

    if (start_socket_agent ())
        return -1;

    thread = thread_create (AGENT_NAME, "");
    tcb = thread-> tcb;

    tcb-> gsl_thread = gsl_thread;
    tcb-> result     = result;
    tcb-> handle     = 0;

    lsend_smtsock_close (& sockq-> qid,
                         & thread-> queue-> qid,
                         NULL, NULL, NULL, 0,
                         (dbyte) (timeout ? number_value (& timeout-> value) : 0),
                         (qbyte) number_value (& handle-> value),
                         TRUE,
                         0);
    return 0;
}


static int
eval_passive (int argc, RESULT_NODE **argv,
              RESULT_NODE *result,
              THREAD *gsl_thread)
{
    RESULT_NODE
        *service = argc > 0 ? argv [0] : NULL;

    if (! arguments_are_defined (argc, argv, result))
      {
        lsend_ggcode_result (& gsl_thread-> queue-> qid, NULL,
                             NULL, NULL, NULL, 0);
        return 0;
      }

    if (start_socket_agent ())
        return -1;

    result-> value. n = passive_TCP (string_value (& service-> value),
                                     QUEUE_LENGTH);

    result-> value. type = TYPE_NUMBER;
    if (result-> value. n == INVALID_SOCKET)
        result-> value. n = -1;

    return 0;
}


static int
eval_accept (int argc, RESULT_NODE **argv,
             RESULT_NODE *result,
             THREAD *gsl_thread)
{
    RESULT_NODE
        *handle  = argc > 0 ? argv [0] : NULL,
        *timeout = argc > 1 ? argv [1] : NULL;
    THREAD  
       *thread;

    if (! arguments_are_defined (argc, argv, result))
      {
        lsend_ggcode_result (& gsl_thread-> queue-> qid, NULL,
                             NULL, NULL, NULL, 0);
        return 0;
      }

    if (start_socket_agent ())
        return -1;

    thread = thread_create (AGENT_NAME, "");
    tcb = thread-> tcb;

    tcb-> gsl_thread = gsl_thread;
    tcb-> result     = result;
    tcb-> handle     = (dword) number_value (& handle-> value);

    lsend_smtsock_input (& sockq-> qid,
                         & thread-> queue-> qid,
                         NULL, NULL, NULL, 0,
                         (dbyte) (timeout ? number_value (& timeout-> value) : 0),
                         tcb-> handle,
                         0);
    return 0;
}


static int
eval_error (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    result-> value. type = TYPE_STRING;
    result-> value. s    = mem_strdup (error_msg);

    return 0;
}


/*************************   INITIALISE THE THREAD   *************************/

MODULE initialise_the_thread (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */
    the_next_event = ok_event;
}


/****************************   REPLY OK RESULT   ****************************/

MODULE reply_ok_result (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result-> value. type = TYPE_NUMBER;
    /*  We rely here on tcb-> handle being the master socket for an accept   */
    /*  and zero for any other request.                                      */
    if (tcb-> handle)
        tcb-> result-> value. n = accept_socket (tcb-> handle);
    else
        tcb-> result-> value. n = 0;

    lsend_ggcode_result (& tcb-> gsl_thread-> queue-> qid, NULL,
                         NULL, NULL, NULL, 0);
}


/**************************   TERMINATE THE THREAD   *************************/

MODULE terminate_the_thread (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    the_next_event = terminate_event;
}


/***************************   REPLY ERROR RESULT   **************************/

MODULE reply_error_result (THREAD *thread)
{
    struct_smtsock_error
        *error_reply;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result-> value. type = TYPE_NUMBER;
    tcb-> result-> value. n    = -1;

    get_smtsock_error (thread-> event-> body, & error_reply);
    strncpy (error_msg, error_reply-> message, ERROR_MSG_LENGTH);
    error_msg [ERROR_MSG_LENGTH] = '\0';
    free_smtsock_error (& error_reply);

    lsend_ggcode_result (& tcb-> gsl_thread-> queue-> qid, NULL,
                         NULL, NULL, NULL, 0);
}


/**************************   REPLY CLOSED RESULT   **************************/

MODULE reply_closed_result (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result-> value. type = TYPE_NUMBER;
    tcb-> result-> value. n    = 1;

    lsend_ggcode_result (& tcb-> gsl_thread-> queue-> qid, NULL,
                         NULL, NULL, NULL, 0);
}


/**************************   REPLY TIMEOUT RESULT   *************************/

MODULE reply_timeout_result (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result-> value. type = TYPE_NUMBER;
    tcb-> result-> value. n    = 2;

    lsend_ggcode_result (& tcb-> gsl_thread-> queue-> qid, NULL,
                         NULL, NULL, NULL, 0);
}


/************************   REPLY CONNECT OK RESULT   ************************/

MODULE reply_connect_ok_result (THREAD *thread)
{
    struct_smtsock_connect_ok
        *connect_reply;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_smtsock_connect_ok (thread-> event-> body, & connect_reply);
    tcb-> result-> value. type = TYPE_NUMBER;
    tcb-> result-> value. n    = connect_reply-> socket;
    free_smtsock_connect_ok (& connect_reply);

    lsend_ggcode_result (& tcb-> gsl_thread-> queue-> qid, NULL,
                         NULL, NULL, NULL, 0);
}


/*************************   REPLY READH OK RESULT   *************************/

MODULE reply_readh_ok_result (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    reply_readh_result (thread-> event-> body, 0);
}


static void
reply_readh_result (byte *body, int rc)
{
    struct_smtsock_readh_reply
        *readh_reply;
    char
        *data,
        *error_text;
    GGCODE_TCB
        *gsl_tcb = tcb-> gsl_thread-> tcb;
    XML_ITEM
        *xml_father;

    xml_father = symbol_xml_parent (& gsl_tcb-> scope_stack,
                                    gsl_tcb-> result_node,
                                    gsl_tcb-> ignorecase,
                                    &error_text);
    if (! xml_father)
      {
        tcb-> result-> value. type = TYPE_NUMBER;
        tcb-> result-> value. n    = -1;

        strncpy (error_msg, error_text, ERROR_MSG_LENGTH);
        error_msg [ERROR_MSG_LENGTH] = '\0';
      }
    else
      {
        get_smtsock_readh_reply (body, & readh_reply);
        tcb-> result-> value. type = TYPE_NUMBER;
        tcb-> result-> value. n    = rc;
        data = mem_alloc (readh_reply-> size + 1);
        memcpy (data, readh_reply-> data, readh_reply-> size);
        data [readh_reply-> size] = '\0';
        xml_put_attr (xml_father, 
                      tcb-> ref_name,
                      data);
        mem_free (data);
        free_smtsock_readh_reply (& readh_reply);
      }
    lsend_ggcode_result (& tcb-> gsl_thread-> queue-> qid, NULL,
                         NULL, NULL, NULL, 0);
}


/***********************   REPLY READH CLOSED RESULT   ***********************/

MODULE reply_readh_closed_result (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    reply_readh_result (thread-> event-> body, 1);
}


/***********************   REPLY READH TIMEOUT RESULT   **********************/

MODULE reply_readh_timeout_result (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    reply_readh_result (thread-> event-> body, 2);
}

