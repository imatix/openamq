/*===========================================================================*
 *                                                                           *
 *  tstpop.c - Test POP3 agent                                               *
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

#define AGENT_NAME   "tstpop"           /*  Our public name                  */

typedef struct                          /*  Thread context block:            */
{
    QID handle;                         /*    Handle for i/o                 */
    int
        message_count;
    qbyte
        total_size;
} TCB;


/*- Function prototypes -----------------------------------------------------*/


/*- Global variables used in this source file only --------------------------*/

static TCB
    *tcb;                               /*  Address thread context block     */

static QID
    pop3;

char
  *main_server = NULL,
  *main_user   = NULL,
  *main_password = NULL;

char *bin_name = NULL;


#include "tstpop.d"                    /*  Include dialog data              */

/********************   INITIALISE AGENT - ENTRY POINT   *********************/

int tstpop_init (void)
{
    AGENT  *agent;                      /*  Handle for our agent             */
    THREAD *thread;
#   include "tstpop.i"                  /*  Include dialog interpreter       */

    /*  Change any of the agent properties that you need to                  */
    agent-> router      = FALSE;        /*  FALSE = default                  */
    agent-> max_threads = 0;            /*  0 = default                      */

    /*                      Method name     Event value      Priority        */
    /*  Shutdown event comes from Kernel                                     */
    declare_smtlib_shutdown   (shutdown_event,    SMT_PRIORITY_MAX);

    declare_smtpop_connect_ok       (connect_ok_event,       0);
    declare_smtpop_error            (pop_error_event,        0);
    declare_smtpop_session_info     (info_response_event,    0);
    declare_smtpop_msg              (msg_event,              0);
    declare_smtpop_msg_header       (hdr_event,              0);
    declare_smtpop_quit_ok          (end_event,              0);
    declare_smtpop_delete_ok        (del_ok_event,           0);
    declare_smtpop_msg_session_info (msg_info_request_event, 0);

    if (agent_lookup (SMT_POP) == NULL)
        smtpop_init ();
    if ((thread = thread_lookup (SMT_POP, "")) != NULL)
        pop3 = thread-> queue-> qid;
    else
        return (-1);

    /*  Signal okay to caller that we initialised okay                       */
    return (0);
}


/*************************   INITIALISE THE THREAD   *************************/

MODULE initialise_the_thread (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */
    the_next_event = ok_event;
}


/*************************   TERMINATE THE THREAD   **************************/

MODULE terminate_the_thread (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */
    the_next_event = terminate_event;
}



/********************************   CONNECT   ********************************/

MODULE do_connect (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    coprintf ("connection request for %s [%s]...", main_user, main_server);

    send_smtpop_connect(
        &pop3,
        main_server,
        main_user,
        main_password
      );
}


/*******************************   GET HANDLE   ******************************/

MODULE get_handle (THREAD *thread)
{
    struct_smtpop_connect_ok
        *connect_params = NULL;

    tcb = thread-> tcb;
    get_smtpop_connect_ok  (thread-> event-> body,&connect_params);

    tcb-> handle        = thread-> event-> sender;
    tcb-> message_count = connect_params-> msg_cnt;
    tcb-> total_size    = connect_params-> msg_size;

    free_smtpop_connect_ok  (&connect_params);
}


/*************************   WAIT USER TRANSACTION   *************************/
static char buf[80];
static char a_word[50];

