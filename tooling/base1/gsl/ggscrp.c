/*===========================================================================*
 *                                                                           *
 *  ggscrp.c - Script execution functions                                    *
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
#include "ggscrp.d"                     /*  Include dialog data              */


/*- Type definitions --------------------------------------------------------*/

typedef struct {
    SCRIPT_NODE_TYPE type;
    Bool             template;
    SCRIPT_LINE     *script_line;
} CONTROL;


typedef struct
{
    void 
       *job;
    QID
        reply_qid;
    SEMAPH
       *semaph;
    SCRIPT_READ
       *script_read;
    SCRIPT_NODE
       *script_node;
    SCRIPT_SOURCE
       *script_source;
    SCRIPT_LINE
       *script_line;
    FILE
       *script_file;
    char
        *name,
        *text,
        *script_ptr,
        *line_text;
    Bool
        template,
        debug,
        end_of_file;
    word
        line_nbr,
        start_line;
    LIST
        control_stack;
    CONTROL
        control;
    SCRIPT_LINE
       *loop_start;
} TCB;


/*- Prototypes --------------------------------------------------------------*/

static void initialise_tcb            (TCB *tcb);
static Bool free_excess_scripts       (long size);
static void destroy_script_data       (SCRIPT_SOURCE *script_source);
static void create_script_source_node (THREAD *thread);
static Bool file_script_read          (void *job, char *text);
static Bool text_script_read          (void *job, char *text);
static void log_error                 (THREAD *thread, char *format, ...);
static void push_control              (SCRIPT_NODE_TYPE type,
                                       Bool template,
                                       SCRIPT_LINE *script_line);
static void pop_control               (THREAD *thread, SCRIPT_NODE_TYPE type);


/*- Definitions -------------------------------------------------------------*/

#define AGENT_NAME      "GGSCRP"        /*  Our public name                  */


/*- Global variables used in this source file only --------------------------*/

static TCB
    *tcb;                               /*  Address thread context block     */

static long
    max_size,                           /*  Threshold before freeing space   */
    total_size;                         /*  Total space taken by parse trees */

/*- Global variables --------------------------------------------------------*/

LIST
    script_list;

/*- Script Functions --------------------------------------------------------*/

int
load_script_file (char  *name, 
                  Bool   template,
                  Bool   debug,
                  Bool   keep,
                  void  *job,
                  QUEUE *replyqueue)
{
    SCRIPT_SOURCE
       *script_source;
    char
        *fname;
    THREAD  
       *thread;
    SEMAPH
       *semaph = NULL;

    /*  If the script is already loaded, don't bother starting a thread,     */
    /*  just send the OK event.                                              */

    FORLIST (script_source, script_list)
        if (streq (script_source-> name, name)
        &&  (script_source-> links || script_source-> keep))
          {
            semaph = semaph_lookup (name);
            if (! semaph)
              {
                fname = file_where ('r', PATH, name, "gsl");
                if (fname)
                  {
                    if (script_source-> timestamp == get_file_time (fname))
                      {
                        script_source-> links++;
                        lsend_ggscrp_done (& replyqueue-> qid,
                                           NULL,
                                           NULL, NULL, NULL, 0,
                                           job,
                                           script_source);
                        return 0;
                      }
                  }
                else
                  {
                    /*  File has gone  */
                    if (script_source-> links == 0)
                        destroy_script_data (script_source);
                    else
                        /*  Flag for destruction when other threads are done */
                        script_source-> keep = FALSE;
                  }
              }
            break;
          }

    free_excess_scripts (max_size);
    thread = thread_create (AGENT_NAME, "");
    if (! thread)
        return -1;

    initialise_tcb ((TCB *) thread-> tcb);

    ((TCB *) thread-> tcb)-> job         = job;
    ((TCB *) thread-> tcb)-> reply_qid   = replyqueue-> qid;

    if (semaph)
      {
        ((TCB *) thread-> tcb)-> script_source = script_source;
        ((TCB *) thread-> tcb)-> semaph        = semaph;
        lsend_ggscrp_wait (& thread-> queue-> qid,
                           & replyqueue-> qid,
                           NULL, NULL, NULL, 0);

        return 0;
      }
    
    /*  Declare semaphore for this script file.  */
    semaph = semaph_create (name, 1);
    /*  This should never happen, but clean up anyway                    */
    if (! semaph)
      {
        thread_destroy (thread, FALSE);
        return -1;
      }

    ((TCB *) thread-> tcb)-> semaph      = semaph;
    ((TCB *) thread-> tcb)-> script_read = file_script_read;
    ((TCB *) thread-> tcb)-> template    = template;
    ((TCB *) thread-> tcb)-> debug       = debug;
    ((TCB *) thread-> tcb)-> name        = memt_strdup (thread-> memtrn, name);

    create_script_source_node (thread);

    tcb-> script_source-> keep = keep;

    lsend_ggscrp_file (& thread-> queue-> qid,
                       & replyqueue-> qid,
                       NULL, NULL, NULL, 0);

    return 0;
}


