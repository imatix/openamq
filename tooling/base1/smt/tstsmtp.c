/*===========================================================================*
 *                                                                           *
 *  tstsmtp.c - Test SMTP agent                                              *
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

#include "smtpriv.h"                    /*  SMT definitions                  */


/*- Definitions -------------------------------------------------------------*/

#define AGENT_NAME   "tstsmtp"          /*  Our public name                  */

#define BUF_SIZE    (30)

typedef struct                          /*  Thread context block:            */
{
    int handle;                         /*    Handle for i/o                 */
    int id;
} TCB;



/*- Function prototypes -----------------------------------------------------*/

/*- Global variables used in this source file only --------------------------*/

static TCB
    *tcb;                               /*  Address thread context block     */

static QID
    smtp,                               /*  smtp event queue     */
    timer;

char *bin_name = NULL;
char
  *main_server = NULL,
  *main_sender = NULL,
  *main_body   = NULL,
  *main_dest   = NULL;



#include "tstsmtp.d"                    /*  Include dialog data              */

/********************   INITIALISE AGENT - ENTRY POINT   *********************/

int tstsmtp_init (void)
{
    AGENT *agent;                       /*  Handle for our agent             */
    THREAD  *thread;                    /*  Handle to console thread         */
#   include "tstsmtp.i"                 /*  Include dialog interpreter       */

    /*  Change any of the agent properties that you need to                  */
    agent-> router      = FALSE;        /*  FALSE = default                  */
    agent-> max_threads = 0;            /*  0 = default                      */


    /*                      Method name     Event value      Priority        */
    /*  Shutdown event comes from Kernel                                     */
    declare_smtlib_shutdown   (shutdown_event,    SMT_PRIORITY_MAX);

    /*  Public methods supported by this agent                               */
    declare_smtsmtp_ok    (ok_event, 0);
    declare_smtsmtp_error (error_event, 0);

    /*  Ensure that smtp is running, else start it up                       */
    if (agent_lookup (SMT_SMTP) == NULL)
        smtsmtp_init ();
    if ((thread = thread_lookup (SMT_SMTP, "")) != NULL)
        smtp = thread-> queue-> qid;
    else
        return (-1);

    /*  Ensure that timer agent is running, else start it up                 */
    smttime_init ();
    if ((thread = thread_lookup (SMT_TIMER, "")) != NULL)
        timer = thread-> queue-> qid;
    else
        return (-1);

    /*  Signal okay to caller that we initialised okay                       */
    return (0);
}


/*************************   INITIALISE THE THREAD   *************************/

MODULE initialise_the_thread (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */
    tcb-> handle = 0;                   /*  For example...                   */
    tcb-> id = 0;
    the_next_event = ok_event;
}


/****************************   SEND MSG TO SMTP   ***************************/


static
char *generate_body (word size);

MODULE send_msg_to_smtp (THREAD *thread)
{
    extern char
        *main_server,
        *main_sender,
        *main_dest,
        *main_body;

    if (main_body == NULL)
      {
        main_body = generate_body (300);
        ASSERT (main_body);
      }

    send_smtsmtp_send_message (
        &smtp,
        main_server,
        main_body,
        main_sender,
        main_dest,
        "ze sibjekt"
      );
}


static
char *generate_body (word length)
{
    char
        *body = NULL;
    dword
        size,
        idx;
    static char
        now_str[BUF_SIZE];
    static char
        date_str[BUF_SIZE];
    int
        rc;

    randomize ();

    ASSERT (length > 100);              /* precondition */

    body = mem_alloc (length+1);

    rc = snprintf (now_str, BUF_SIZE, "%s", conv_time_pict (time_now(), "hh:mm"));
    ASSERT (rc > 0);
    rc = snprintf (date_str, BUF_SIZE, "%s", conv_date_pict (date_now(), "yymmdd"));
    ASSERT (rc > 0);

    rc = snprintf (body, length, "automatic body generated %s-%s\n",date_str, now_str);
    ASSERT (rc > 0);

    size = strlen (body);

    coprintf ("Size [%i] - Length [%i]", size, length);
    for (idx=size; idx<length; idx++)
        body[idx] = 'A' + random(26);

    coprintf ("idx : %i", idx);
    body[idx] = 0;

    return body;
}



/*************************   TERMINATE THE THREAD   **************************/

MODULE terminate_the_thread (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */
    the_next_event = terminate_event;
}


/*******************************   DISPLAY OK   ******************************/