MODULE get_and_send_user_request (THREAD *thread)
{
    char *ch;
    char *ptr;
    int id;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    coprintf ("Enter request : ");
    ch = fgets (buf, sizeof(buf), stdin);

    while (*ch != 0)
      {
        if (*ch == '\r' || *ch == '\n')
          {
            /* we close the string, fgets returns an extra unwanted CR LF */
            *ch = 0;
            break;
          }
        *ch = tolower(*ch);
        ch++;
      }

    getstrfld (buf, 0, 0, " ", a_word);

    if (streq (a_word, "quit"))
        send_smtpop_quit (&tcb-> handle);

    else
    if (streq (a_word, "info"))
      {
        ptr = getstrfld (buf, 1, 0, " ", a_word);

        if (ptr == NULL || !atoi(ptr))
            send_smtpop_get_session_info(&tcb-> handle);
        else
          {
            id = atoi (a_word);
            if (id < 0 )
              {
                raise_exception (help_event);
                return;
              }
            send_smtpop_get_msg_info (
                &tcb-> handle,
                (word)id,
                NULL
              );
          }
      }

    else
    if (streq (a_word, "get"))
      {
        ptr = getstrfld (buf, 1, 0, " ", a_word);

        if (ptr == NULL)
          {
            raise_exception (help_event);
            return ;
          }

        id = atoi (a_word);
        if (id < 0 )
          {
            raise_exception (help_event);
            return;
          }

        send_smtpop_get_msg(&tcb-> handle, (word)id, "att_dir");
      }

    else
    if (streq (a_word, "hdr"))
      {
        ptr = getstrfld (buf, 1, 0, " ", a_word);

        if (ptr == NULL)
          {
            raise_exception (help_event);
            return ;
          }

        id = atoi (a_word);
        if (id < 0 )
          {
            raise_exception (help_event);
            return;
          }

        send_smtpop_get_msg_header(&tcb-> handle, (word)id, "att_dir");
      }

    else
    if (streq (a_word, "del"))
      {
        ptr = getstrfld (buf, 1, 0, " ", a_word);

        if (ptr == NULL)
          {
            raise_exception (help_event);
            return ;
          }

        id = atoi (a_word);
        if (id < 0 )
          {
            raise_exception (help_event);
            return;
          }

        send_smtpop_delete_msg (&tcb-> handle, (word)id, NULL);
      }
    else
      {
        if ( strneq (a_word, "?") && strneq (a_word, "help") )
            coprintf ("Bad request");
        raise_exception (help_event);
      }
}

/****************** DISPLAY MSG COUNT AND SIZE *******************************/
MODULE display_msg_count_and_size (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    coprintf ("CONNECTION ESTABLISHED : %2i messages - %8li bytes",
              tcb-> message_count,
              tcb-> total_size);
}


/*****************************   DISPLAY USAGE   *****************************/

MODULE display_usage (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    coprintf (
        "USAGE : "
        "\n"
        "  info                  - message count and size\n"
        "  info id               - message size\n"
        "  help                  - this message\n"
        "  hdr id                - get message header\n"
        "  get id                - get message\n"
        "  del id                - delete message from server\n"
        "  quit                  - just try to guess...\n"
        "\n"
        "NOTE:"
        "  Message are REALLY deleted from server when quit is performed.\n"
        "  If application crashes before explicit 'quit' command, messages are\n"
        "  kept on the server"
      );
}


/******************************   DISPLAY INFO   *****************************/

MODULE display_info (THREAD *thread)
{
    struct_smtpop_session_info *info = NULL;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_smtpop_session_info  (thread-> event-> body, &info);

    coprintf ("Session information : %i messages - %i bytes", info-> count, info-> size);

    free_smtpop_session_info  (&info);
}


/****************************   DISPLAY MESSAGE   ****************************/

MODULE display_message (THREAD *thread)
{
    struct_smtpop_msg *params;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_smtpop_msg        (thread-> event-> body, &params);

    coprintf ("Id....................%i", params-> msg_id);
    coprintf ("From..................%s", params-> from ? params-> from : "NULL");
    coprintf ("To....................%s", params-> to ? params-> to : "NULL");
    coprintf ("Cc....................%s", params-> cc ? params-> cc : "NULL");
    coprintf ("Date..................%s", params-> date ? params-> date : "NULL");
    coprintf ("Subject...............%s", params-> subject ? params-> subject : "NULL");
    coprintf ("Attach................%s", params-> attachments ? params-> attachments : "NULL");
    coprintf ("Attach location.......%s", params-> attach_dir ? params-> attach_dir : "NULL");
    coprintf ("[BODY]%s[END_BODY]"      , params-> body ? params-> body : "NULL");

    free_smtpop_msg       (&params);
}


/*******************************   KILL POP3   *******************************/

MODULE kill_pop3 (THREAD *thread)
{
    smt_shutdown();
}