static void
initialise_tcb (TCB *tcb)
{
    tcb-> semaph               = NULL;
    tcb-> script_read          = NULL;
    tcb-> script_node          = NULL;
    tcb-> script_source        = NULL;
    tcb-> script_line          = NULL;
    tcb-> script_file          = NULL;
    tcb-> name                 = NULL;
    tcb-> text                 = NULL;
    tcb-> script_ptr           = NULL;
    tcb-> line_text            = NULL;
    tcb-> end_of_file          = FALSE;
    tcb-> line_nbr             = 0;
    tcb-> start_line           = 0;
    tcb-> control.type         = GG_UNDEFINED;
    tcb-> control. script_line = NULL;
    tcb-> loop_start           = NULL;

    list_reset (& tcb-> control_stack);
}


static Bool
free_excess_scripts (long size)
{
    SCRIPT_SOURCE
        *script_source,
        *oldest_source;

    while (total_size > size)
      {
        oldest_source = NULL;
        FORLIST (script_source, script_list)
            if ((script_source-> links <= 0) /*  ie not currently in use     */
            &&  (! script_source-> macros)   /*  and doesn't contain macros  */
            && ((! oldest_source)
            ||  (script_source-> last_closed < oldest_source-> last_closed)))
                oldest_source = script_source;

        if (oldest_source)
            destroy_script_data (oldest_source);
        else
            return FALSE;               /*  Unsuccessful                     */
      }
    return TRUE;                        /*  Successful                       */
}


static void
create_script_source_node (THREAD *thread)
{
    MEMTRN
       *memtrn;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    memtrn = mem_new_trans ();

    tcb-> script_source = memt_alloc (memtrn, sizeof (SCRIPT_SOURCE));
    ASSERT (tcb-> script_source);
    list_reset (tcb-> script_source);

    tcb-> script_source-> memtrn      = memtrn;
    tcb-> script_source-> name        = memt_strdup (memtrn, 
                                                     tcb-> name);
    tcb-> script_source-> keep        = FALSE;
    tcb-> script_source-> size        = 0;
    tcb-> script_source-> links       = 1;
    tcb-> script_source-> macros      = 0;
    tcb-> script_source-> last_closed = 0;
    tcb-> script_source-> timestamp   = 0;

    list_reset (& tcb-> script_source-> line_head);
    list_relink_after (tcb-> script_source, & script_list);
}


static Bool
file_script_read (void *job, char *text)
{
    THREAD
        *thread;
    int
        length;

    thread = job;
    tcb = thread-> tcb;

    ASSERT (job);
    ASSERT (text);

    if (! tcb-> script_file || ! file_read (tcb-> script_file, text))
        return FALSE;

    tcb-> line_nbr++;
    if (tcb-> debug)
      {
        if (tcb-> line_text)
          {
            length = strlen (tcb-> line_text);
            tcb-> line_text = mem_realloc (tcb-> line_text, 
                                           length + strlen (text) + 2);
            tcb-> line_text [length] = '\n';
            strcpy (tcb-> line_text + length + 1, text);
          }
        else
            tcb-> line_text = memt_strdup (thread-> memtrn, text);
      }
    return TRUE;
}


