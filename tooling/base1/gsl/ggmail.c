/*===========================================================================*
 *                                                                           *
 *  ggmail.c - Mail object functions                                         *
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
#include "ggmail.h"                     /*  Include header file              */
#include "ggmail.d"                     /*  Include dialog data              */


/*- Definitions -------------------------------------------------------------*/

#define AGENT_NAME      "GGMAIL"        /*  Our public name                  */
#define NAME "mail"

/*- Function prototypes -----------------------------------------------------*/

static EVAL_FUNCTION
    eval_error,
    eval_send;

/*- Type definitions --------------------------------------------------------*/

typedef struct {
    QUEUE
        *replyqueue;
    RESULT_NODE
        *result;
} TCB;

/*- Global variables used in this source file only --------------------------*/

static TCB
    *tcb;                               /*  Address thread context block     */

static QUEUE
    *smtpq;

static PARM_LIST
    one_value_parm_list = {PARM_VALUE};

static GSL_FUNCTION mail_functions [] = 
{
    /*  Don't forget to keep these in alphaetical order!                     */
    {"error", 0, 0, 0, NULL,                          TRUE,  eval_error},
    {"send",  5, 5, 1, (void *) &one_value_parm_list, FALSE, eval_send}
};

#define ERROR_MSG_LENGTH 80

static char
    error_msg [ERROR_MSG_LENGTH + 1];

/*- Functions ---------------------------------------------------------------*/

int register_mail_functions (void)
{
    AGENT   *agent;                     /*  Handle for our agent             */
#   include "ggmail.i"                  /*  Include dialog interpreter       */

    declare_smtlib_shutdown   (shutdown_event, SMT_PRIORITY_MAX);

    declare_smtsmtp_ok    (ok_event,    0);
    declare_smtsmtp_error (error_event, 0);

    error_msg [0] = '\0';
    smtpq         = NULL;

    return object_register (NAME, mail_functions, tblsize (mail_functions), NULL);
}


static int
eval_error (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    result-> value. type = TYPE_STRING;
    result-> value. s    = mem_strdup (error_msg);

    return 0;
}


static int
eval_send (int argc, RESULT_NODE **argv,
           RESULT_NODE *result,
           THREAD *gsl_thread)
{
    RESULT_NODE
        *server    = argc > 0 ? argv [0] : NULL,
        *body      = argc > 1 ? argv [1] : NULL,
        *sender    = argc > 2 ? argv [2] : NULL,
        *recipient = argc > 3 ? argv [3] : NULL,
        *subject   = argc > 4 ? argv [4] : NULL;
    THREAD  
       *thread;

    if (! arguments_are_defined (argc, argv, result))
      {
        lsend_ggcode_result (& gsl_thread-> queue-> qid, NULL,
                             NULL, NULL, NULL, 0);
        return 0;
      }
    if (! smtpq)
      {
        if (agent_lookup (SMT_SMTP) == NULL)
            smtsmtp_init ();
        if ( (thread = thread_lookup (SMT_SMTP, "master")) != NULL )
            smtpq = thread-> queue;
        else
          {
            strcpy (object_error, "Unable to start SMTP agent.");
            return -1;
          }
      }

    thread = thread_create (AGENT_NAME, "");
    tcb = thread-> tcb;

    tcb-> replyqueue = gsl_thread-> queue;
    tcb-> result     = result;

    lsend_smtsmtp_send_message (& smtpq-> qid,
                                & thread-> queue-> qid,
                                NULL, NULL, NULL, 0,
                                string_value (& server-> value),
                                string_value (& body-> value),
                                string_value (& sender-> value),
                                string_value (& recipient-> value),
                                string_value (& subject-> value));
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
    tcb-> result-> value. n    = 0;

    lsend_ggcode_result (& tcb-> replyqueue-> qid, NULL,
                         NULL, NULL, NULL, 0);
}


/***************************   REPLY ERROR RESULT   **************************/

MODULE reply_error_result (THREAD *thread)
{
    struct_smtsmtp_reply
        *error_reply;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_smtsmtp_reply (thread-> event-> body, & error_reply);
    tcb-> result-> value. type = TYPE_NUMBER;
    tcb-> result-> value. n    = (long) error_reply-> code;
    strncpy (error_msg, error_reply-> msg, ERROR_MSG_LENGTH);
    error_msg [ERROR_MSG_LENGTH] = '\0';
    free_smtsmtp_reply (& error_reply);

    lsend_ggcode_result (& tcb-> replyqueue-> qid, NULL,
                         NULL, NULL, NULL, 0);
}


/**************************   TERMINATE THE THREAD   *************************/

MODULE terminate_the_thread (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    the_next_event = terminate_event;
}