/*****************************   DISPLAY HEADER   ****************************/

MODULE display_header (THREAD *thread)
{
    struct_smtpop_msg_header *params;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    coprintf ("Header gotten !");
    get_smtpop_msg_header (thread-> event-> body, &params);

    coprintf ("Id....................%i", params-> msg_id);
    coprintf ("From..................%s", params-> from ? params-> from : "NULL");
    coprintf ("To....................%s", params-> to ? params-> to : "NULL");
    coprintf ("Cc....................%s", params-> cc ? params-> cc : "NULL");
    coprintf ("Date..................%s", params-> date ? params-> date : "NULL");
    coprintf ("Subject...............%s", params-> subject ? params-> subject : "NULL");

    free_smtpop_msg_header (&params);
}


/*****************************   DISPLAY DEL OK   ****************************/

MODULE display_del_ok (THREAD *thread)
{
    struct_smtpop_msg_id *params;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_smtpop_msg_id (thread-> event-> body, &params);

    coprintf ("Message [%i] successfully deleted", params-> msg_id);

    free_smtpop_msg_id (&params);
}


/*****************************   DISPLAY BAD ID   ****************************/

MODULE display_bad_id (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    coprintf ("Error : bad message id");
}


/*****************************   DISPLAY ERROR   *****************************/

MODULE display_error (THREAD *thread)
{
    struct_smtpop_error *params;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_smtpop_error (thread-> event-> body, &params);

    coprintf ("Error: [%s]", params-> reason);

    free_smtpop_error (&params);
}


/**************************   DISPLAY MESSAGE INFO   *************************/

MODULE display_message_info (THREAD *thread)
{
    struct_smtpop_msg_session_info *params = NULL;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_smtpop_msg_session_info (thread-> event-> body, &params);

    coprintf ("msg %i : %i bytes (body + headers)", params-> msg_id, params-> size);

    free_smtpop_msg_session_info (&params);
}


/* ***************************** MAIN ****************************************/


#define TRACE_VAR(_VAR, _CS)          \
       printf ("%s(%i) - %-10s : [%"#_CS"]\n", __FILE__, __LINE__, #_VAR, _VAR)

void display_command_line_usage (void);
Bool load_config (const char *filename);

int main (int argc, char *argv [])
{
    int rc;

    console_capture     ("pop.log", 'w');

    bin_name = strip_file_path (argv[0]);

    if (argc != 2)
      {
         display_command_line_usage ();
         return -1;
      }

    if (! load_config (argv[1]) )
      return -1;

    TRACE_VAR (main_server, s);    
    TRACE_VAR (main_user, s);    
    TRACE_VAR (main_password, s);

    /*  Application is latent - initialise it                   */
    smt_init ();                   /*  Initialise SMT kernel    */
    rc = tstpop_init ();

    thread_create (AGENT_NAME, "");
    console_set_mode (CONSOLE_PLAIN);

    /*  Application is active - execute it                      */
    smt_exec_full ();              /*  Run until completed      */

    /*  Application is halted - terminate it                    */
    smt_term ();                   /*  Shut-down SMT kernel     */

    mem_free (main_user);
    mem_free (main_server);
    mem_free (main_password);

    coprintf ("Allocs=%ld frees=%ld\n", mem_allocs (), mem_frees ());
    mem_assert ();

    return (0);
}


void display_command_line_usage (void)
{
    const char *usage =
"\n"
"USAGE:\n"
"\n"
"    %s tst_cfg\n"
"\n"
"where tst_cfg is an XML file:\n"
"    <server_config (pop3, user, password) />\n"
"with pop3, user and password as XML attributes\n";

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
        display_command_line_usage ();
        return FALSE;;
      }

    config = xml_first_child (root);
    ASSERT (config);

    /* default server is localhost */
    main_server =   mem_strdup (xml_get_attr (config, "pop3",    "127.0.0.1"));
    main_user   =   mem_strdup (xml_get_attr (config, "user",    "admin"));
    /* username is assumed as password by default */
    main_password = mem_strdup (xml_get_attr (config, "password", main_user));

    xml_free (root);

    return TRUE;
}