int
load_script_text (char  *name, 
                  char  *text, 
                  Bool   template,
                  Bool   debug,
                  void  *job,
                  QUEUE *replyqueue)
{
    THREAD  
       *thread;

    free_excess_scripts (max_size);
    thread = thread_create (AGENT_NAME, "");
    if (! thread)
        return -1;

    initialise_tcb ((TCB *) thread-> tcb);

    ((TCB *) thread-> tcb)-> job         = job;
    ((TCB *) thread-> tcb)-> reply_qid   = replyqueue-> qid;
    ((TCB *) thread-> tcb)-> script_read = text_script_read;
    ((TCB *) thread-> tcb)-> template    = template;
    ((TCB *) thread-> tcb)-> debug       = debug;
    ((TCB *) thread-> tcb)-> name        = memt_strdup (thread-> memtrn, name);
    ((TCB *) thread-> tcb)-> text        = memt_strdup (thread-> memtrn, text);

    create_script_source_node (thread);

    lsend_ggscrp_text (& thread-> queue-> qid,
                       & replyqueue-> qid,
                       NULL, NULL, NULL, 0);

    return 0;
}


static Bool
text_script_read (void *job, char *text)
{
    THREAD
        *thread;
    char
       *endptr;
    int
        len;
    int
        length;

    thread = job;
    tcb = thread-> tcb;

    /* Handle blank line separately - strtok doesn't like empty tokens */
    if (! *tcb-> script_ptr)
        return FALSE;

    endptr = strchr (tcb-> script_ptr, '\n');
    if (endptr)
        len = endptr - tcb-> script_ptr;
    else
        len = strlen (tcb-> script_ptr);

    if (len > LINE_MAX)
        len = LINE_MAX;

    strncpy (text, tcb-> script_ptr, len);
    text [len] = '\0';

    if (endptr)
        tcb-> script_ptr = endptr + 1;
    else
        tcb-> script_ptr += len;

    tcb-> line_nbr++;
    if (tcb-> debug)
      {
        if (tcb-> line_text)
          {
            length = strlen (tcb-> line_text);
            tcb-> line_text = mem_realloc (tcb-> line_text, 
                                           length + strlen (text) + 2);
            tcb-> line_text [length] = '\n';
            strcpy (tcb-> line_text + length + 1, text);
          }
        else
            tcb-> line_text = memt_strdup (thread-> memtrn, text);
      }
    return TRUE;
}


void
close_script (SCRIPT_SOURCE *script_source)
{
    if (script_source)
      {
        script_source-> links--;
        if (script_source-> links == 0
        && !script_source-> keep)
            destroy_script_data (script_source);
        else
            script_source-> last_closed = date_to_timer (date_now (), time_now ());
      }
}


SCRIPT_LINE *
read_next_script_line (SCRIPT_LINE *script_line)
{
    SCRIPT_SOURCE
       *script_source;

    if (! script_line)
        return NULL;

    script_source = script_line-> parent;
    script_line   = script_line-> next;

    if ((void *) script_line == & script_source-> line_head)
        script_line = NULL;

    return script_line;
}


static void
destroy_script_data (SCRIPT_SOURCE *script_source)
{
    MEMTRN
        *memtrn;
    SCRIPT_LINE
        *script_line,
        *next_line;

    memtrn = script_source-> memtrn;

    script_line = (SCRIPT_LINE *) script_source-> line_head. next;
    while (script_line != (SCRIPT_LINE *) & script_source-> line_head)
      {
        if (script_line-> macros)
            script_line = script_line-> block_end-> next;
        else
          {
            next_line = script_line-> next;
            gg_free  (script_line-> node);
            mem_free (script_line-> text);
            list_unlink (script_line);
            mem_free (script_line);
            script_line = next_line;
          }
      }
    if (!script_source-> macros)
      {
        mem_free (script_source-> name);
        list_unlink (script_source);
        total_size -= script_source-> size;
        mem_free (script_source);
      }

    memt_assert  (memtrn);
    mem_rollback (memtrn);
}          