MODULE display_ok (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    coprintf ("the mail was successfully sent");

    the_next_event = ok_event;
}


/*****************************   DISPLAY ERROR   *****************************/

MODULE display_error (THREAD *thread)
{
    struct_smtsmtp_reply *smtp_reply;

    get_smtsmtp_reply (thread-> event-> body, &smtp_reply);
    coprintf ("SMPT ERROR (%i) : %s", smtp_reply-> code, smtp_reply-> msg);
    free_smtsmtp_reply (&smtp_reply);

    the_next_event = ok_event;
}


/********************************   KILL ALL   *******************************/

MODULE kill_all (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    smt_shutdown();
}




void display_usage (void);
Bool load_config (const char *filename);
Bool load_body   (const char *filename);
int smtptst_init (void);

int main (int argc, char *argv [])
{
    bin_name = strip_file_path (argv[0]);

    if (argc < 2 || argc > 3)
      {
         display_usage ();
         return -1;
      }

    if (! load_config (argv[1]) )
        return -1;

    if (argc == 3)
      {
        if (!load_body (argv[2]))
          {
            coprintf ("bad body file");
            return -1;
          }
      }

    coprintf (
        "Sending one mail from [%s] to [%s] via [%s]",
        main_sender,
        main_dest,
        main_server
      );

    smt_init ();                   /*  Initialise SMT kernel    */
    /*  Application is latent - initialise it                   */
    if (agent_lookup (SMT_SOCKET) == NULL)
        sock_init ();
    if (agent_lookup (AGENT_NAME) == NULL)
        tstsmtp_init ();

    thread_create (AGENT_NAME, "");

    /*  Application is active - execute it                      */
    smt_exec_full ();              /*  Run until completed      */

    /*  Application is halted - terminate it                    */
    smt_term ();                   /*  Shut-down SMT kernel     */
    sock_term ();

    mem_free (main_server);
    mem_free (main_sender);
    mem_free (main_dest);
    mem_free (main_body);

    printf ("Allocs=%ld frees=%ld\n", mem_allocs (), mem_frees ());
    mem_assert ();

    return (0);
}


void display_usage (void)
{
    const char *usage =
"\n"
"USAGE:\n"
"\n"
"    %s tst_cfg [body]\n"
"\n"
"where:\n"
"  tst_cfg is an XML file:\n"
"    <test_config (smtp, sender, dest) />\n"
"    with smtp, sender and dest as XML attributes\n"
"  body is a file with body to send. if no file, body is generated\n"
"     '.txt' extension is assumed if no extension\n";

    coprintf (usage, bin_name);
}

Bool load_config (const char *filename)
{
    int rc;
    XML_ITEM *config = NULL;
    XML_ITEM *root   = NULL;
    char fullname[FILE_NAME_MAX + 1];

    root = xml_create ("root", NULL);

    ASSERT (filename != NULL);
    ASSERT (strlen(filename) < FILE_NAME_MAX);
    default_extension (fullname, filename, "cfg");

    rc = xml_load_file (&root, ".", fullname, FALSE);

    if (rc != XML_NOERROR)
      {
        coprintf ("Error while loading \"%s\". Check file presence and consistence",
                   fullname);
        display_usage ();
        return FALSE;;
      }

    config = xml_first_child (root);
    ASSERT (config);

    /* default server is localhost */
    main_server =   mem_strdup (xml_get_attr (config, "smtp",    "127.0.0.1"));
    main_sender =   mem_strdup (xml_get_attr (config, "sender",  "admin@crjo"));
    main_dest   =   mem_strdup (xml_get_attr (config, "dest",    "user@crjo"));

    xml_free (root);

    return TRUE;
}

Bool load_body (const char *filename)
{
    FILE *file = NULL;
    char *fullname = NULL;
    size_t bytes_read, size;

    ASSERT (filename);

    fullname = mem_strdup (file_where ('s', NULL, filename, "txt"));
    ASSERT (fullname);


    file = file_locate (NULL, filename, "txt");
    if (!file)
      {
        coprintf ("ERROR : unable to open file %s", fullname);
        mem_free (fullname);
        return FALSE;
      }


    size = get_file_size (fullname);

    main_body = mem_alloc (size+1);
    ASSERT (main_body);

    bytes_read = fread (main_body, 1, size, file);
    ASSERT (bytes_read == size);

    main_body[size] = 0;

    mem_free (fullname);
    return TRUE;
}