/********************   INITIALISE AGENT - ENTRY POINT    ********************/

/*  ---------------------------------------------------------------------[<]-
    Function: ggscrp_init

    Synopsis: Initialises the GSLGen script manager agent.  Returns 0 if
    initialised okay, -1 if there was an error.
    Supports these public methods:
    <Table>
    FILE        Open and parse a script file.
    TEXT        Open and parse text as a script.
    </Table>
    ---------------------------------------------------------------------[>]-*/

int
ggscrp_init (long size)
{
    AGENT   *agent;                     /*  Handle for our agent             */
#   include "ggscrp.i"                  /*  Include dialog interpreter       */

    /*  Shutdown event comes from Kernel                                     */
    declare_smtlib_shutdown   (shutdown_event, SMT_PRIORITY_MAX);

    /*  Public methods supported by this agent                               */
    declare_ggscrp_file (file_event, 0);
    declare_ggscrp_text (text_event, 0);
    declare_ggscrp_wait (wait_event, 0);

    /*  Reply messages from parser agent                                     */
    declare_ggpars_ok    (ok_event,          0);
    declare_ggpars_eof   (end_of_file_event, 0);
    declare_ggpars_error (error_event,       0);

    /*  Initialise list of parsed scripts                                    */
    list_reset (& script_list);
    total_size = 0;
    max_size = size;

    /*  Start parser                                                         */
    if (ggpars_init ())
        return (-1);

    /*  Signal okay to caller that we initialised okay                       */
    return (0);
}

int
ggscrp_term (void)
{
    while (!list_empty (& script_list))
        destroy_script_data ((SCRIPT_SOURCE *) script_list. prev);

    return ggpars_term ();
}


void
ggscrp_set_size (long size)
{
    max_size = size;
}


/*************************   INITIALISE THE THREAD   *************************/

MODULE initialise_the_thread (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */
}


/**************************   TERMINATE THE THREAD   *************************/

MODULE terminate_the_thread (THREAD *thread)
{
    SEMAPH
       *semaph;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_file)
        file_close (tcb-> script_file);

    if (tcb-> name)
      {
        semaph = semaph_lookup (tcb-> name);
        if (semaph)
            semaph_destroy (semaph);
      }

    mem_free (tcb-> name);
    mem_free (tcb-> text);
    mem_free (tcb-> line_text);

    the_next_event = terminate_event;
}


/***************************   WAIT ON SEMAPHORE   ***************************/

MODULE wait_on_semaphore (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> semaph)
        semaph_wait (tcb-> semaph);
}


/*****************************   OPEN THE FILE   *****************************/

MODULE open_the_file (THREAD *thread)
{
    char
        *fname;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    fname = file_where ('r', PATH, tcb-> name, "gsl");
    if (fname)
      {
        tcb-> script_source-> timestamp = get_file_time (fname);
        tcb-> script_file = file_open (fname, 'r');
      }

    if (! fname || ! tcb-> script_file)
        raise_exception (error_event);
}


/*****************************   OPEN THE TEXT   *****************************/

MODULE open_the_text (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> script_ptr = tcb-> text;
}


/*******************************   PARSE LINE   ******************************/

MODULE parse_line (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Save line number in case command is multi-line */
    tcb-> start_line = tcb-> line_nbr + 1;

    /*  Reset line text  */
    tcb-> line_text = NULL;

    if (tcb-> template)
        gg_parse_template (tcb-> script_read,
                           thread,
                           tcb-> script_source-> memtrn,
                           thread-> queue);
    else
        gg_parse_gsl (tcb-> script_read,
                      thread,
                      tcb-> script_source-> memtrn,
                      thread-> queue);
}


/***********************   DESTROY SCRIPT SOURCE NODE   **********************/

MODULE destroy_script_source_node (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    destroy_script_data (tcb-> script_source);
    tcb-> script_source = NULL;
}


/************************   SEND FILE ERROR MESSAGE   ************************/

MODULE send_file_error_message (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    log_error (thread, "Error opening file: %s", tcb-> name);
}


/******************************   RETURN DONE   ******************************/

MODULE return_done (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_source)
        total_size += tcb-> script_source-> size;

    free_excess_scripts (max_size);
    send_ggscrp_done (& tcb-> reply_qid,
                      tcb-> job,
                      tcb-> script_source);
}


/*****************************   SAVE THE NODE   *****************************/

MODULE save_the_node (THREAD *thread)
{
    struct_ggpars_ok
        *parse_result;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Get parse tree from parser agent message                             */
    get_ggpars_ok (thread-> event-> body, & parse_result);
    tcb-> script_node = (SCRIPT_NODE *) parse_result-> parse_root;
    tcb-> script_source-> size += parse_result-> size;
    free_ggpars_ok (& parse_result);

    tcb-> script_line = memt_alloc (tcb-> script_source-> memtrn,
                                    sizeof (SCRIPT_LINE));
    ASSERT (tcb-> script_line);
    list_reset (tcb-> script_line);

    list_relink_before (  tcb-> script_line, 
                        & tcb-> script_source-> line_head);
        
    tcb-> script_line-> parent      = tcb-> script_source;
    tcb-> script_line-> loop_start  = tcb-> loop_start;
    tcb-> script_line-> block_end   = NULL;
    tcb-> script_line-> line        = tcb-> start_line;
    tcb-> script_line-> template    = tcb-> template;
    tcb-> script_line-> node        = tcb-> script_node;
    tcb-> script_line-> macros      = 0;

    tcb-> script_line-> text = tcb-> line_text;
    tcb-> line_text = NULL;
}


/************************   GENERATE NODE TYPE EVENT   ***********************/

MODULE generate_node_type_event (THREAD *thread)
{
    static event_t type_event [] = {
        other_event,          /*  GG_COMMENT        */
        other_event,          /*  GG_LINE           */
        anomaly_event,        /*  GG_TEXT           */
        anomaly_event,        /*  GG_SUBSTITUTE     */
        anomaly_event,        /*  GG_OPERATOR       */
        other_event,          /*  GG_LITERAL        */
        anomaly_event,        /*  GG_NUMBER         */
        anomaly_event,        /*  GG_SYMBOL         */
        anomaly_event,        /*  GG_MEMBER         */
        other_event,          /*  GG_CALL           */
        anomaly_event,        /*  GG_OPERAND        */
        other_event,          /*  GG_OUTPUT         */
        other_event,          /*  GG_APPEND         */
        other_event,          /*  GG_CLOSE          */
        if_event,             /*  GG_IF             */
        else_if_event,        /*  GG_ELSIF          */
        else_event,           /*  GG_ELSE           */
        end_if_event,         /*  GG_END_IF         */
        for_event,            /*  GG_FOR            */
        end_for_event,        /*  GG_END_FOR        */
        scope_event,          /*  GG_SCOPE          */
        end_scope_event,      /*  GG_END_SCOPE      */
        new_event,            /*  GG_NEW            */
        end_new_event,        /*  GG_END_NEW        */
        other_event,          /*  GG_DELETE         */
        other_event,          /*  GG_MOVE           */
        other_event,          /*  GG_COPY           */
        while_event,          /*  GG_WHILE          */
        end_while_event,      /*  GG_END_WHILE      */
        other_event,          /*  GG_NEXT           */
        other_event,          /*  GG_LAST           */
        macro_event,          /*  GG_MACRO          */
        end_macro_event,      /*  GG_END_MACRO      */
        function_event,       /*  GG_FUNCTION       */
        end_function_event,   /*  GG_END_FUNCTION   */
        other_event,          /*  GG_RETURN         */
        other_event,          /*  GG_GSL            */
        other_event,          /*  GG_DIRECT         */
        other_event,          /*  GG_XML            */
        template_event,       /*  GG_TEMPLATE       */
        end_template_event,   /*  GG_END_TEMPLATE   */
        other_event,          /*  GG_ECHO           */
        other_event,          /*  GG_ABORT          */
        other_event,          /*  GG_DEFINE         */
        other_event,          /*  GG_SAVE           */
        other_event,          /*  GG_SORT           */
        _LR_NULL_EVENT };     /*  GG_UNDEFINED      */

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  If the node has an 'incoming' node, then we need to collect the      */
    /*  incoming script lines.  This should never clash with another event   */
    /*  ie the event would have been 'other'                                 */
    if (((tcb-> script_node-> type == GG_GSL)
    ||  (tcb-> script_node-> type == GG_DIRECT)
    ||  (tcb-> script_node-> type == GG_XML))
    &&  (tcb-> script_node-> name))
        the_next_event = incoming_event;
    else
      {
        ASSERT (tcb-> script_node-> type <  tblsize (type_event));
        the_next_event = type_event [tcb-> script_node-> type];
      }
}


/*********************   CONFIRM CONTROL STACK IS EMPTY   ********************/

MODULE confirm_control_stack_is_empty (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (! list_empty (& tcb-> control_stack))
      {
        list_pop (& tcb-> control_stack, tcb-> control);
        raise_exception (unmatched_control_event);
      }
}


/**************************   CLEAN CONTROL STACK   **************************/

MODULE clean_control_stack (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    list_destroy (& tcb-> control_stack);
}


/***************************   SEND ERROR MESSAGE   **************************/

MODULE send_error_message (THREAD *thread)
{
    struct_ggpars_error
        *parse_error;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Get parse tree from parser agent message                             */
    get_ggpars_error (thread-> event-> body, & parse_error);
    log_error (thread, "%s", parse_error-> error_text);
    free_ggpars_error (& parse_error);

    /*  Free line text  */
    mem_free (tcb-> line_text);
}


/****************************   SAVE LOOP START   ****************************/

MODULE save_loop_start (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> loop_start = tcb-> script_line;
}


/****************************   PUSH THE CONTROL   ***************************/

MODULE push_the_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    push_control (tcb-> script_node-> type, tcb-> template, tcb-> script_line);
}

static void
push_control (SCRIPT_NODE_TYPE type, Bool template, SCRIPT_LINE *script_line)
{
    CONTROL
        control;

    control.type        = type;
    control.template    = template;
    control.script_line = script_line;
    list_push (& tcb-> control_stack, control);
}


/***************************   SET TEMPLATE MODE   ***************************/

MODULE set_template_mode (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> template = TRUE;
}


/******************************   SET GSL MODE   *****************************/

MODULE set_gsl_mode (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> template = FALSE;
}


/************************   SET TEMPLATE OR GSL MODE   ***********************/

MODULE set_template_or_gsl_mode (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> template = tcb-> script_node-> op1
                     ? (Bool) atoi ((char *) tcb-> script_node-> op1-> op1) 
                     : TRUE;
}


/*************************   POP IF OR ELSE CONTROL   ************************/

MODULE pop_if_or_else_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> control. type = GG_UNDEFINED;

    if (! list_empty (& tcb-> control_stack))
        list_pop (& tcb-> control_stack, tcb-> control);

    if ((tcb-> control. type != GG_IF)
    &&  (tcb-> control. type != GG_ELSE))
        raise_exception (unmatched_control_event);
    else
      {
        tcb-> template = tcb-> control.template;
        tcb-> control. script_line-> block_end = tcb-> script_line;
      }
}


/*****************************   POP IF CONTROL   ****************************/

MODULE pop_if_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_control (thread, GG_IF);
}


static void
pop_control (THREAD *thread, SCRIPT_NODE_TYPE type)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> control. type        = GG_UNDEFINED;
    tcb-> control. script_line = NULL;

    if (! list_empty (& tcb-> control_stack))
        list_pop (& tcb-> control_stack, tcb-> control);

    if (tcb-> control. type != type)
        raise_exception (unmatched_control_event);
    else
      {
        tcb-> template = tcb-> control.template;
        tcb-> control. script_line-> block_end = tcb-> script_line;
      }
}


/****************************   PUSH IF CONTROL   ****************************/

MODULE push_if_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    push_control (GG_IF, tcb-> template, tcb-> script_line);
}


/****************************   POP FOR CONTROL   ****************************/

MODULE pop_for_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_control (thread, GG_FOR);
}


/***************************   POP SCOPE CONTROL   ***************************/

MODULE pop_scope_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_control (thread, GG_SCOPE);
}


/************************   GET PREVIOUS LOOP START   ************************/

MODULE get_previous_loop_start (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> loop_start = tcb-> loop_start-> loop_start;
}


/***************************   POP WHILE CONTROL   ***************************/

MODULE pop_while_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_control (thread, GG_WHILE);
}


/***************************   POP MACRO CONTROL   ***************************/

MODULE pop_macro_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_control (thread, GG_MACRO);
}


/**************************   POP FUNCTION CONTROL   *************************/

MODULE pop_function_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_control (thread, GG_FUNCTION);
}


/****************************   POP NEW CONTROL   ****************************/

MODULE pop_new_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_control (thread, GG_NEW);
}


/**********************   SEND UNMATCHED CONTROL ERROR   *********************/

MODULE send_unmatched_control_error (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> control. script_line)
        log_error (thread, 
                   "Unmatched control at (%s %u).",
                   tcb-> control. script_line-> parent-> name,
                   tcb-> control. script_line-> line);
    else
        log_error (thread,
                   "Unmatched control.");
}


/**************************   POP TEMPLATE CONTROL   *************************/

MODULE pop_template_control (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_control (thread, GG_TEMPLATE);
}


/****************************   READ SCRIPT LINE   ***************************/

MODULE read_script_line (THREAD *thread)
{
    static char
        line [LINE_MAX + 1];

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (! tcb-> script_read (thread, line))
        the_next_event = end_of_file_event;
    else
    if (strncmp (line,
                 (char *) tcb-> script_node-> name-> op1,
                 strlen ((char *) tcb-> script_node-> name-> op1)) == 0)
      {
        tcb-> script_line-> text = memt_strdup (tcb-> script_source-> memtrn,
                                                "");

        the_next_event = terminator_event;
      }
    else
      {
        tcb-> script_line-> text = memt_strdup (tcb-> script_source-> memtrn,
                                                line);

        the_next_event = ok_event;
      }
}


/***************************   EXTEND SCRIPT LINE   **************************/

MODULE extend_script_line (THREAD *thread)
{
    static char
        line [LINE_MAX + 1];
    int
        len;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (! tcb-> script_read (thread, line))
        the_next_event = end_of_file_event;
    else
    if (strncmp (line,
                 (char *) tcb-> script_node-> name-> op1,
                 strlen ((char *) tcb-> script_node-> name-> op1)) == 0)
        the_next_event = terminator_event;
    else
      {
        len = strlen (tcb-> script_line-> text);
        tcb-> script_line-> text = mem_realloc (tcb-> script_line-> text,
                                                len + strlen (line) + 2);
        tcb-> script_line-> text [len] = '\n';
        strcpy (tcb-> script_line-> text + len + 1, line);

        the_next_event = ok_event;
      }
}


/**************************   UNEXPECTED EOF ERROR   *************************/

MODULE unexpected_eof_error (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    log_error (thread,
               "Unexpected end of script.");
}


/*************************   SIGNAL INTERNAL ERROR   *************************/

MODULE signal_internal_error (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    coprintf ("Internal error.");
}


/**************************   SHUTDOWN EVERYTHING   **************************/

MODULE shutdown_everything (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    smt_shutdown ();
}

/*- Error Functions ---------------------------------------------------------*/

static void
log_error (THREAD *thread, char *format, ...)
{
    va_list
        argptr;
    char
        buffer [LINE_MAX];
    int
        length;

    va_start (argptr, format);          /*  Start variable arguments list    */
#if (defined (DOES_SNPRINTF))
    length = vsnprintf (buffer, LINE_MAX, format, argptr);
    ASSERT (length != -1);
#else
    length = vsprintf (buffer, format, argptr);
    ASSERT (length <= LINE_MAX);
#endif
    va_end (argptr);                    /*  End variable arguments list      */

    send_ggscrp_error (& tcb-> reply_qid,
                       tcb-> job,
                       tcb-> name,
                       tcb-> start_line,
                       buffer);
}
