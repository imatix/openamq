/*===========================================================================*
 *                                                                           *
 *  ggcode.c - Code generator functions                                      *
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


/*- Type definitions --------------------------------------------------------*/

typedef GGCODE_TCB TCB;

/*- Function prototypes -----------------------------------------------------*/

static THREAD   *create_gsl_thread              (QUEUE    *replyqueue,
                                                 qbyte     job,
                                                 XML_ITEM *switches);
static void      initialise_gsl_thread          (THREAD *thread);
static void      destroy_gsl_thread             (THREAD *thread);
static void      build_xml_root                 (THREAD *gsl_thread,
                                                 int     count,
                                                 va_list ap);
static void      initialise_predefined_scopes   (THREAD *thread);
static void      copy_predefined_attribute      (THREAD *thread,
                                                 char *name);
static Bool      free_macro_data                (SYMBOL *symbol);
static void      report_error                   (event_t event,
                                                 char *format, ...);
static void      report_undefined_expression    (RESULT_NODE *node);
static void      must_be_defined                (RESULT_NODE *node);
static void      open_output_file               (char *mode);
static void      get_matching_child             (SCOPE_BLOCK *scope_block);
static int       parse_locator                  (XML_ITEM **parent,
                                                 XML_ITEM **sibling);
static Bool      sort_compare                   (LIST *t1, LIST *t2);
static XML_ITEM *lookup_from_xml                (RESULT_NODE *scope);
static int       substitute_parameters_into_xml (XML_ITEM    *xml_item,
                                                 SCRIPT_NODE *args,
                                                 RESULT_NODE *params);
static void      substitute_parameter_into_xml  (XML_ITEM *xml_item,
                                                 SCRIPT_NODE *arg,
                                                 RESULT_NODE *param);
static void      output_the_line                (char *line, 
                                                 Bool cobol);
static void      send_text_to_output            (char *text);
static void      report_unaliased_unstacked_scope
                                                (void);
static void      pop_the_script                 (void);
static void      get_the_parameter              (void);
static void      report_non_numeric_error       (RESULT_NODE *node);
static void      pop_the_result_node            (void);

/*- Definitions -------------------------------------------------------------*/

#define AGENT_NAME      "GSL"           /*  Our public name                  */


/*- Global variables used in this source file only --------------------------*/

static GGCODE_TCB
    *tcb;                               /*  Address thread context block     */

static QID
    operq;                              /*  Operator console event queue     */

static MEMTRN
    *global_memtrn;                     /*  Global memory allocations.       */

static SYMTAB
    *global_macros;                     /*  Global macros.                   */

static XML_ITEM
    *global_junk;

static HANDLER_FCT
    *abort_fct = NULL;                  /*  Abort handler function           */

#include "ggcode.d"                     /*  Include dialog data              */
#include "ggfunc.inc"                   /*  Include inbuilt functions        */


/********************   INITIALISE AGENT - ENTRY POINT    ********************/

/*  ---------------------------------------------------------------------[<]-
    Function: gsl_init

    Synopsis: Initialises the GSLGen interpreter agent.  Returns 0 if
    initialised okay, -1 if there was an error.
    Supports these public methods:
    <Table>
    EXECUTE     Execute a script in its entirety (including initialise
                & terminate).
    INITIALISE  Prepare to execute a script
    NEXT        Execute one line of a script.
    GSL         Execute a GSL command
    FINISH      After executing a script.
    </Table>
    ---------------------------------------------------------------------[>]-*/

int
gsl_init (long size)
{
    AGENT
       *agent;                          /*  Handle for our agent             */
    THREAD
       *thread;                         /*  Handle to operator thread        */

#   include "ggcode.i"                  /*  Include dialog interpreter       */

    /*  Shutdown event comes from Kernel                                     */
    declare_smtlib_shutdown   (shutdown_event, SMT_PRIORITY_MAX);

    /*  Public methods supported by this agent                               */
    declare_ggcode_execute  (execute_event,  0);
    declare_ggcode_start    (start_event,    0);
    declare_ggcode_continue (continue_event, 0);
    declare_ggcode_next     (next_event,     0);
    declare_ggcode_gsl      (gsl_event,      0);
    declare_ggcode_call     (call_event,     0);
    declare_ggcode_finish   (finish_event,   0);
    declare_ggcode_result   (result_event,   0);

    /*  Reply messages from script handler agent                             */
    declare_ggscrp_done  (done_event,  0);
    declare_ggscrp_error (error_event, 0);

    /*  Reply messages from parser agent                                     */
    declare_ggpars_ok    (ok_event,    0);
    declare_ggpars_error (error_event, 0);

    /*  Ensure that operator console is running, else start it up            */
    smtoper_init ();
    if ((thread = thread_lookup (SMT_OPERATOR, "")) != NULL)
        operq = thread-> queue-> qid;
    else
        return (-1);

    /*  Start script handler                                                 */
    if (ggscrp_init (size))
        return (-1);

    /*  Prepare object manager and inbuilt functions  */
    initialise_objects ();
    if (register_inbuilt_functions ())
        return (-1);

    global_memtrn = mem_new_trans ();
    global_macros = sym_create_table ();
    list_reset (& all_scopes);
    global_junk   = xml_new (NULL, NULL, NULL);

    /*  Signal okay to caller that we initialised okay                       */
    return (0);
}


THREAD *
gsl_execute (QUEUE    *replyqueue,
             qbyte     job,
             XML_ITEM *switches,
             int       count,
             /* XML_ITEM *xml */ ...)
{
    va_list 
        ap;
    THREAD
       *thread;                         /*  Handle to GSL thread             */
    MEMTRN
        *save_memtrn;

    thread = create_gsl_thread (replyqueue,
                                job,
                                switches);

    /*  Memory allocations should be attached to gsl thread.  */
    save_memtrn = smt_memtrn_;
    smt_memtrn_ = thread-> memtrn;

    va_start (ap, count);
    build_xml_root (thread,
                    count,
                    ap);
    va_end (ap);
    initialise_predefined_scopes (thread);
    lsend_ggcode_execute (& thread-> queue-> qid,
                          replyqueue ? & replyqueue-> qid : NULL,
                          NULL, NULL, NULL, 0);

    /*  And restore old memory transaction.  */
    smt_memtrn_ = save_memtrn;

    return thread;
}


static THREAD *
create_gsl_thread (QUEUE    *replyqueue,
                   qbyte     job,
                   XML_ITEM *switches)
{
    THREAD
        *thread;                    /*  Handle to GSL thread             */
    TCB
        *tcb;

    thread = thread_create (AGENT_NAME, "");
    if (! thread)
        return NULL;

    initialise_gsl_thread (thread);

    tcb = thread-> tcb;         /*  Point to thread's context        */

    tcb-> replyq       = replyqueue;
    tcb-> job          = (void *) (long) job;
    tcb-> execute_full = TRUE;
    tcb-> xml_count    = 0;
    tcb-> xml_root     = NULL;
    tcb-> xml_junk     = xml_new (NULL, NULL, NULL);

    if (switches)
      {
        tcb-> xml_switches = xml_new (tcb-> xml_junk, "switches", NULL);
        ASSERT (tcb-> xml_switches);
        xml_copy (tcb-> xml_switches, switches);
      }

    return thread;
}


static void
initialise_gsl_thread (THREAD *thread)
{
    TCB
        *tcb  = thread-> tcb;           /*  Point to thread's context        */

    thread-> atdestroy = destroy_gsl_thread;

    /*  Initialise TCB stuff.                                                */
    tcb-> scratch_memtrn = mem_new_trans ();

    tcb-> thread          = thread;
    tcb-> output_line     = 1;
    tcb-> last_line       = 0;
    tcb-> shuffle         = 0;
    tcb-> execute_level   = 0;

    tcb-> script_name     = NULL;
    tcb-> script_text     = NULL;
    tcb-> output_name     = NULL;
    tcb-> terminator      = NULL;

    tcb-> output          = NULL;

    tcb-> script_root     = NULL;
    tcb-> script_node     = NULL;
    tcb-> evaluate_node   = NULL;
    tcb-> operand_node    = NULL;
    tcb-> fake_for_node   = NULL;

    tcb-> result_ptr      = NULL;
    tcb-> result_root     = NULL;
    tcb-> result_node     = NULL;
    tcb-> output_buffer   = NULL;

    tcb-> template    = FALSE;
    tcb-> ignorecase  = FALSE;
    tcb-> stdout_echo = TRUE;
    tcb-> stepping    = FALSE;

    list_reset (& tcb-> scope_stack);
    list_reset (& tcb-> script_stack);
    tcb-> node_stack = NULL;

    tcb-> scope_stack_ref = memt_alloc (thread-> memtrn, 
                                        sizeof (SCOPE_STACK_REF));
    list_reset (tcb-> scope_stack_ref);
    tcb-> scope_stack_ref-> scope_stack = & tcb-> scope_stack;
    list_relink_before (tcb-> scope_stack_ref, & all_scopes);

    tcb-> script_source   = NULL;
    tcb-> script_line     = NULL;
    tcb-> output_fct      = NULL;

    /*  Create macro symbol table                                            */
    tcb-> macros = sym_create_table ();
}


static void
destroy_gsl_thread (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Now get rid of macro definitions  */
    sym_exec_all (tcb-> macros, free_macro_data);
    sym_delete_table (tcb-> macros);

    mem_rollback (tcb-> scratch_memtrn);

    list_unlink (tcb-> scope_stack_ref);
    mem_free    (tcb-> scope_stack_ref);

    mem_free (tcb-> xml_root);
    destroy_result (tcb-> output_buffer);

    close_output_file (thread);
    close_script (tcb-> script_source);

    list_destroy (& tcb-> script_stack);

    if (tcb-> fake_for_node)
        mem_free (tcb-> fake_for_node);

    while (tcb-> node_stack)
      {
        tcb-> result_node = tcb-> node_stack;
        tcb-> node_stack  = tcb-> node_stack-> next;
        tcb-> result_node-> next = NULL;

        if (tcb-> result_node && (tcb-> result_node != tcb-> result_root))
            destroy_result (tcb-> result_node);
      }

    if (tcb-> result_root)
        destroy_result (tcb-> result_root);

    while (!list_empty (&tcb-> scope_stack))
        destroy_scope_block (& tcb-> scope_stack);

    mem_strfree (& tcb-> script_name);
    mem_strfree (& tcb-> script_text);

    /*  Deal with other threads that may be sharing the same XML structures. */
    remove_scope_references_to_xml (tcb-> xml_junk);
    xml_free (tcb-> xml_junk);
}


static void
build_xml_root (THREAD *gsl_thread,
                int     count,
                va_list ap)
{
    TCB
        *tcb;
    int
        i;

    if (count <= 0)
        return;

    tcb = gsl_thread-> tcb;             /*  Point to thread's context        */

    tcb-> xml_root = memt_alloc (gsl_thread-> memtrn,
                                 count * sizeof (XML_ITEM *));
    ASSERT (tcb-> xml_root);

    tcb-> xml_count = count;
    for (i = 0; i < count; i++)
        tcb-> xml_root [i] = va_arg (ap, XML_ITEM *);
}


static void
initialise_predefined_scopes (THREAD *thread)
{
    TCB
        *tcb = thread-> tcb;
    SCOPE_BLOCK
        *global_block,
        *root_item,
        *switches_block;
    int
        i;
    const char
        *argn,
        *arg;

    /*  Create initial global block  */
    tcb-> xml_gsl = xml_new (global_junk, "gsl", "");
    global_block  = create_scope_block (& tcb-> scope_stack, 
                                        GG_UNDEFINED, "gsl");
    global_block-> xml_item = tcb-> xml_gsl;

    /*  For backwards compatibility we create another copy called global */
    global_block = create_scope_block (& tcb-> scope_stack,
                                       GG_UNDEFINED, "global");
    global_block-> stacked = FALSE;
    global_block-> xml_item = tcb-> xml_gsl;

    /*  Create root XML blocks  */
    for (i = 0; i < tcb-> xml_count; i++)
      {
        root_item = create_scope_block (& tcb-> scope_stack,
                                        GG_UNDEFINED, 
                                        xml_item_name (tcb-> xml_root [i]));
        create_scope_item (root_item, tcb-> xml_root [i], 1);
        first_scope_item  (root_item);
      }

    /*  Create switches scope, initially stacked to find attributes.  */
    switches_block = create_scope_block (& tcb-> scope_stack,
                                         GG_UNDEFINED, "switches");
    switches_block-> xml_item = tcb-> xml_switches;

    /*  Copy predefined stuff into gsl block  */
    copy_predefined_attribute (thread, "date");
    copy_predefined_attribute (thread, "time");
    copy_predefined_attribute (thread, "version");
    copy_predefined_attribute (thread, "me");
    copy_predefined_attribute (thread, "template");
    copy_predefined_attribute (thread, "filename");
    copy_predefined_attribute (thread, "script");
    copy_predefined_attribute (thread, "ignorecase");
    copy_predefined_attribute (thread, "shuffle");
    copy_predefined_attribute (thread, "robust");
    copy_predefined_attribute (thread, "terminator");
    copy_predefined_attribute (thread, "cobol");

    /*  Now make switches unstacked.  */
    switches_block-> stacked = FALSE;

    /*  Copy arguments from switches into gsl block  */
    i = 0;
    do
      {
        i++;
        argn = strprintf ("arg%u", i);
        arg = xml_get_attr (tcb-> xml_switches,
                            argn,
                            NULL);
        if (arg)
            xml_put_attr (tcb-> xml_gsl,
                          argn,
                          arg);
      } while (arg);
}


static void
copy_predefined_attribute (THREAD *thread, char *name)
{
    TCB
        *tcb = thread-> tcb;
    RESULT_NODE
        symbol_name = {NULL,
                       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                       NULL, NULL, NULL, NULL, 
                       0, NULL, NULL, 
                       0, 0, 0,
                       {TYPE_STRING, NULL, 0},
                       FALSE },
        symbol =      {NULL,
                       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                       NULL, NULL, NULL, NULL, 
                       0, NULL, NULL, 
                       0, 0, 0,
                       {TYPE_UNDEFINED, NULL, 0},
                       FALSE };
        
    symbol. name = &symbol_name;
    symbol_name. value. s = name;
    xml_put_attr (tcb-> xml_gsl, 
                  name,
                  symbol_value (& tcb-> scope_stack, 
                                & symbol,
                                FALSE, 
                                NULL));
}


THREAD *
gsl_start (QUEUE    *replyqueue,
           qbyte     job,
           XML_ITEM *switches,
           int       count,
           /* XML_ITEM *xml */ ...)
{
    va_list 
        ap;
    THREAD
       *thread;                         /*  Handle to GSL thread             */
    MEMTRN
        *save_memtrn;

    thread = create_gsl_thread (replyqueue,
                                job,
                                switches);

    /*  Memory allocations should be attached to gsl thread.  */
    save_memtrn = smt_memtrn_;
    smt_memtrn_ = thread-> memtrn;

    va_start (ap, count);
    build_xml_root (thread,
                    count,
                    ap);
    va_end (ap);
    initialise_predefined_scopes (thread);
    ((TCB *) thread-> tcb)-> execute_full = FALSE;
    lsend_ggcode_start (& thread-> queue-> qid,
                        replyqueue ? & replyqueue-> qid : NULL,
                        NULL, NULL, NULL, 0);

    /*  And restore old memory transaction.  */
    smt_memtrn_ = save_memtrn;

    return thread;
}


void
gsl_continue (THREAD   *gsl_thread,
              QUEUE    *replyqueue)
{
    lsend_ggcode_continue (& gsl_thread-> queue-> qid,
                           replyqueue ? & replyqueue-> qid : NULL,
                           NULL, NULL, NULL, 0);
}

void
gsl_next (THREAD   *gsl_thread,
          QUEUE    *replyqueue)
{
    lsend_ggcode_next (& gsl_thread-> queue-> qid,
                       replyqueue ? & replyqueue-> qid : NULL,
                       NULL, NULL, NULL, 0);
}

void
gsl_command (THREAD   *gsl_thread,
             char     *command,
             Bool      terminate,
             QUEUE    *replyqueue)
{
    ((TCB *) gsl_thread-> tcb)-> execute_full = terminate;
    lsend_ggcode_gsl (& gsl_thread-> queue-> qid,
                      replyqueue ? & replyqueue-> qid : NULL,
                      NULL, NULL, NULL, 0,
                      command);
}


int
gsl_function (THREAD   *gsl_thread,
              QUEUE    *replyqueue,
              char     *function,
              int       parm_count,
              char     *parm_value[])
{
    SYMBOL
        *symbol = NULL;
    SCRIPT_MACRO
        *macro;
    SCRIPT_NODE
        *arg;
    int
        argn,
        i;
    SCOPE_BLOCK
        *scope_block;
    MEMTRN
        *save_memtrn;

    tcb = gsl_thread-> tcb;

    symbol = sym_lookup_symbol (tcb-> macros, function);
    if (! symbol)
        symbol = sym_lookup_symbol (global_macros, function);

    if (! symbol)
        return -1;

    macro = (SCRIPT_MACRO *) symbol-> data;
    arg = macro-> args;
    if (! arg)
        argn = 0;
    else
        argn = 1;

    while (arg
       && (arg-> type     == GG_OPERATOR)
       && (arg-> operator == OP_NEXT_ARG))
      {
        arg = arg-> op1;
        argn++;
      }

    /*  Must be at least as many args as params  */
    if (argn < parm_count)
        return -1;

    /*  Memory allocations should be attached to gsl thread.  */
    save_memtrn = smt_memtrn_;
    smt_memtrn_ = gsl_thread-> memtrn;

    scope_block = create_scope_block
                      (& tcb-> scope_stack,
                       GG_MACRO, 
                       function);
    scope_block-> stacked  = FALSE;     /*  Macro/f'n scopes aren't stacked  */
    scope_block-> xml_item = xml_new (tcb-> xml_junk, "", "");

    if (parm_count)
        xml_put_attr_ic (scope_block-> xml_item,
                         (char *) arg-> op1,
                         parm_value [0],
                         tcb-> ignorecase,
                         TRUE);
    for (i = 1; i < parm_count; i++)
      {
        arg = arg-> parent;
        xml_put_attr_ic (scope_block-> xml_item,
                         (char *) arg-> op2-> op1,
                         parm_value [i],
                         tcb-> ignorecase,
                         TRUE);
      }

    push_script_position (gsl_thread);

    tcb-> script_line   = macro-> position;
    tcb-> script_source = tcb-> script_line-> parent;

    lsend_ggcode_call (& gsl_thread-> queue-> qid,
                       replyqueue ? & replyqueue-> qid : NULL,
                       NULL, NULL, NULL, 0);

    /*  And restore old memory transaction.  */
    smt_memtrn_ = save_memtrn;

    return 0;
}


void
gsl_finish (THREAD *gsl_thread)
{
    lsend_ggcode_finish (& gsl_thread-> queue-> qid,
                         NULL,
                         NULL, NULL, NULL, 0);
}


THREAD *
gsl_spawn (THREAD *old_thread,
           char   *command)
{
    THREAD
       *new_thread;                     /*  Handle to GSL thread             */
    TCB
        *old_tcb,
        *new_tcb;
    MEMTRN
        *save_memtrn;

    if (old_thread)
        old_tcb = old_thread-> tcb;
    else
        old_tcb = tcb;
            
    new_thread = create_gsl_thread (old_tcb-> replyq,
                                    0,
                                    old_tcb-> xml_switches);

    /*  Memory allocations should be attached to gsl thread.  */
    save_memtrn = smt_memtrn_;
    smt_memtrn_ = new_thread-> memtrn;

    new_tcb = new_thread-> tcb;
    initialise_predefined_scopes (new_thread);
    copy_scope_stack (& new_tcb-> scope_stack,
                      & old_tcb-> scope_stack);
    lsend_ggcode_gsl (& new_thread-> queue-> qid,
                      old_tcb-> replyq ? & old_tcb-> replyq-> qid : NULL,
                      NULL, NULL, NULL, 0,
                      command);

    /*  And restore old memory transaction.  */
    smt_memtrn_ = save_memtrn;

    return new_thread;
}


char *
gsl_cur_script (THREAD *thread)
{
    if (((GGCODE_TCB *) thread-> tcb)-> script_source)
        return ((GGCODE_TCB *) thread-> tcb)-> script_source-> name;
    else
        return NULL;
}


int
gsl_cur_line (THREAD *thread)
{
    if (((GGCODE_TCB *) thread-> tcb)-> script_line)
        return ((GGCODE_TCB *) thread-> tcb)-> script_line-> line;
    else
        return 0;
}


char *
gsl_cur_text (THREAD *thread)
{
    if (((GGCODE_TCB *) thread-> tcb)-> script_line)
        return ((GGCODE_TCB *) thread-> tcb)-> script_line-> text;
    else
        return NULL;
}


int
gsl_term (void)
{
    sym_exec_all (global_macros, free_macro_data);
    sym_delete_table (global_macros);

    xml_free (global_junk);
    memt_assert  (global_memtrn);
    mem_rollback (global_memtrn);

    destroy_objects ();
    destroy_caches ();
    return ggscrp_term ();
}


static Bool
free_macro_data (SYMBOL *symbol)
{
    SCRIPT_LINE
        *script_line;
    SCRIPT_SOURCE
        *script_source;

    script_line = (SCRIPT_LINE *) ((SCRIPT_MACRO *) (symbol-> data))-> position;
    script_source = script_line-> parent;
    script_source-> macros--;
    script_line-> macros--;

    close_script (script_source);

    mem_free (symbol-> data);
    return TRUE;
}


/*************************   INITIALISE THE THREAD   *************************/

MODULE initialise_the_thread (THREAD *thread)
{
    char
       *value;
    int i;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> me = xml_get_attr (tcb-> xml_gsl, "me", NULL) ;

    for (i = 0; i < tcb-> xml_count; i++)
        if (!tcb-> xml_root [0])
          {
            report_error (fatal_event, "Missing XML data.");
            return;
          }

    /*  Lookup initial template flag                                         */
    value = xml_get_attr (tcb-> xml_gsl, "template", NULL) ;
    if (value)
        tcb-> template = atoi (value);
    else
        tcb-> template = 0;
}


static void
report_error (event_t event, char *format, ...)
{
    va_list
        argptr;
    char
        buffer [LINE_MAX];
    int
        length;
    THREAD
       *thread = tcb-> thread;

    va_start (argptr, format);          /*  Start variable arguments list    */
#if (defined (DOES_SNPRINTF))
    length = vsnprintf (buffer, LINE_MAX, format, argptr);
    ASSERT (length != -1);
#else
    length = vsprintf (buffer, format, argptr);
    ASSERT (length <= LINE_MAX);
#endif
    va_end (argptr);                    /*  End variable arguments list      */

    if (tcb-> script_line)
        send_ggcode_error (tcb-> replyq ? & tcb-> replyq-> qid : NULL,
                           tcb-> job,
                           tcb-> script_source-> name,
                           tcb-> script_line  -> line,
                           buffer);
    else
        send_ggcode_error (tcb-> replyq ? & tcb-> replyq-> qid : NULL,
                           tcb-> job,
                           NULL,
                           0,
                           buffer);

    if (event != _LR_NULL_EVENT)
        call_exception (event);
}


/*************************   RESET ERROR OCCURRENCE   ************************/

MODULE reset_error_occurrence (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> error_occurred = FALSE;
}


/************************   CALL LOAD INITIAL SCRIPT   ***********************/

MODULE call_load_initial_script (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Find initial script name, first in switches, then in XML             */
    tcb-> script_name = mem_strdup (xml_get_attr (tcb-> xml_gsl, "script", NULL));

    if (tcb-> script_name)
        call_exception  (load_script_file_event);
    else
        report_error (error_event, "No script specified.");
}


/*******************   RAISE EXCEPTION IF ERROR OCCURRED   *******************/

MODULE raise_exception_if_error_occurred (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> error_occurred)
        raise_exception (exception_event);
}


/*************************   GET NEXT SCRIPT LINE   **************************/

MODULE get_next_script_line (THREAD *thread)
{
    char
        *value;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Lookup ignore case flag                                              */
    value = xml_get_attr (tcb-> xml_gsl, "ignorecase", NULL);
    tcb-> ignorecase = value ? atoi (value) : 0;

    /*  Lookup shuffle flag                                                  */
    value = xml_get_attr (tcb-> xml_gsl, "shuffle", NULL);
    tcb-> shuffle = value ? atoi (value) : 0;

    /*  Lookup xml robustness flag                                           */
    value = xml_get_attr (tcb-> xml_gsl, "robust", NULL);
    tcb-> robust = value ? atoi (value) : 0;

    /*  Lookup line terminator                                               */
    tcb-> terminator = xml_get_attr (tcb-> xml_gsl, "terminator", "\n");

    tcb-> script_line = read_next_script_line (tcb-> script_line);
    while (tcb-> script_line && ! tcb-> script_line-> node)
        tcb-> script_line = read_next_script_line (tcb-> script_line);

    if (tcb-> script_line)
      {
        tcb-> script_root = tcb-> script_line-> node;
        tcb-> script_node = tcb-> script_root;
      }
    else
      {
        tcb-> script_root = NULL;
        tcb-> script_node = NULL;
        raise_exception (end_of_script_event);
      }
}


/************************   CALL ACCEPT GSL COMMAND   ************************/

MODULE call_accept_gsl_command (THREAD *thread)
{
    struct_ggcode_gsl
        *param;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_ggcode_gsl (thread-> event-> body, & param);

    tcb-> template = FALSE;

    tcb-> script_name = mem_strdup ("Command");
    tcb-> script_text = mem_strdup (param-> command);

    call_exception (load_script_text_event);
    free_ggcode_gsl (&param);
}


/************************   CALL EXECUTE SCRIPT LINE   ***********************/

MODULE call_execute_script_line (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node)
        call_exception  (execute_line_event);
}


/***************************   GENERATE OK EVENT   ***************************/

MODULE generate_ok_event (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    the_next_event = ok_event;
}


/**********************   CALL FUNCTION CALL EXCEPTION   *********************/

MODULE call_function_call_exception (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */
    
    call_exception (function_call_event);
}


/************************   GENERATE NODE TYPE EVENT   ***********************/

MODULE generate_node_type_event (THREAD *thread)
{
    static event_t type_event [] = {
        comment_event,        /*  GG_COMMENT        */
        line_event,           /*  GG_LINE           */
        text_event,           /*  GG_TEXT           */
        substitute_event,     /*  GG_SUBSTITUTE     */
        operator_event,       /*  GG_OPERATOR       */
        literal_event,        /*  GG_LITERAL        */
        number_event,         /*  GG_NUMBER         */
        symbol_event,         /*  GG_SYMBOL         */
        member_event,         /*  GG_MEMBER         */
        call_event,           /*  GG_CALL           */
        operand_event,        /*  GG_OPERAND        */
        output_event,         /*  GG_OUTPUT         */
        append_event,         /*  GG_APPEND         */
        close_event,          /*  GG_CLOSE          */
        if_event,             /*  GG_IF             */
        elsif_event,          /*  GG_ELSIF          */
        else_event,           /*  GG_ELSE           */
        end_if_event,         /*  GG_END_IF         */
        for_event,            /*  GG_FOR            */
        end_for_event,        /*  GG_END_FOR        */
        scope_event,          /*  GG_SCOPE          */
        end_scope_event,      /*  GG_END_SCOPE      */
        new_event,            /*  GG_NEW            */
        end_new_event,        /*  GG_END_NEW        */
        delete_event,         /*  GG_DELETE         */
        move_event,           /*  GG_MOVE           */
        copy_event,           /*  GG_COPY           */
        while_event,          /*  GG_WHILE          */
        end_while_event,      /*  GG_END_WHILE      */
        next_event,           /*  GG_NEXT           */
        last_event,           /*  GG_LAST           */
        macro_event,          /*  GG_MACRO          */
        end_macro_event,      /*  GG_END_MACRO      */
        function_event,       /*  GG_FUNCTION       */
        end_function_event,   /*  GG_END_FUNCTION   */
        return_event,         /*  GG_RETURN         */
        gsl_event,            /*  GG_GSL            */
        direct_event,         /*  GG_DIRECT         */
        xml_event,            /*  GG_XML            */
        template_event,       /*  GG_TEMPLATE       */
        end_template_event,   /*  GG_END_TEMPLATE   */
        echo_event,           /*  GG_ECHO           */
        abort_event,          /*  GG_ABORT          */
        define_event,         /*  GG_DEFINE         */
        save_event,           /*  GG_SAVE           */
        sort_event,           /*  GG_SORT           */
        _LR_NULL_EVENT };     /*  GG_UNDEFINED      */

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node)
      {
        ASSERT (tcb-> script_node-> type <  tblsize (type_event));
        the_next_event = type_event [tcb-> script_node-> type];
        if (the_next_event == _LR_NULL_EVENT)
            raise_exception (anomaly_event);
      }
    else
        the_next_event = end_of_script_event;
}


/***************************   CREATE RESULT ROOT   **************************/

MODULE create_result_root (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_root = new_result_node ();
    ASSERT (tcb-> result_root);
    init_result_node (tcb-> result_root);
    
    tcb-> result_root-> script_node = tcb-> script_root;
    tcb-> result_node = tcb-> result_root;
}


/***************************   CALL EVALUATE OP1   ***************************/

MODULE call_evaluate_op1 (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> op1)
      {
        tcb-> evaluate_node = tcb-> script_node-> op1;
        tcb-> result_ptr    =   tcb-> result_node
                            ? & tcb-> result_node-> op1
                            : & tcb-> result_root;
        call_exception (evaluate_event);
      }
}


/**************************   COPY LINE TO OUTPUT   **************************/

MODULE copy_line_to_output (THREAD *thread)
{
    Bool
        extend,                         /*  Is line extended?  ie no EOL     */
        cobol;                          /*  Are we producing COBOL code?     */
    char
        *value,
        *line;
    RESULT_NODE
        *new_node;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    extend = tcb-> script_node-> op1-> extend;
    value = xml_get_attr (tcb-> xml_gsl, "cobol", NULL);
    cobol = (value != NULL);

    if (tcb-> output_buffer)
      {
        new_node = new_result_node ();
        ASSERT (new_node);
        init_result_node (new_node);

        new_node-> op1 = tcb-> output_buffer;
        /*  Migrate indent value to new node.  */
        new_node-> indent = tcb-> output_buffer-> indent;
        tcb-> output_buffer-> indent = 0;
        new_node-> op1-> parent = new_node;
        new_node-> op2 = tcb-> result_node-> op1;
        new_node-> op2-> parent = new_node;
        new_node-> culprit = new_node-> op1-> culprit
                           ? new_node-> op1-> culprit
                           : new_node-> op2-> culprit;
        tcb-> result_node-> op1 = new_node;
        tcb-> output_buffer = NULL;
      }
    /*  If this isn't end of line then save result and stop.  */
    if (extend)
      {
        tcb-> output_buffer = tcb-> result_node-> op1;
        tcb-> result_node-> op1 = NULL;
      }
    else
      {
        line = concatenate_results (tcb-> result_node-> op1,
                                    tcb-> shuffle,
                                    TRUE);
        if (! line)
          {
            report_undefined_expression (tcb-> result_node-> op1);
            return;
          }

        output_the_line (line, cobol);
        send_text_to_output (tcb-> terminator);
      }
}


static void
output_the_line (char *line, Bool cobol)
{
    char
        *out,
        *ptr,
        linenum [7];

    out = line;
    while (out)
      {
        ptr = strchr (out, '\n');
        if (ptr)
            *ptr = 0;
        if (cobol)
          {
            sprintf (linenum, "%04d00", tcb-> output_line);
            if (strlen (out) > 6)
              {
                memcpy (out, linenum, 6);
                send_text_to_output (out);
              }
            else
                send_text_to_output (linenum);
          }
        else
            send_text_to_output (out);

        tcb-> output_line++;
        if (ptr)
          {
            send_text_to_output (tcb-> terminator);
            out = ptr + 1;
          }
        else
            out = 0;
      }
    put_numeric_attr (tcb-> xml_gsl,
                      "line", 
                      tcb-> output_line);

    mem_free (line);
}


static void
send_text_to_output (char *text)
{
    if (tcb-> output)
        fprintf (tcb-> output, "%s", text);
    else
      {
        if (tcb-> output_fct)
            (tcb-> output_fct) ((qbyte) (long) tcb-> job, text);
        if (tcb-> stdout_echo)
            fprintf (stdout, "%s", text);
      }
}


/**********************   COPY OUTPUT BUFFER TO OUTPUT   *********************/

MODULE copy_output_buffer_to_output (THREAD *thread)
{
    Bool
        cobol;                          /*  Are we producing COBOL code?     */
    char
        *value,
        *line;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (! tcb-> output_buffer)
        return;

    value = xml_get_attr (tcb-> xml_gsl, "cobol", NULL);
    cobol = (value != NULL);

    line = concatenate_results (tcb-> output_buffer,
                                tcb-> shuffle,
                                TRUE);
    if (! line)
      {
        report_undefined_expression (tcb-> output_buffer);
        return;
      }
    output_the_line (line, cobol);
}


/****************************   FREE RESULT ROOT   ***************************/

MODULE free_result_root (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    destroy_result (tcb-> result_root);
    tcb-> result_root = NULL;
}


/**************************   CALL EVALUATE SCOPE   **************************/

MODULE call_evaluate_scope (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> scope)
      {
        tcb-> evaluate_node =   tcb-> script_node-> scope;
        tcb-> result_ptr    = & tcb-> result_node-> scope;
        call_exception (evaluate_event);
      }
}


/***************************   CALL EVALUATE NAME   **************************/

MODULE call_evaluate_name (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> name)
      {
        tcb-> evaluate_node =   tcb-> script_node-> name;
        tcb-> result_ptr    = & tcb-> result_node-> name;
        call_exception (evaluate_event);
      }
}


/**********************   GENERATE OPERATOR TYPE EVENT   *********************/

MODULE generate_operator_type_event (THREAD *thread)
{
    static event_t op_type_event [] = {
        undefined_event,                /*  OP_UNDEFINED           */
        arithmetic_event,               /*  OP_TIMES               */
        arithmetic_event,               /*  OP_DIVIDE              */
        arithmetic_event,               /*  OP_PLUS                */
        arithmetic_event,               /*  OP_MINUS               */
        default_event,                  /*  OP_DEFAULT             */
        comparison_event,               /*  OP_EQUALS              */
        comparison_event,               /*  OP_NOT_EQUALS          */
        comparison_event,               /*  OP_GREATER_THAN        */
        comparison_event,               /*  OP_LESS_THAN           */
        comparison_event,               /*  OP_GREATER_EQUAL       */
        comparison_event,               /*  OP_LESS_EQUAL          */
        safe_comparison_event,          /*  OP_SAFE_EQUALS         */
        safe_comparison_event,          /*  OP_SAFE_NOT_EQUALS     */
        safe_comparison_event,          /*  OP_SAFE_GREATER_THAN   */
        safe_comparison_event,          /*  OP_SAFE_LESS_THAN      */
        safe_comparison_event,          /*  OP_SAFE_GREATER_EQUAL  */
        safe_comparison_event,          /*  OP_SAFE_LESS_EQUAL     */
        arithmetic_event,               /*  OP_NOT                 */
        or_event,                       /*  OP_OR                  */
        and_event,                      /*  OP_AND                 */
        next_arg_event };               /*  OP_NEXT_ARG            */

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    ASSERT (tcb-> script_node-> operator < tblsize (op_type_event));
    the_next_event = op_type_event [tcb-> script_node-> operator];
}


/*******************************   SKIP BLOCK   ******************************/

MODULE skip_block (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> script_line = tcb-> script_line-> block_end;
    tcb-> script_root = tcb-> script_line-> node;
    tcb-> script_node = tcb-> script_root;
}


/***************************   CLOSE OUTPUT FILE   ***************************/

MODULE close_output_file (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> output)
        file_close (tcb-> output);

    tcb-> output = NULL;
    mem_strfree (&tcb-> output_name);
    xml_put_attr (tcb-> xml_gsl,
                  "outfile",
                  NULL);
    xml_put_attr (tcb-> xml_gsl,
                  "line",
                  NULL);
}


/**************************   OPEN FILE FOR OUTPUT   *************************/

MODULE open_file_for_output (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  .output <op1>  */

    must_be_defined (tcb-> result_node-> op1);
    if (exception_raised)
        return;

    open_output_file (FOPEN_WRITE_TEXT);
    tcb-> output_line = 1;
    tcb-> last_line = 0;

    put_numeric_attr (tcb-> xml_gsl,
                      "line",
                      tcb-> output_line);
}


static void
open_output_file (char *mode)
{
    tcb-> output_name = mem_strdup (string_value (& tcb-> result_node-> op1-> value));

    if (tcb-> output_name && *tcb-> output_name)
        tcb-> output = fopen (tcb-> output_name, mode);

    if (tcb-> output)
        xml_put_attr (tcb-> xml_gsl,
                      "outfile",
                      tcb-> output_name);
    else
        report_error (error_event, "Can't open output file %s", tcb-> output_name);
}


/**************************   OPEN FILE FOR APPEND   *************************/

MODULE open_file_for_append (THREAD *thread)
{
    FILE
        *current;
    char
        buffer [LINE_MAX + 1];

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  .append <op1>  */

    must_be_defined (tcb-> result_node-> op1);
    if (exception_raised)
        return;

    /*  Count line number for extended file  */
    tcb-> output_line = 0;
    tcb-> last_line = 0;
    current = fopen (string_value (& tcb-> result_node-> op1-> value),
                     FOPEN_READ_TEXT);
    if (current)
      {
        while (file_read (current, buffer))
            tcb-> output_line++;
        file_close (current);
      }

    open_output_file (FOPEN_APPEND_TEXT);

    put_numeric_attr (tcb-> xml_gsl,
                      "line",
                      tcb-> output_line);
}


/**************************   PUSH SCRIPT POSITION   *************************/

MODULE push_script_position (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    list_push (& tcb-> script_stack, tcb-> script_line);
}


/*********************   CALL PREPARE TO INTERPRET GSL   *********************/

MODULE call_prepare_to_interpret_gsl (THREAD *thread)
{
    char
        buffer [32];
    SCRIPT_LINE
        *position;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_root-> name)
      {
        report_error (error_event, "Illegal GSL source.");
        return;
      }

    tcb-> template = tcb-> script_line-> template;
    if (tcb-> script_root-> scope)
      {
        tcb-> script_name = mem_strdup (string_value (& tcb-> result_node-> scope-> value));
        call_exception (load_script_file_event);
        return;
      }

    if (tcb-> script_root-> op1)
      {
        position = tcb-> script_line;
        snprintf (buffer, 32, "(%s %u)",
                              position-> parent-> name,
                              position-> line);

        tcb-> script_name = mem_strdup (buffer);
        tcb-> script_text = mem_strdup (
                                string_value (& tcb-> result_node-> op1-> value));

        call_exception (load_script_text_event);
      }
}


/*************************   COPY DIRECT TO OUTPUT   *************************/

MODULE copy_direct_to_output (THREAD *thread)
{
    char
        line [LINE_MAX + 2];
    FILE
        *file;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_root-> scope)
      {
        file = file_locate (PATH, string_value (& tcb-> result_node-> scope-> value),
                            NULL);
        if (! file)
          {
            report_error (error_event, "Can't open literal file %s",
                          string_value (& tcb-> result_node-> scope-> value));
            return;
          }
        while (file_read (file, line))
            send_text_to_output (line);

        file_close (file);
        return;
      }

    if (tcb-> script_root-> name)
      {
        send_text_to_output (tcb-> script_line-> text);
        return;
      }

    if (tcb-> script_root-> op1)
        send_text_to_output (string_value (& tcb-> result_node-> op1-> value));
}


/****************************   CALL EVALUATE AS   ***************************/

MODULE call_evaluate_as (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> as)
      {
        tcb-> evaluate_node =   tcb-> script_node-> as;
        tcb-> result_ptr    = & tcb-> result_node-> as;
        call_exception (evaluate_event);
      }
}


/****************************   CALL EVALUATE TO   ***************************/

MODULE call_evaluate_to (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> to)
      {
        tcb-> evaluate_node =   tcb-> script_node-> to;
        tcb-> result_ptr    = & tcb-> result_node-> to;
        call_exception (evaluate_event);
      }
}


/**************************   CALL EVALUATE BEFORE   *************************/

MODULE call_evaluate_before (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> before)
      {
        tcb-> evaluate_node =   tcb-> script_node-> before;
        tcb-> result_ptr    = & tcb-> result_node-> before;
        call_exception (evaluate_event);
      }
}


/**************************   CALL EVALUATE AFTER   **************************/

MODULE call_evaluate_after (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> after)
      {
        tcb-> evaluate_node =   tcb-> script_node-> after;
        tcb-> result_ptr    = & tcb-> result_node-> after;
        call_exception (evaluate_event);
      }
}


/****************************   LOAD SOURCE XML   ****************************/

MODULE load_source_xml (THREAD *thread)
{
    char
       *buffer;
    XML_ITEM
        *parent,
        *sibling,
        *xml_item,
        *xml_next,
        *file_root;
    int
        rc;
    
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /* .xml [as <as>] [to <to> | before <before> | after <after>] ...        */
    /* Invalid XML causes a runtime error unless the robust switch is 1      */

    must_be_defined (tcb-> result_node-> as);
    must_be_defined (tcb-> result_node-> to);
    must_be_defined (tcb-> result_node-> before);
    must_be_defined (tcb-> result_node-> after);
    if (exception_raised)
        return;

    if (parse_locator (& parent, & sibling))
        return;

    if (tcb-> script_root-> scope)
      {
        file_root = xml_create (NULL, NULL);
        rc = xml_load_file (& file_root,
                            PATH,
                            string_value (& tcb-> result_node-> scope-> value),
                            TRUE);
      }
    else
      {
        if (tcb-> script_root-> name)
            buffer = tcb-> script_line-> text;
        else
            buffer = string_value (& tcb-> result_node-> op1-> value);

        file_root = xml_create (NULL, NULL);
        rc = xml_load_string (& file_root,
                              buffer,
                              TRUE);
      }

    if (rc)
      {
        if (tcb-> robust == 0)
            report_error (error_event, "%s", xml_error ());
        /*  If robust=1, we do nothing at all - the program must check for
            XML validity through other means if it wants to.    */
      }
    else
      {
        xml_item = xml_first_child (file_root);
        while (xml_item)
          {
            xml_next = xml_next_sibling (xml_item);
            xml_detach (xml_item);
            if (tcb-> script_root-> as)
                xml_rename (xml_item,
                            string_value (& tcb-> result_node-> as-> value));

            if (sibling)
                xml_attach_sibling (sibling, xml_item);
            else
                xml_attach_child   (parent,  xml_item);

            xml_item = xml_next;
          }
      }
    xml_free (file_root);
}


static int
parse_locator (XML_ITEM **parent, XML_ITEM **sibling)
{
    RESULT_NODE
        *sibling_node;

    if (tcb-> result_root-> to)
      {
        *parent = lookup_from_xml (tcb-> result_node-> to);
        if (! *parent)
            return -1;
      }

    if (! (tcb-> result_root-> to
        || tcb-> result_root-> before
        || tcb-> result_root-> after))
      {
        *sibling = NULL;
        *parent  = lookup_from_xml (NULL);
        if (! *parent)
            return -1;
        return 0;
      }            

    if (tcb-> result_root-> before && tcb-> result_root-> after)
      {
        report_error (error_event, 
                      "Only one of 'before' and 'after' may be specified.");
        return -1;
      }

    if (tcb-> result_root-> before || tcb-> result_root-> after)
      {
        sibling_node = tcb-> result_root-> before ? tcb-> result_node-> before
                                            : tcb-> result_node-> after;
        *sibling = lookup_from_xml (sibling_node);
        if (! *sibling)
            return -1;

        if (tcb-> result_root-> to)
          {
            if (*parent != xml_parent (*sibling))
              {
                report_error (error_event, "Scope %s is not a child of scope %s",
                              string_value (& sibling_node-> value),
                              extended_scope_string (tcb-> result_node-> to));
                return -1;
              }
          }
        else
          {
            *parent = xml_parent (*sibling);
            if (! *parent)
              {
                report_error (error_event, "Scope %s has no parent.",
                              string_value (& sibling_node-> value));
                return -1;
              }
          }
      }
    else                       /*  No 'before' nor 'after'  */
        *sibling = NULL;

    if (tcb-> result_root-> after)
        *sibling = xml_next_sibling (*sibling);

    return 0;
}


static XML_ITEM *
lookup_from_xml (RESULT_NODE *scope)
{
    XML_ITEM
        *xml_item;
    char
        *error_text;

    xml_item = extended_scope_xml (& tcb-> scope_stack,
                                   scope,
                                   tcb-> ignorecase,
                                   &error_text);
    if (! xml_item)
      {
        if (error_text)
            report_error (error_event, "%s", error_text);
        else
            report_error (error_event, "Unknown data scope: %s",
                                       extended_scope_string (scope));
      }

    return xml_item;
}


/***********************   CALL BUILD ITERATION LIST   ***********************/

MODULE call_build_iteration_list (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    call_exception (build_list_event);
}

/* JS 2001/02/18 - for 'count' alias */
/********************   CALL BUILD COUNT ITERATION LIST   ********************/

MODULE call_build_count_iteration_list (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    call_exception (build_count_list_event);
}


/* JS 2001/02/18 - for 'count' alias */
/*************************   OPEN COUNT ALIAS BLOCK   ************************/

MODULE open_count_alias_block (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;
    XML_ITEM
        *xml_item;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  If count loop isn't aliased, then make an unstacked alias 'count'.   */
    /*  This is for backwards compatibility.                                 */
    if (! tcb-> result_node-> as)
      {
        xml_item = last_scope_block (& tcb-> scope_stack)-> xml_item;

        scope_block = create_scope_block (& tcb-> scope_stack, 
                                          GG_UNDEFINED, "count");
        scope_block-> stacked  = FALSE;
        scope_block-> xml_item = xml_item;
      }
}


/* JS 2001/02/18 - for 'count' alias */
/************************   CLOSE COUNT ALIAS BLOCK   ************************/

MODULE close_count_alias_block (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Now close the unstacked alias 'count', if it exists.                 */
    /*  This is for backwards compatibility.                                 */
    if (! tcb-> result_node-> as)
        destroy_scope_block (& tcb-> scope_stack);
}


/*****************************   SORT XML ITEMS   ****************************/

MODULE sort_xml_items (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;
    long
        index,
        count;
    XML_ITEM
        *parent,
        *next_xml_item,
        **next_item_arr;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    scope_block = last_scope_block (& tcb-> scope_stack);

    if (tcb-> script_root-> op2 && scope_block-> total)
      {
        /*  We build an array to hold the first XML item not in the scope    */
        /*  for each XML item in the scope.  This allows us to sort the      */
        /*  scope items in situ.                                             */
        next_item_arr = mem_alloc (scope_block-> total * sizeof (XML_ITEM *));
        ASSERT (next_item_arr);

        index = 0;
        first_scope_item (scope_block);
        parent = xml_parent (scope_block-> xml_item);

        while (scope_block-> xml_item)
          {
            /*  Count how many consecutive XML items are in the scope.       */
            count = 0;
            do
              {
                /*  Get sort key data type right now, so that we don't have  */
                /*  to do it inside the tighter sorting loop.  If sort_type  */
                /*  is TYPE_UNKNOWN then all the sort keys were converted to */
                /*  numbers during the first pass, but if sort_type is       */
                /*  TYPE_STRING then it's possible that some weren't         */
                /*  converted to strings, so we must do it now.              */
                if (tcb-> sort_type == TYPE_STRING)
                    string_value (& scope_block-> scope_item-> sort_key);

                next_xml_item = xml_next_sibling (scope_block-> xml_item);
                next_scope_item (scope_block);
                count++;
              } while (next_xml_item
                   && (next_xml_item == scope_block-> xml_item));
                
            while (count-- > 0)
                next_item_arr [index++] = next_xml_item;
          }

        /*  Now do the sort.                                                 */
        list_sort (& scope_block-> item_list, sort_compare);

        /*  And reattach the sorted XML items in the same location they were */
        index = 0;
        first_scope_item (scope_block);
        while (scope_block-> xml_item)
          {
            next_xml_item = next_item_arr [index++];
            if (next_xml_item)
                xml_attach_sibling (next_xml_item, scope_block-> xml_item);
            else
                xml_attach_child   (parent,        scope_block-> xml_item);
                
            next_scope_item (scope_block);
          }
        mem_free (next_item_arr);
      }
}


static Bool
sort_compare (LIST *t1, LIST *t2)
{
    if (tcb-> sort_type == TYPE_STRING)
        return (strcmp (((SCOPE_ITEM *) t1)-> sort_key. s,
                        ((SCOPE_ITEM *) t2)-> sort_key. s) > 0);
    else
        return (((SCOPE_ITEM *) t1)-> sort_key. n
             >  ((SCOPE_ITEM *) t2)-> sort_key. n);
}


/***************************   DESTROY FOR BLOCK   ***************************/

MODULE destroy_for_block (THREAD *thread)
{
    SCRIPT_NODE_TYPE
        scope_type;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    do
      {
        scope_type = last_scope_block (& tcb-> scope_stack)-> scope_type;
        destroy_scope_block (& tcb-> scope_stack);
      }
    while (scope_type != GG_FOR);
}


/**************************   INITIALISE FOR BLOCK   *************************/

MODULE initialise_for_block (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    scope_block = last_scope_block (& tcb-> scope_stack);

    /*  Sort if necessary  */
    if (tcb-> script_node-> op2)
      {
        /*  Get sort key data type right now, so that we don't have to       */
        /*  it inside the tighter sorting loop.  If sort_type is             */
        /*  TYPE_UNKNOWN then all the sort keys were converted to            */
        /*  numbers during the first pass, but if sort_type is               */
        /*  TYPE_STRING then it's possible that some weren't converted       */
        /*  to strings, so we must do it now.                                */
        first_scope_item (scope_block);
        while (scope_block-> xml_item)
          {
            if (tcb-> sort_type == TYPE_STRING)
                string_value (& scope_block-> scope_item-> sort_key);

            next_scope_item (scope_block);
          }

        list_sort (& scope_block-> item_list, sort_compare);
      }

    if (! first_scope_item (scope_block))
      {
        skip_block (thread);
        destroy_scope_block (& tcb-> scope_stack);
      }
}


/***************************   GO TO TOP OF BLOCK   **************************/

MODULE go_to_top_of_block (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (! tcb-> script_line-> loop_start)
      {
        report_error (error_event, "No loop.");
        return;
      }

    tcb-> script_line = tcb-> script_line-> loop_start;
    tcb-> script_root = tcb-> script_line-> node;
    tcb-> script_node = tcb-> script_root;
}


/************************   GO TO TOP OF NAMED BLOCK   ***********************/

MODULE go_to_top_of_named_block (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  .(next|last) [<scope>]  */
    if (tcb-> result_node-> scope)
      {
        must_be_defined (tcb-> result_node-> scope);
        if (exception_raised)
            return;

        scope_block = last_scope_block (& tcb-> scope_stack);
        while (scope_block
           && (! streq (scope_block-> name,
                        string_value (& tcb-> result_node-> scope-> value))))
          {
            if (scope_block-> scope_type == GG_FOR)
              {
                tcb-> script_line = tcb-> script_line-> loop_start;
                tcb-> script_root = tcb-> script_line-> node;
                tcb-> script_node = tcb-> script_root;
              }
            destroy_scope_block (& tcb-> scope_stack);
            scope_block = last_scope_block (& tcb-> scope_stack);
          }
        if (! scope_block)
          {
            report_error (error_event,
                          strprintf ("No loop named '%s'.",
                                     tcb-> result_node-> scope-> value. s));
            return;
          }
        if (scope_block-> scope_type != GG_FOR)
          {
            report_error (error_event,
                          strprintf ("Scope '%s' is not a 'for' loop.",
                                     tcb-> result_node-> scope-> value. s));
            return;
          }
      }
    go_to_top_of_block (thread);
}


/*********************   CONFIRM SCOPE NAME IS CORRECT   *********************/

MODULE confirm_scope_name_is_correct (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  .end??? [<scope>]  */
    if (! tcb-> result_node-> scope)
        return;

    must_be_defined (tcb-> result_node-> scope);
    if (exception_raised)
        return;

    scope_block = last_scope_block (& tcb-> scope_stack);
    if (! streq (scope_block-> name,
                 string_value (& tcb-> result_node-> scope-> value)))
      {
        report_error (error_event,
                      strprintf ("Mismatched scope name '%s'; should be '%s'",
                                 tcb-> result_node-> scope-> value. s,
                                 scope_block-> name));
        return;
      }
}


/****************************   ITERATE FOR LOOP   ***************************/

MODULE iterate_for_loop (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    while (last_scope_block (& tcb-> scope_stack)-> scope_type != GG_FOR)
        destroy_scope_block (& tcb-> scope_stack);

    if (! next_scope_item (last_scope_block (& tcb-> scope_stack)))
      {
        skip_block (thread);
        destroy_scope_block (& tcb-> scope_stack);
      }
}


/*********************   SKIP BLOCK IF CONDITION FALSE   *********************/

MODULE skip_block_if_condition_false (THREAD *thread)
{
    Bool
        where_condition;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    must_be_defined (tcb-> result_node-> op1);
    if (exception_raised)
        return;

    where_condition = (Bool) number_value (& tcb-> result_node-> op1-> value);
    if (! where_condition)
        skip_block (thread);
}


/************************   GENERATE CONDITION EVENT   ***********************/

MODULE generate_condition_event (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    must_be_defined (tcb-> result_node-> op1);
    if (exception_raised)
        return;

    if (number_value (& tcb-> result_node-> op1-> value))
        the_next_event = true_event;
    else
        the_next_event = false_event;
}


/**************************   ECHO TEXT TO CONSOLE   *************************/

MODULE echo_text_to_console (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    must_be_defined (tcb-> result_node-> op1);
    if (exception_raised)
        return;

    send_smtoper_info (
        &operq,
        strprintf ("%s M: %s", 
                   tcb-> me,
                   tcb-> script_node-> op1
                       ? string_value (& tcb-> result_node-> op1-> value)
                       : ""));
}


/*****************************   COPY XML ITEM   *****************************/

MODULE copy_xml_item (THREAD *thread)
{
    XML_ITEM
        *parent,
        *sibling,
        *to_item,
        *from_item;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  copy <scope> as <as> [to <to> | before <before> | after <after>] */

    must_be_defined (tcb-> result_node-> scope);
    must_be_defined (tcb-> result_node-> as);
    must_be_defined (tcb-> result_node-> to);
    must_be_defined (tcb-> result_node-> before);
    must_be_defined (tcb-> result_node-> after);
    if (exception_raised)
        return;

    from_item = lookup_from_xml (tcb-> result_node-> scope);
    if (! from_item)
        return;

    if (parse_locator (& parent, & sibling))
        return;

    to_item = xml_create (tcb-> script_root-> as
                              ? string_value (& tcb-> result_node-> as-> value)
                              : xml_item_name (from_item),
                          NULL);
    ASSERT (to_item);
    xml_copy (to_item, from_item);

    if (sibling)
        xml_attach_sibling (sibling, to_item);
    else
        xml_attach_child   (parent,  to_item);
}


/****************************   DELETE XML ITEM   ****************************/

MODULE delete_xml_item (THREAD *thread)
{
    XML_ITEM
        *xml_delete;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  .delete <scope>  */

    must_be_defined (tcb-> result_node-> scope);
    if (exception_raised)
        return;

    xml_delete = lookup_from_xml (tcb-> result_node-> scope);
    if (! xml_delete)
        return;
    remove_scope_references_to_xml (xml_delete);
    xml_free (xml_delete);
}


/*****************************   MOVE XML ITEM   *****************************/

MODULE move_xml_item (THREAD *thread)
{
    XML_ITEM
        *parent,
        *sibling,
        *xml_item,
        *xml_scan;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  .move <scope> as <as> [to <to> | before <before> | after <after>] */

    must_be_defined (tcb-> result_node-> scope);
    must_be_defined (tcb-> result_node-> as);
    must_be_defined (tcb-> result_node-> to);
    must_be_defined (tcb-> result_node-> before);
    must_be_defined (tcb-> result_node-> after);
    if (exception_raised)
        return;

    xml_item = lookup_from_xml (tcb-> result_node-> scope);
    if (! xml_item)
        return;

    if (parse_locator (& parent, & sibling))
        return;

    xml_scan = parent;
    while (xml_scan)
      {
        if (xml_scan == xml_item)
          {
            report_error (error_event, "Attempt to make object its own child.");
            return;
          }
        xml_scan = xml_parent (xml_scan);
      }

    if (sibling == xml_item)
        sibling = xml_next_sibling (sibling);

    xml_detach (xml_item);
    if (sibling)
        xml_attach_sibling (sibling, xml_item);
    else
        xml_attach_child   (parent,  xml_item);

    if (tcb-> script_root-> as)
        xml_rename (xml_item,
                    string_value (& tcb-> result_node-> as-> value));
}


/*****************************   SAVE XML FILE   *****************************/

MODULE save_xml_file (THREAD *thread)
{
    char
        *filename;
    XML_ITEM
        *xml_item;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  .save <scope> [as <as>]  */

    must_be_defined (tcb-> result_node-> scope);
    must_be_defined (tcb-> result_node-> as);
    if (exception_raised)
        return;

    xml_item = lookup_from_xml (tcb-> result_node-> scope);
    if (! xml_item)
        return;

    if (tcb-> script_root-> as)
        filename = string_value (& tcb-> result_node-> as-> value);
    else
      {
        filename = mem_alloc
                       (strlen (string_value (& tcb-> result_node-> scope-> value)) + 5);
        strcpy (filename, tcb-> result_node-> scope-> value. s);
        strcat (filename, ".xml");
      }
    xml_save_file (xml_item, filename);

    if (! tcb-> script_root-> as)
        mem_free (filename);
}


/*************************   INITIALISE SCOPE BLOCK   ************************/

MODULE initialise_scope_block (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;
    XML_ITEM
        *xml_item;
    RESULT_NODE
        *last_name;
    char
        *alias;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  .scope <scope> as <as>  */
    must_be_defined (tcb-> result_node-> scope);
    must_be_defined (tcb-> result_node-> as);
    if (exception_raised)
        return;

    /*  Find the item referred to.  */
    xml_item = lookup_from_xml (tcb-> result_node-> scope);
    if (! xml_item)
        return;

    if (tcb-> result_node-> as)
      {
        string_value (& tcb-> result_node-> as-> value);
        if (tcb-> result_node-> as-> value. s [0] != 0)
            alias = tcb-> result_node-> as-> value. s;
        else
            alias = NULL;
      }
    else
      {
        /*  Find the last name in the extended scope.  */
        last_name = tcb-> result_node-> scope;
        if (last_name-> script_node-> type == GG_MEMBER)
            last_name = last_name-> op2;

        alias = string_value (& last_name-> value);
      }

    if ((! alias)
    && (! tcb-> script_node-> stacked))
      {
        report_unaliased_unstacked_scope ();
        return;
      }

    scope_block = create_scope_block (& tcb-> scope_stack, 
                                      GG_SCOPE, alias);
    scope_block-> stacked  = tcb-> script_node-> stacked;
    scope_block-> xml_item = xml_item;
}


static void
report_unaliased_unstacked_scope (void)
{
    report_error (error_event, 
                  "Attempt to created an unstacked scope with no alias.");
}


/****************************   CLOSE THE SCOPE   ****************************/

MODULE close_the_scope (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    destroy_scope_block (& tcb-> scope_stack);
}


/**************************   INITIALISE NEW BLOCK   *************************/

MODULE initialise_new_block (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;
    XML_ITEM
        *parent,
        *sibling,
        *xml_item;
    char
        *name,
        *alias;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  .new [[<scope>] .] <name> as <as>
                               [to <to> | before <before> | after <after>]   */

    must_be_defined (tcb-> result_node-> scope);
    must_be_defined (tcb-> result_node-> name);
    must_be_defined (tcb-> result_node-> as);
    must_be_defined (tcb-> result_node-> to);
    must_be_defined (tcb-> result_node-> before);
    must_be_defined (tcb-> result_node-> after);
    if (exception_raised)
        return;

    /*  This is where we fudge the to field by setting it from the scope     */
    /*  field, allowing the alternative forms of the .new statement.         */
    if (tcb-> result_node-> scope && tcb-> result_node-> to)
      {
        report_error (error_event, 
                      "Only one of 'scope' and 'to' may be specified.");
        return;
      }
    if (! tcb-> result_node-> to)
      {
        tcb-> result_node-> to    = tcb-> result_node-> scope;
        tcb-> result_node-> scope = NULL;
      }

    if (parse_locator (& parent, & sibling))
        return;
   
    if (tcb-> script_root-> name)
        name = string_value (& tcb-> result_node-> name-> value);
    else
        name = NULL;

    if (tcb-> result_node-> as)
      {
        string_value (& tcb-> result_node-> as-> value);
        if (tcb-> result_node-> as-> value. s [0] != 0)
            alias = tcb-> result_node-> as-> value. s;
        else
            alias = NULL;
      }
    else
        alias = name;

    if ((! alias)
    && (! tcb-> script_node-> stacked))
      {
        report_unaliased_unstacked_scope ();
        return;
      }

    xml_item = xml_create (name, NULL);
    ASSERT (xml_item);

    if (sibling)
        xml_attach_sibling (sibling, xml_item);
    else
        xml_attach_child   (parent,  xml_item);

    scope_block = create_scope_block (& tcb-> scope_stack, 
                                      GG_NEW, alias);
    scope_block-> stacked  = tcb-> script_node-> stacked;
    scope_block-> xml_item = xml_item;
}


/***********************   REGISTER MACRO OR FUNCTION   **********************/

MODULE register_macro_or_function (THREAD *thread)
{
    SYMBOL
        *symbol;
    char
        *scope;
    SYMTAB
        **macros = NULL;
    MEMTRN
        **memtrn;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    must_be_defined (tcb-> result_node-> scope);
    must_be_defined (tcb-> result_node-> name);
    if (exception_raised)
        return;

    if (tcb-> script_node-> scope)
      {
        scope = string_value (& tcb-> result_node-> scope-> value);

        if (lexcmp (scope, "global") == 0)
          {
            macros = & global_macros;
            memtrn = & global_memtrn;
          }
        else
          {
            report_error (error_event,
                          "Unknown macro/function scope: %s",
                          tcb-> result_node-> scope-> value.s);
            return;
          }
      }
    else
      {
        macros = & tcb-> macros;
        memtrn = & smt_memtrn_;
      }

    symbol = sym_assume_symbol (*macros,
                                tcb-> result_node-> name-> value.s,
                                NULL);
    ASSERT (symbol);

    if (symbol-> data)                  /*  Macro already defined  */
      {
        report_error (error_event, "Macro/function %s%s%s already exists.",
                      tcb-> script_node-> scope
                          ? tcb-> result_node-> scope-> value.s : "",
                      tcb-> script_node-> scope 
                          ? "." : "",
                      tcb-> result_node-> name-> value.s);
        return;
      }

    symbol-> data = memt_alloc (*memtrn, sizeof (SCRIPT_MACRO));
    ASSERT (symbol-> data);

    ((SCRIPT_MACRO *) (symbol-> data))-> position = tcb-> script_line;
    ((SCRIPT_MACRO *) (symbol-> data))-> args     = tcb-> script_node-> op2;

    tcb-> script_line-> macros++;       /*  Macro definition is a link       */
    tcb-> script_source-> macros++;
}


/***************************   CALL EVALUATE CALL   **************************/

MODULE call_evaluate_call (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    init_result_node (& tcb-> call_result);
    call_exception (evaluate_call_event);
}


/***************************   PICKUP CALL RESULT   **************************/

MODULE pickup_call_result (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    copy_result (tcb-> result_node, & tcb-> call_result);
    mem_free (tcb-> call_result. value. s);
    mem_free (tcb-> call_result. value. b);

    /*  And reset call result to null  */
    init_result_node (& tcb-> call_result);
}


/***************************   SAVE RETURN VALUE   ***************************/

MODULE save_return_value (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> result_node-> op1)
        copy_result (& tcb-> call_result,
                     tcb-> result_node-> op1);
}


/***************************   CLOSE MACRO BLOCK   ***************************/

MODULE close_macro_block (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;
    XML_ITEM
        *xml_item;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Clean up any intermediate scopes (after a .return)  */
    scope_block = last_scope_block (& tcb-> scope_stack);
    while (scope_block && scope_block-> scope_type != GG_MACRO)
      {
        destroy_scope_block (& tcb-> scope_stack);
        scope_block = last_scope_block (& tcb-> scope_stack);
      }

    /*  No scope block can only mean return with call.  ggscrp picks up     */
    /*  mismatched .endmacro/.endfunction                                   */
    if (! scope_block)
      {
        report_error (error_event, "Return without call.");
        return;
      }

    /*  Free xml parameter block  */
    if (scope_block-> xml_item)
      {
        xml_item = scope_block-> xml_item;
        remove_scope_references_to_xml (xml_item);
        xml_free (xml_item);
      }

    /*  And finally destroy the macro block.  */
    destroy_scope_block (& tcb-> scope_stack);
}


/**************************   POP SCRIPT POSITION   **************************/

MODULE pop_script_position (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_the_script ();
}


static void
pop_the_script (void)
{
    if (! list_empty (& tcb-> script_stack))
      {
        list_pop (& tcb-> script_stack, tcb-> script_line);
        if (tcb-> script_line)
          {
            tcb-> script_source = tcb-> script_line-> parent;

            xml_put_attr (tcb-> xml_gsl,
                          "script",
                          tcb-> script_line-> parent-> name);
          }
      }
    else
      {
/*          tcb-> script_line   = NULL; */
/*          tcb-> script_source = NULL; */
        raise_exception (script_stack_empty_event);
      }
}


/***************************   PUSH CURRENT NODE   ***************************/

MODULE push_current_node (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> result_node)
      {
        tcb-> result_node-> next = tcb-> node_stack;
        tcb-> node_stack = tcb-> result_node;
      }
}


/**************************   GET FIRST PARAMETER   **************************/

MODULE get_first_parameter (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    while ((tcb-> script_node-> op1)
       &&  (tcb-> script_node-> op1-> type     == GG_OPERATOR)
       &&  (tcb-> script_node-> op1-> operator == OP_NEXT_ARG))
        tcb-> script_node = tcb-> script_node-> op1;

    tcb-> evaluate_node = tcb-> script_node-> op1;

    tcb-> result_node-> item_nbr = 0;

    get_the_parameter ();
}


static void
get_the_parameter ()
{
    GSL_FUNCTION
        *gsl_function;
    int
        parmn;
    static PARM_TYPE parm_type_event [] = {
        value_event,
        reference_event,
        simple_scope_event,
        extended_scope_event,
        expression_event };
    PARM_TYPE
        parm_type;

    tcb-> result_node-> script_node = tcb-> script_node;
    gsl_function = tcb-> result_node-> gsl_function;
    if (tcb-> result_node-> item_nbr >= tcb-> result_node-> argc)
        the_next_event = none_event;
    else
      {
        parmn = tcb-> result_node-> item_nbr;
        if (parmn >= gsl_function-> cnt_parmt)
            parmn = gsl_function-> cnt_parmt - 1;
        parm_type = (*gsl_function-> parmt) [parmn];
        if (parm_type >= PARM_VALUE
        &&  parm_type <= PARM_EXPRESSION)
            the_next_event = parm_type_event [parm_type];
        else
            raise_exception (anomaly_event);
      }
}


/*************************   INITIALISE MACRO BLOCK   ************************/

MODULE initialise_macro_block (THREAD *thread)
{
    XML_ITEM
        *xml_item;
    SCOPE_BLOCK
        *scope_block;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /* [<scope> .]  <name> [(<op1>)] */

    xml_item = xml_new (tcb-> xml_junk, "", "");
    scope_block = create_scope_block
                      (& tcb-> scope_stack,
                       GG_MACRO, 
                       string_value (& tcb-> result_node-> name-> value));

    scope_block-> stacked  = FALSE;     /*  Macro/f'n scopes aren't stacked  */
    scope_block-> xml_item = xml_item;

    /*  Create unstacked alias 'my' for macro scope block.                   */
    scope_block = create_scope_block (& tcb-> scope_stack, 
                                      GG_UNDEFINED, "my");
    scope_block-> stacked  = FALSE;
    scope_block-> xml_item = xml_item;

    if (substitute_parameters_into_xml (xml_item,
                                        tcb-> result_node-> macro-> args,
                                        tcb-> result_node-> op1))
        return;

    tcb-> script_line   = tcb-> result_node-> macro-> position;
    tcb-> script_source = tcb-> script_line-> parent;
}


static int
substitute_parameters_into_xml (XML_ITEM *xml_item,
                                SCRIPT_NODE *args,
                                RESULT_NODE *params)
{
    SCRIPT_NODE
        *arg = args;
    RESULT_NODE
        *param = params,
        *param_parent = NULL;           /*  Keep in case child is NULL       */
    int
        argn,
        paramn;

    if (! params)
        return 0;

    paramn = 1;
    while (param
       && (param-> script_node-> type     == GG_OPERATOR)
       && (param-> script_node-> operator == OP_NEXT_ARG))
      {
        param_parent = param;
        param = param-> op1;
        paramn++;
      }

    if (! arg)
        argn = 0;
    else
        argn = 1;

    while (arg
       && (arg-> type     == GG_OPERATOR)
       && (arg-> operator == OP_NEXT_ARG))
      {
        arg = arg-> op1;
        argn++;
      }

    /*  Must be at least as many args as params  */
    if (argn < paramn)
      {
        pop_the_script ();
        pop_the_result_node ();
        report_error (error_event, "Mismatched parameters.");
        return -1;
      }

    substitute_parameter_into_xml (xml_item, arg, param);
    while (param != params)
      {
        param = param_parent;
        param_parent = param-> parent;
        arg = arg-> parent;
        substitute_parameter_into_xml (xml_item, arg-> op2, param-> op2);
      }
    return 0;
}


static void
substitute_parameter_into_xml (XML_ITEM *xml_item,
                               SCRIPT_NODE *arg,
                               RESULT_NODE *param)
{
    char
        *name;

    name = (char *) arg-> op1;          /*  Don't evaluate simple text       */
    ASSERT (name);
    xml_put_attr_ic (xml_item,
                     name,
                     param
                     ? (param-> value. type != TYPE_UNDEFINED
                        ? string_value (& param-> value)
                        : NULL)
                     : NULL,
                     tcb-> ignorecase,
                     TRUE);
}


/**************************   BUILD FAKE FOR NODE   *************************/

MODULE build_fake_for_node (THREAD *thread)
{
    int
        argc;
    SCRIPT_NODE
        *item_node,
        *where_node = NULL;
    RESULT_NODE
        *as_node    = NULL;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Manually free the function name value - otherwise it gets lost       */
    destroy_result (tcb-> result_node-> name);

    argc = tcb-> result_node-> argc;

    if (argc == 0)
      {
        report_error (error_event, "Missing parameter for function 'count'.");
        return;
      }

    item_node  =            tcb-> result_node-> argv [0]-> script_node;
    where_node = argc > 1 ? tcb-> result_node-> argv [1]-> script_node : NULL;
    if (argc > 2)
      {
        /*  If there was a third, empty argument, create a null string       */
        /*  result.  This will force an unaliased scope.                     */
        as_node = tcb-> result_node-> argv [2];
        if (! as_node)
          {
            as_node = new_result_node ();
            ASSERT (as_node);
            init_result_node (as_node);
            as_node-> value. type = TYPE_STRING;
            as_node-> value. s    = mem_strdup ("");
          }
      }
    else
        as_node = NULL;

    if ((item_node-> type != GG_SYMBOL)
    &&  (item_node-> type != GG_OPERAND))
      {
        report_error (error_event, "Illegal count item.");
        return;
      }

    tcb-> fake_for_node = mem_alloc (sizeof (SCRIPT_NODE));
    ASSERT (tcb-> fake_for_node);
    init_script_node (tcb-> fake_for_node);

    tcb-> fake_for_node-> scope  = item_node-> scope;
    tcb-> fake_for_node-> name   = item_node-> type == GG_SYMBOL
                                       ? item_node-> name : item_node-> op1;
    
    tcb-> fake_for_node-> op1     = where_node;
    tcb-> fake_for_node-> stacked = TRUE;

    tcb-> result_node-> as = as_node;
    
    tcb-> result_node-> script_node = tcb-> fake_for_node;
    tcb-> script_node = tcb-> fake_for_node;
}


/**************************   SAVE TOTAL AS RESULT   *************************/

MODULE save_total_as_result (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    scope_block = last_scope_block (& tcb-> scope_stack);

    tcb-> call_result. value. type = TYPE_NUMBER;
    tcb-> call_result. value. n    = scope_block-> total;
}


/*************************   DESTROY FAKE FOR NODE   *************************/

MODULE destroy_fake_for_node (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    mem_free (tcb-> fake_for_node);
    tcb-> fake_for_node = NULL;
}


/************************   EVALUATE SUBSTITUTE NODE   ***********************/

MODULE evaluate_substitute_node (THREAD *thread)
{
    char
        buffer [LINE_MAX + 1],
        *example = NULL;
    int
        length,
        space;
    SCRIPT_NODE
        *symbol_node,
        *parent;
    RESULT_NODE
        *symbol_result_node;
    char
        *error_text;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  'op2' is pretty-print specifier, 'as' is format string  */
    must_be_defined (tcb-> result_node-> op2);
    must_be_defined (tcb-> result_node-> as);
    if (exception_raised)
        return;

    tcb-> result_node-> constant =           tcb-> result_node-> op1   -> constant
            && (tcb-> script_node-> op2 ? tcb-> result_node-> op2-> constant : TRUE)
            && (tcb-> script_node-> as ? tcb-> result_node-> as-> constant : TRUE);

    copy_result (tcb-> result_node, tcb-> result_node-> op1);
        
    if (tcb-> result_node-> value. type == TYPE_UNDEFINED)
      {
        /*  A top-level default operator with no second operand means don't  */
        /*  panic about NULL                                                 */
        if ((tcb-> script_node-> op1-> type     == GG_OPERATOR)
        &&  (tcb-> script_node-> op1-> operator == OP_DEFAULT)
        &&  (tcb-> script_node-> op1-> op2      == NULL))
          {
            tcb-> result_node-> value. type = TYPE_UNKNOWN;
            tcb-> result_node-> value. s    = mem_strdup ("");
          }
        else
          {
            tcb-> result_node-> culprit = tcb-> result_node-> op1-> culprit;
            return;
          }
      }

    /*  Only match the case if ignorecase is TRUE and the expression         */
    /*  consists of a single identifier, possibly with a default clause.     */
    if (tcb-> result_node-> value. type != TYPE_NUMBER)
      {
        if (tcb-> ignorecase)
          {
            symbol_node        = tcb-> script_node-> op1;
            symbol_result_node = tcb-> result_node-> op1;
            if ((symbol_node-> type     == GG_OPERATOR)
            &&  (symbol_node-> operator == OP_DEFAULT))
              {
                symbol_node        = symbol_node       -> op1;
                symbol_result_node = symbol_result_node-> op1;
              }

            if ((symbol_node-> type == GG_SYMBOL)
            &&  (symbol_node-> name))
                example = string_value (& symbol_result_node-> name-> value);
          }

        /*  Calculate the space for this node by adding the length of the   */
        /*  GSL construct to the number of spaces in the next concatenated  */
        /*  node minus the shuffle parameter.                               */
        space = tcb-> script_node-> width;

        parent = tcb-> script_node;
        while (parent-> parent
           &&  parent-> parent-> type     == GG_OPERATOR
           &&  parent-> parent-> operator == OP_UNDEFINED
           &&  parent == parent-> parent-> op2)
            parent = parent-> parent;

        if (parent
        &&  parent-> type     == GG_OPERATOR
        &&  parent-> operator == OP_UNDEFINED
        &&  parent-> op2
        &&  parent-> op2-> spaces >= tcb-> shuffle)
            space += parent-> op2-> spaces
                   - tcb-> shuffle;
        else
        /*  This is a tricky but important case that looks something like:  */
        /*      $(attr:block)\                                              */
        /*                .                                                 */
        if (tcb-> script_node-> extend
        &&  tcb-> script_line-> next
        &&  tcb-> script_line-> next-> node-> type == GG_LINE)
          {
            space = tcb-> script_line-> next-> node-> op1-> spaces
                  - tcb-> script_node-> spaces
                  - tcb-> shuffle;

            /*  Check for preceding stuff on the same line                  */
            if (tcb-> script_node-> parent
            &&  tcb-> script_node-> parent-> type     == GG_OPERATOR
            &&  tcb-> script_node-> parent-> operator == OP_UNDEFINED
            &&  tcb-> script_node == tcb-> script_node-> parent-> op2)
                space -= strlen (string_value (& tcb-> result_node-> parent-> op1-> value));
          }

        if (pretty_print (& tcb-> result_node-> value   ,
                          tcb-> result_node-> op2, 
                          example,
                          space,
                          &error_text))
          {
            report_error (error_event, "%s", error_text);
            return;
          }
      }

    if (tcb-> script_node-> as)
      {
        length = format_output (buffer, LINE_MAX, 
                                tcb-> result_node, &error_text);
        if (length < 0)
          {
            report_error (error_event, "%s", error_text);
            return;
          }

        mem_free (tcb-> result_node-> value. s);
        tcb-> result_node-> value. s = mem_alloc (length + 1);
        ASSERT (tcb-> result_node-> value. s);
        strcpy (tcb-> result_node-> value. s, buffer);
        tcb-> result_node-> value. type = TYPE_UNKNOWN;
      }
}


/*************************   EVALUATE LITERAL NODE   *************************/

MODULE evaluate_literal_node (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_node-> constant = tcb-> result_node-> op1-> constant;
    if (tcb-> result_node-> op1-> value. type != TYPE_UNDEFINED)
      {
        string_value (& tcb-> result_node-> op1-> value);
        tcb-> result_node-> value. type = TYPE_STRING;
        tcb-> result_node-> value. s    = mem_alloc 
           (tcb-> result_node-> op1-> indent +
            strlen (tcb-> result_node-> op1-> value. s) + 1);
        memset (tcb-> result_node-> value. s, ' ',
                tcb-> result_node-> op1-> indent);
        strcpy (tcb-> result_node-> value. s
              + tcb-> result_node-> op1-> indent, 
                tcb-> result_node-> op1-> value. s);
      }
    else
      tcb-> result_node-> culprit = tcb-> result_node-> op1-> culprit;
}


/***************************   CALL EVALUATE OP2   ***************************/

MODULE call_evaluate_op2 (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> op2)
      {
        tcb-> evaluate_node =   tcb-> script_node-> op2;
        tcb-> result_ptr    = & tcb-> result_node-> op2;
        call_exception (evaluate_event);
      }
}


/**************************   EVALUATE NUMBER NODE   *************************/

MODULE evaluate_number_node (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_node-> constant = 
                     (tcb-> result_node-> op1 ? tcb-> result_node-> op1-> constant : TRUE)
                  && (tcb-> result_node-> op2 ? tcb-> result_node-> op2-> constant : TRUE);
        
    tcb-> result_node-> value. s = mem_alloc (
                         (tcb-> result_node-> op1 ? strlen (tcb-> result_node-> op1-> value. s) : 1)
                       + (tcb-> script_node-> operator == OP_MINUS ? 1 : 0)
                       + (tcb-> result_node-> op2 ? strlen (tcb-> result_node-> op2-> value. s) + 1 : 0)
                       + 1);
    ASSERT (tcb-> result_node-> value. s);
    tcb-> result_node-> value. s [0] = '\0';
    if (tcb-> script_node-> operator == OP_MINUS)
        strcat (tcb-> result_node-> value. s, "-");
    strcat (tcb-> result_node-> value. s, tcb-> result_node-> op1 ? tcb-> result_node-> op1-> value. s : "0");
    if (tcb-> script_node-> op2)
      {
        strcat (tcb-> result_node-> value. s, ".");
        strcat (tcb-> result_node-> value. s, tcb-> result_node-> op2-> value. s);
      }
    tcb-> result_node-> value. type = TYPE_UNKNOWN;

    number_value (& tcb-> result_node-> value);
}


/******************   CALL PARSE OP1 TO OPERAND EXCEPTION   ******************/

MODULE call_parse_op1_to_operand_exception (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> operand_node = NULL;
    if (tcb-> result_node-> op1-> value. type != TYPE_UNDEFINED)
        call_exception (parse_op1_to_operand_event);
    else
        tcb-> result_node-> culprit = tcb-> result_node-> op1-> culprit;
}


/**************************   PARSE OP1 TO OPERAND   *************************/

MODULE parse_op1_to_operand (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    gg_parse_expression (string_value (& tcb-> result_node-> op1-> value), 0,
                         tcb-> scratch_memtrn,
                         thread-> queue);
}


/**********************   SAVE PARSE RESULT TO OPERAND   *********************/

MODULE save_parse_result_to_operand (THREAD *thread)
{
    struct_ggpars_ok
        *parse_result;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Get parse tree from parser agent message                             */
    get_ggpars_ok (thread-> event-> body, & parse_result);
    tcb-> operand_node = (SCRIPT_NODE *) parse_result-> parse_root;
    free_ggpars_ok (& parse_result);
}


/****************************   LOG PARSE ERROR   ****************************/

MODULE log_parse_error (THREAD *thread)
{
    struct_ggpars_error
        *parse_error;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    report_error (_LR_NULL_EVENT, "Error in operand: %s", 
                  tcb-> result_node-> op1-> value. s);

    /*  Get parse tree from parser agent message                             */
    get_ggpars_error (thread-> event-> body, & parse_error);
    report_error (_LR_NULL_EVENT, "%s", parse_error-> error_text);
    free_ggpars_error (& parse_error);
}


/*************************   CALL EVALUATE OPERAND   *************************/

MODULE call_evaluate_operand (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> operand_node)
      {
        tcb-> evaluate_node = tcb-> operand_node;
        tcb-> result_ptr    = tcb-> result_node ? & tcb-> result_node-> operand : & tcb-> result_root;
        call_exception (evaluate_event);
      }
}


/*************************   PICKUP OPERAND RESULT   *************************/

MODULE pickup_operand_result (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> result_node-> operand)
      {
        copy_result (tcb-> result_node, tcb-> result_node-> operand);
        tcb-> result_node-> indent   =  tcb-> result_node-> operand-> indent;
        tcb-> result_node-> constant =  tcb-> result_node-> op1    -> constant
                                     && tcb-> result_node-> operand-> constant;
      }
}


/**************************   EVALUATE SYMBOL NODE   *************************/

MODULE evaluate_symbol_node (THREAD *thread)
{
    char
        *error_text;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    must_be_defined (tcb-> result_node-> scope);
    must_be_defined (tcb-> result_node-> name);
    if (exception_raised)
        return;

    tcb-> result_node-> value. s = mem_strdup (
                      symbol_value (& tcb-> scope_stack,
                                                          tcb-> result_node,
                                                          tcb-> ignorecase,
                                                          &error_text));

    if (error_text)
        report_error (error_event, "%s", error_text);

    if (tcb-> result_node-> value. s)
        tcb-> result_node-> value. type = TYPE_UNKNOWN;
    else
        tcb-> result_node-> culprit = memt_strdup
                                          (tcb-> scratch_memtrn,
                                           name_the_symbol (tcb-> result_node));
    
}


/**************************   EVALUATE MEMBER NODE   *************************/

MODULE evaluate_member_node (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> op2
    &&  tcb-> result_node-> op2-> value. type == TYPE_UNDEFINED)
    tcb-> result_node-> culprit = tcb-> result_node-> op2-> culprit;
    else
    if (tcb-> script_node-> op1
    && tcb-> result_node-> op1-> value. type == TYPE_UNDEFINED)
    tcb-> result_node-> culprit = tcb-> result_node-> op1-> culprit;
    else
        tcb-> result_node-> value. type = TYPE_UNKNOWN;
}


/***************************   EVALUATE TEXT NODE   **************************/

MODULE evaluate_text_node (THREAD *thread)
{
    int
        length,
        chunklen,
        i;
    char
        *in,
        *ptr,
        *out,
        *sign;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    sign = operator_text (tcb-> script_node-> operator);

    /*  Count total string length including line terminators  */
    length = strlen (sign) + tcb-> script_node-> line_break;
    in = (char *) tcb-> script_node-> op1;
    while (in)
      {
        ptr = strchr (in, '\\');
        if (ptr)
          {
            length += ptr - in + 1;
            ptr += 2;
          }
        else
            length += strlen (in);

        in = ptr;
      }

    tcb-> result_node-> value. type = TYPE_UNKNOWN;
    tcb-> result_node-> value. s = mem_alloc (length + 1);

    strcpy (tcb-> result_node-> value. s, sign);
    out = tcb-> result_node-> value. s + strlen (sign);

    in = (char *) tcb-> script_node-> op1;
    while (in)
      {
        ptr = strchr (in, '\\');
        if (ptr)
          {
            memcpy (out, in, ptr - in);
            out += ptr++ - in;
            if (*ptr == 'n')
                *out++ = '\n';
            else
            if (*ptr == 'r')
                *out++ = '\r';
            else
            if (*ptr == 't')
                *out++ = '\t';
            else
                *out++ = *ptr;

            ptr++;
          }
        else
          {
            chunklen = strlen (in);
            memcpy (out, in, chunklen);
            out += chunklen;
          }
        in = ptr;
      }

    for (i = 0; i < tcb-> script_node-> line_break; i++)
        *out++ = '\n';

    *out = 0;

    tcb-> result_node-> constant = TRUE;
}


/********************   DEFINE SYMBOL AS NODE OP2 RESULT   *******************/

MODULE define_symbol_as_node_op2_result (THREAD *thread)
{
    char
        *error_text;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  An undefined result gives an error except in three specific cases:   */
    /*      1. '.define x ='                                                 */
    /*      2. '.define x = y ?'                                             */
    /*      2. '.define x ?= y ?'                                            */
    if ((tcb-> script_root-> op2)                           /*  Case 1      */
    &&  (tcb-> result_node-> op2-> value. type == TYPE_UNDEFINED)
    &&  ! (((tcb-> script_root-> operator == OP_UNDEFINED)  /*  Case 2      */
        ||  (tcb-> script_root-> operator == OP_DEFAULT))   /*  Case 3      */
        && ((tcb-> script_root-> op2-> type     == GG_OPERATOR)
        &&  (tcb-> script_root-> op2-> operator == OP_DEFAULT)
        &&  (tcb-> script_root-> op2-> op2      == NULL)) ))
      {
        report_undefined_expression (tcb-> result_node-> op2);
        return;
      }

    if (! store_symbol_definition (& tcb-> scope_stack,
                                   tcb-> ignorecase,
                                   tcb-> result_node,
                                   tcb-> result_node-> op2
                                     ? & tcb-> result_node-> op2-> value
                                       : NULL,
                                   & error_text))
        report_error (error_event, error_text);
}


static void
report_undefined_expression (RESULT_NODE *node)
{
    char
        *error_text;

    undefined_expression_error (node, & error_text);
    report_error (error_event, error_text);
}


/***********************   PREPARE DEFINE EXPRESSION   ***********************/

MODULE prepare_define_expression (THREAD *thread)
{
    RESULT_NODE
        *new_node;
    XML_ITEM
        *xml_father;
    char
        *error_text;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  <name>.<scope> [operator]= <op2>  */

    must_be_defined (tcb-> result_node-> name);
    must_be_defined (tcb-> result_node-> scope);
    if (exception_raised)
        return;

    xml_father = symbol_xml_parent (& tcb-> scope_stack,
                                    tcb-> result_node,
                                    tcb-> ignorecase,
                                    &error_text);
    if (! xml_father)
      {
        report_error (error_event, error_text);
        return;
      }

    /*  Asignment with an operator must have an operand                  */
    if (! tcb-> script_root-> op2)
      {
        report_error (error_event, "Missing operand.");
        return;
      }

    new_node = new_result_node ();
    ASSERT (new_node);
    init_result_node (new_node);

    new_node-> parent = tcb-> result_node;
    tcb-> result_node-> op1 = new_node;

    init_value_node (new_node-> value);

    if (tcb-> result_node-> name)
        new_node-> value. s = mem_strdup (
        xml_get_attr_ic (xml_father,
                 string_value (& tcb-> result_node-> name-> value),
                 NULL,
                 tcb-> ignorecase));
    else
        new_node-> value. s = mem_strdup
            (compound_item_value (xml_father));

    if (new_node-> value. s)
        new_node-> value. type = TYPE_UNKNOWN;
    else
      {
        new_node-> value. type = TYPE_UNDEFINED;
        new_node-> culprit     = memt_strdup (tcb-> scratch_memtrn,
                                              tcb-> result_node-> culprit);
      }
    new_node-> constant = TRUE;   /* We already have the result */
}


static void
must_be_defined (RESULT_NODE *node)
{
    if (node)
        if (node-> value. type == TYPE_UNDEFINED)
            report_undefined_expression (node);
}


/**********************   EVALUATE ARITHMETIC OPERATOR   *********************/

MODULE evaluate_arithmetic_operator (THREAD *thread)
{
    int
        length;
    long
        i;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_node-> constant = 
        (tcb-> result_node-> op1 ? tcb-> result_node-> op1-> constant : TRUE)
     && (tcb-> result_node-> op2 ? tcb-> result_node-> op2-> constant : TRUE);

    if ((tcb-> result_node-> op1)
    &&  (tcb-> result_node-> op1-> value. type == TYPE_UNDEFINED))
      {
        tcb-> result_node-> culprit = tcb-> result_node-> op1-> culprit;
        return;
      }
    if (tcb-> result_node-> op2-> value. type == TYPE_UNDEFINED)
      {
        tcb-> result_node-> culprit = tcb-> result_node-> op2-> culprit;
        return;
      }

    if (tcb-> result_node-> op1)
      {
        /*  Binary operators  */

        /*  Handle PLUS to catch "x" + "y" = "xy"  */
        if (tcb-> script_node-> operator == OP_PLUS)
          {
            /*  If one operand is non-string and the other is unknown,       */
            /*  try to make both numeric.                                    */
            if ((tcb-> result_node-> op1-> value. type != TYPE_STRING)
            &&  (tcb-> result_node-> op2-> value. type == TYPE_UNKNOWN))
                number_value (& tcb-> result_node-> op2-> value);

            if ((tcb-> result_node-> op2-> value. type != TYPE_STRING)
            &&  (tcb-> result_node-> op1-> value. type == TYPE_UNKNOWN))
                number_value (& tcb-> result_node-> op1-> value);

            if ((tcb-> result_node-> op1-> value. type == TYPE_NUMBER)
            &&  (tcb-> result_node-> op2-> value. type == TYPE_NUMBER))
              {
                tcb-> result_node-> value. n
                    = tcb-> result_node-> op1-> value. n
                    + tcb-> result_node-> op2-> value. n;
                tcb-> result_node-> value. type = TYPE_NUMBER;
              }
            else
              {
                /*  Convert both to strings then concatenate.  */
                string_value (& tcb-> result_node-> op1-> value);
                string_value (& tcb-> result_node-> op2-> value);

                length = strlen (tcb-> result_node-> op1-> value. s)
                       + strlen (tcb-> result_node-> op2-> value. s);

                tcb-> result_node-> value. s = mem_alloc (length + 1);
                ASSERT (tcb-> result_node-> value. s);
                strcpy (tcb-> result_node->       value. s,
                        tcb-> result_node-> op1-> value. s);
                strcat (tcb-> result_node->       value. s,
                        tcb-> result_node-> op2-> value. s);

                tcb-> result_node-> value. type = TYPE_STRING;
              }
          }
        else
        /*  Handle TIMES to catch "x" * 3 = "xxx"  */
        if (tcb-> script_node-> operator == OP_TIMES)
          {
            /*  2nd operand must be numeric.  */
            number_value (& tcb-> result_node-> op2-> value);
            if (tcb-> result_node-> op2-> value. type != TYPE_NUMBER)
              {
                report_non_numeric_error (tcb-> result_node-> op2);
                return;
              }

            /*  Try to make 1st operand numeric, unless explicitly string.   */
            if (tcb-> result_node-> op1-> value. type != TYPE_STRING)
                number_value (& tcb-> result_node-> op1-> value);

            if (tcb-> result_node-> op1-> value. type == TYPE_STRING)
              {
                length = strlen (string_value (& tcb-> result_node-> op1-> value));
                i = (long) tcb-> result_node-> op2-> value. n;    /* Integer */
                if (i < 0)
                  {
                    report_error (error_event, 
                                  "String repeat count is negative.");
                    return;
                  }

                tcb-> result_node-> value. s = mem_alloc (length * i + 1);
                tcb-> result_node-> value. s [0] = '\0';
                ASSERT (tcb-> result_node-> value. s);
                while (i-- > 0)
                    strcat (tcb-> result_node-> value. s, tcb-> result_node-> op1-> value. s);
                tcb-> result_node-> value. type = TYPE_STRING;
              }
            else
              {
                tcb-> result_node-> value. n
                    = tcb-> result_node-> op1-> value. n
                    * tcb-> result_node-> op2-> value. n;
                tcb-> result_node-> value. type = TYPE_NUMBER;
              }
          }
        else
          {
            /*  Other operators besides '*', '+'  */

            /*  Operators must be numeric.  */
            number_value (& tcb-> result_node-> op1-> value);
            if (tcb-> result_node-> op1-> value. type != TYPE_NUMBER)
              {
                report_non_numeric_error (tcb-> result_node-> op1);
                return;
              }

            number_value (& tcb-> result_node-> op2-> value);
            if (tcb-> result_node-> op2-> value. type != TYPE_NUMBER)
              {
                report_non_numeric_error (tcb-> result_node-> op2);
                return;
              }

            switch (tcb-> script_node-> operator)
              {
                case OP_MINUS:
                    tcb-> result_node-> value. n
                        = tcb-> result_node-> op1-> value. n
                        - tcb-> result_node-> op2-> value. n;
                    break;

                case OP_DIVIDE: 
                    if (tcb-> result_node-> op2-> value. n != 0)
                        tcb-> result_node-> value. n = tcb-> result_node-> op1-> value. n
                                               / tcb-> result_node-> op2-> value. n;
                    else
                      {
                        report_error (error_event, "Division by zero.");
                        return;
                      }
                    break;

                case OP_OR:
                    tcb-> result_node-> value. n
                        = (double) ((Bool) tcb-> result_node-> op1-> value. n
                        ||          (Bool) tcb-> result_node-> op2-> value. n);
                    break;

                case OP_AND:
                    tcb-> result_node-> value. n
                        = (double) ((Bool) tcb-> result_node-> op1-> value. n
                        &&          (Bool) tcb-> result_node-> op2-> value. n);
                    break;

                default:
                    raise_exception (anomaly_event);
                    return;
              }
            tcb-> result_node-> value. type = TYPE_NUMBER;
          }
      }
    else
      {
        /*  Unary operators  */
        number_value (& tcb-> result_node-> op2-> value);
        if (tcb-> result_node-> op2-> value. type != TYPE_NUMBER)
          {
            report_non_numeric_error (tcb-> result_node-> op2);
            return;
          }
        switch (tcb-> script_node-> operator)
          {
            case OP_NOT:
                tcb-> result_node-> value. n
                    = (double) ! (Bool) tcb-> result_node-> op2-> value. n;
                break;

            case OP_PLUS:
                tcb-> result_node-> value. n
                    = tcb-> result_node-> op2-> value. n;
                break;

            case OP_MINUS:
                tcb-> result_node-> value. n
                    = - tcb-> result_node-> op2-> value. n;
                break;

            default:
                raise_exception (anomaly_event);
                return;
          }
        tcb-> result_node-> value. type = TYPE_NUMBER;
      }
}


static void
report_non_numeric_error (RESULT_NODE *node)
{
    report_error (error_event, "Illegal numeric operand: %s",
                  string_value (& node-> value));
}


/**********************   DEFINE SYMBOL AS NODE RESULT   *********************/

MODULE define_symbol_as_node_result (THREAD *thread)
{
    char
        *error_text;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  An undefined result gives an error.                                  */
    if (tcb-> result_node-> value. type == TYPE_UNDEFINED)
      {
        report_undefined_expression (tcb-> result_node);
        return;
      }

    if (! store_symbol_definition (& tcb-> scope_stack,
                                   tcb-> ignorecase,
                                   tcb-> result_node,
                                   & tcb-> result_node-> value,
                                   & error_text))
        report_error (error_event, error_text);
}


/******************   RAISE EXCEPTION IF SYMBOL IS DEFINED   *****************/

MODULE raise_exception_if_symbol_is_defined (THREAD *thread)
{
    XML_ITEM
        *xml_child,
        *xml_father;
    char
        *error_text;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    must_be_defined (tcb-> result_node-> name);
    must_be_defined (tcb-> result_node-> scope);
    if (exception_raised)
        return;

    xml_father = symbol_xml_parent (& tcb-> scope_stack,
                                    tcb-> result_node,
                                    tcb-> ignorecase,
                                    &error_text);
    if (! xml_father)
      {
        report_error (error_event, error_text);
        return;
      }

    if (tcb-> script_root-> name)
      {
        if (xml_attr_ic (xml_father,
                         string_value (& tcb-> result_node-> name-> value),
                         tcb-> ignorecase))
            raise_exception (exception_event);
      }
    else
      {
        xml_child = xml_first_child (xml_father);
        while (xml_child)
          {
            if (! xml_item_name (xml_child))
              {
                raise_exception (exception_event);
                return;
              }
            xml_child = xml_next_sibling (xml_child);
          }
      }
}


/**********************   REJECT IF WHERE CLAUSE FALSE   *********************/

MODULE reject_if_where_clause_false (THREAD *thread)
{
    Bool
        where_condition = TRUE;
    SCOPE_BLOCK
        *scope_block;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    scope_block = last_scope_block (& tcb-> scope_stack);

    must_be_defined (tcb-> result_node-> op1);
    if (exception_raised)
        return;

    if (tcb-> script_node-> op1)
      {
        where_condition = (Bool) number_value (& tcb-> result_node-> op1-> value);

        destroy_result (tcb-> result_node-> op1);
        tcb-> result_node-> op1 = NULL;

        if (! where_condition)
      {
            raise_exception (reject_event);
        return;
      }
      }
    scope_block-> total++;
}


/**************************   SAVE SORT KEY VALUE   **************************/

MODULE save_sort_key_value (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    scope_block = last_scope_block (& tcb-> scope_stack);

    must_be_defined (tcb-> result_node-> op2);
    if (exception_raised)
        return;

    if (tcb-> script_node-> op2)
      {
        /*  If type is unknown, see if it can be made a number.              */
        if ((tcb-> result_node-> op2-> value. type == TYPE_UNKNOWN)
        &&  (tcb-> sort_type                       == TYPE_UNKNOWN))
            number_value (& tcb-> result_node-> op2-> value);

        /*  If one sort key is a string, all must be treated as strings.     */
        if (tcb-> result_node-> op2-> value. type == TYPE_STRING)
            tcb-> sort_type = TYPE_STRING;

        copy_value (& scope_block-> scope_item-> sort_key,
            & tcb-> result_node-> op2-> value);

        destroy_result (tcb-> result_node-> op2);
        tcb-> result_node-> op2 = NULL;
      }
}


/*****************************   GET NEXT CHILD   ****************************/

MODULE get_next_child (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    scope_block = last_scope_block (& tcb-> scope_stack);

    scope_block-> xml_item = xml_next_sibling (scope_block-> xml_item);
    get_matching_child (scope_block);
}


/****************************   KILL SCOPE ITEM   ****************************/

MODULE kill_scope_item (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    scope_block = last_scope_block (& tcb-> scope_stack);

    destroy_scope_item (scope_block-> scope_item);
}


/**********************   CALL EVALUATE THE PARAMETER   **********************/

MODULE call_evaluate_the_parameter (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_ptr = & tcb-> result_node-> argv [tcb-> result_node-> item_nbr];
    if (tcb-> evaluate_node)
        call_exception (evaluate_event);
}


/*****************   CALL EVALUATE THE REFERENCE PARAMETER   *****************/

MODULE call_evaluate_the_reference_parameter (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_ptr = & tcb-> result_node-> argv [tcb-> result_node-> item_nbr];
    if (tcb-> evaluate_node)
        call_exception (evaluate_reference_event);
}


/************************   INSERT NULL RESULT NODE   ************************/

MODULE insert_null_result_node (THREAD *thread)
{
    RESULT_NODE
        *new_node;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    new_node = new_result_node ();
    ASSERT (new_node);
    init_result_node (new_node);

    new_node-> parent      = tcb-> result_node;
    new_node-> script_node = tcb-> evaluate_node;

    tcb-> result_node-> argv [tcb-> result_node-> item_nbr] = new_node;
}


/****************   CALL EVALUATE THE SIMPLE SCOPE PARAMETER   ***************/

MODULE call_evaluate_the_simple_scope_parameter (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_ptr = & tcb-> result_node-> argv [tcb-> result_node-> item_nbr];
    if (tcb-> evaluate_node)
        call_exception (evaluate_simple_scope_event);
}


/***************   CALL EVALUATE THE EXTENDED SCOPE PARAMETER   **************/

MODULE call_evaluate_the_extended_scope_parameter (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_ptr = & tcb-> result_node-> argv [tcb-> result_node-> item_nbr];
    if (tcb-> evaluate_node)
        call_exception (evaluate_extended_scope_event);
}


/***************************   GET NEXT PARAMETER   **************************/

MODULE get_next_parameter (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_node-> item_nbr++;
    if (tcb-> result_node-> item_nbr < tcb-> result_node-> argc)
      {
        tcb-> evaluate_node = tcb-> script_node-> op2;
        tcb-> script_node   = tcb-> script_node-> parent;
      }

    get_the_parameter ();
}


/***********************   EVALUATE CALL METHOD NODE   ***********************/

MODULE evaluate_call_method_node (THREAD *thread)
{
    int
        rc;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  Clean up error message just in case */
    object_error [0] = '\0';
    rc = tcb-> result_node-> gsl_function-> evaluate
                                               (  tcb-> result_node-> argc, 
                                                  tcb-> result_node-> argv,
                                                & tcb-> call_result,
                                                thread);
    if ((! rc) && tcb-> result_node-> gsl_function-> immediate)
        the_next_event = result_event;
        
    if (rc)
        report_error (error_event, object_error);
}


/*********************   REPORT ILLEGAL SCOPE PARAMETER   ********************/

MODULE report_illegal_scope_parameter (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_current_node (thread);
    report_error (error_event, "Illegal scope parameter %i to function %s%s%s",
                  tcb-> result_node-> item_nbr + 1,
                  tcb-> script_node-> scope
                      ? tcb-> result_node-> scope-> value.s : "",
                  tcb-> script_node-> scope 
                      ? "." : "",
                  tcb-> result_node-> name-> value.s);
}


/*************************   COPY RESULT FROM NAME   *************************/

MODULE copy_result_from_name (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    copy_result (tcb-> result_node,
                 tcb-> result_node-> name);
}


/**************************   COPY RESULT FROM OP1   *************************/

MODULE copy_result_from_op1 (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    copy_result (tcb-> result_node,
                 tcb-> result_node-> op1);
}


/*********************   ONE OF SCOPE NAME MUST BE NULL   ********************/

MODULE one_of_scope_name_must_be_null (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> scope && tcb-> script_node-> name)
        report_illegal_scope_parameter (thread);
}


/*********************   COPY NAME TO SCOPE IF DEFINED   *********************/

MODULE copy_name_to_scope_if_defined (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> name)
      {
        tcb-> result_node-> scope = tcb-> result_node-> name;
        tcb-> result_node-> name  = NULL;
      }
}


/************************   NODE SCOPE MUST BE NULL   ************************/

MODULE node_scope_must_be_null (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node-> scope)
        report_illegal_scope_parameter (thread);
}


/**************************   CONCATENATE OPERANDS   *************************/

MODULE concatenate_operands (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_node-> culprit = tcb-> result_node-> op1-> culprit
                                ? tcb-> result_node-> op1-> culprit
                                : tcb-> result_node-> op2-> culprit;

    /*  Concatenation is delayed until all operands to be concatenated have */
    /*  been evaluated.  This is needed for blocks to be concatenated.      */
    /*  We don't evaluate if this is a template line either, done in        */
    /*  copy_line_to_output instead.                                        */
    if ( (! (tcb-> script_node-> parent-> type     == GG_OPERATOR
    &&       tcb-> script_node-> parent-> operator == OP_UNDEFINED) )
    &&      (tcb-> script_node-> parent-> type != GG_LINE) )
      {
        tcb-> result_node-> value. s = concatenate_results (tcb-> result_node,
                                                            tcb-> shuffle,
                                                            FALSE);
        if (tcb-> result_node-> value. s)
            tcb-> result_node-> value. type = TYPE_STRING;
      }
    /*  Result is constant if both operands are constant, plus 2nd operand   */
    /*  has no spaces.  Otherwise shuffle may hit us.                        */
    tcb-> result_node-> constant = (tcb-> result_node-> op1-> constant
                                && (tcb-> result_node-> op2-> script_node-> spaces == 0)
                                &&  tcb-> result_node-> op2-> constant);
}


/*************************   RETURN OP1 IF DEFINED   *************************/

MODULE return_op1_if_defined (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> result_node-> op1-> value. type != TYPE_UNDEFINED)
      {
    tcb-> result_node-> constant = tcb-> result_node-> op1-> constant;

        copy_result (tcb-> result_node, tcb-> result_node-> op1);
        raise_exception (dialog_return_event);
      }
}


/*******************************   RETURN OP2   ******************************/

MODULE return_op2 (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> result_node-> constant = 
                     (tcb-> result_node-> op1 ? tcb-> result_node-> op1-> constant : TRUE)
                  && (tcb-> result_node-> op2 ? tcb-> result_node-> op2-> constant : TRUE);

    copy_result (tcb-> result_node, tcb-> result_node-> op2);
    raise_exception (dialog_return_event);
}


/**********************   EVALUATE COMPARISON OPERATOR   *********************/

MODULE evaluate_comparison_operator (THREAD *thread)
{
    int
        cmp;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> result_node-> op1-> value. type == TYPE_UNDEFINED)
      {
        tcb-> result_node-> culprit = tcb-> result_node-> op1-> culprit;
        return;
      }
    if (tcb-> result_node-> op2-> value. type == TYPE_UNDEFINED)
      {
        tcb-> result_node-> culprit = tcb-> result_node-> op2-> culprit;
        return;
      }

    /*  If one operand is non-string and the other is unknown,       */
    /*  try to make both numeric.                                    */
    if ((tcb-> result_node-> op1-> value. type != TYPE_STRING)
    &&  (tcb-> result_node-> op2-> value. type == TYPE_UNKNOWN))
        number_value (& tcb-> result_node-> op2-> value);

    if ((tcb-> result_node-> op2-> value. type != TYPE_STRING)
    &&  (tcb-> result_node-> op1-> value. type == TYPE_UNKNOWN))
        number_value (& tcb-> result_node-> op1-> value);

    /*  Handle numeric operators  */
    if ((tcb-> result_node-> op1-> value. type == TYPE_NUMBER)
    &&  (tcb-> result_node-> op2-> value. type == TYPE_NUMBER))
      {
        switch (tcb-> script_node-> operator)
          {
            case OP_EQUALS:
            case OP_SAFE_EQUALS:
                tcb-> result_node-> value. n
                    = (tcb-> result_node-> op1-> value. n
                    == tcb-> result_node-> op2-> value. n);
                break;

            case OP_NOT_EQUALS:
            case OP_SAFE_NOT_EQUALS:
                tcb-> result_node-> value. n
                    = (tcb-> result_node-> op1-> value. n
                    != tcb-> result_node-> op2-> value. n);
                break;

            case OP_GREATER_THAN:
            case OP_SAFE_GREATER_THAN:
                tcb-> result_node-> value. n
                    = (tcb-> result_node-> op1-> value. n
                    >  tcb-> result_node-> op2-> value. n);
                break;

            case OP_LESS_THAN:
            case OP_SAFE_LESS_THAN:
                tcb-> result_node-> value. n
                    = (tcb-> result_node-> op1-> value. n
                    <  tcb-> result_node-> op2-> value. n);
                break;

            case OP_GREATER_EQUAL:
            case OP_SAFE_GREATER_EQUAL:
                tcb-> result_node-> value. n
                    = (tcb-> result_node-> op1-> value. n
                    >= tcb-> result_node-> op2-> value. n);
                break;

            case OP_LESS_EQUAL:
            case OP_SAFE_LESS_EQUAL:
                tcb-> result_node-> value. n
                    = (tcb-> result_node-> op1-> value. n
                    <= tcb-> result_node-> op2-> value. n);
                break;

            default:
                raise_exception (anomaly_event);
                return;
          }
      }
    else
      /*  Handle non-numeric operators  */
      {
        cmp = strcmp (string_value (& tcb-> result_node-> op1-> value),
                      string_value (& tcb-> result_node-> op2-> value));
        switch (tcb-> script_node-> operator)
          {
            case OP_EQUALS:
            case OP_SAFE_EQUALS:
                tcb-> result_node-> value. n = (cmp == 0);
                break;

            case OP_NOT_EQUALS:
            case OP_SAFE_NOT_EQUALS:
                tcb-> result_node-> value. n = (cmp != 0);
                break;

            case OP_GREATER_THAN:
            case OP_SAFE_GREATER_THAN:
                tcb-> result_node-> value. n = (cmp >  0);
                break;

            case OP_LESS_THAN:
            case OP_SAFE_LESS_THAN:
                tcb-> result_node-> value. n = (cmp <  0);
                break;

            case OP_GREATER_EQUAL:
            case OP_SAFE_GREATER_EQUAL:
                tcb-> result_node-> value. n = (cmp >= 0);
                break;

            case OP_LESS_EQUAL:
            case OP_SAFE_LESS_EQUAL:
                tcb-> result_node-> value. n = (cmp <= 0);
                break;

            default:
                raise_exception (anomaly_event);
                return;
          }
      }
    tcb-> result_node-> value. type = TYPE_NUMBER;
}


/*********************   RETURN FALSE IF OP1 UNDEFINED   *********************/

MODULE return_false_if_op1_undefined (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> result_node-> op1-> value. type == TYPE_UNDEFINED)
      {
        tcb-> result_node-> value. type = TYPE_NUMBER;
        tcb-> result_node-> value. n    = 0;
        raise_exception (dialog_return_event);
      }
}


/*********************   RETURN FALSE IF OP2 UNDEFINED   *********************/

MODULE return_false_if_op2_undefined (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> result_node-> op2-> value. type == TYPE_UNDEFINED)
      {
        tcb-> result_node-> value. type = TYPE_NUMBER;
        tcb-> result_node-> value. n    = 0;
        raise_exception (dialog_return_event);
      }
}


/************************   RETURN TRUE IF OP1 TRUE   ************************/

MODULE return_true_if_op1_true (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (number_value (& tcb-> result_node-> op1-> value))
      {
        tcb-> result_node-> value. type = TYPE_NUMBER;
        tcb-> result_node-> value. n    = 1;
        raise_exception (dialog_return_event);
      }
}


/***********************   RETURN FALSE IF OP1 FALSE   ***********************/

MODULE return_false_if_op1_false (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (! number_value (& tcb-> result_node-> op1-> value))
      {
        tcb-> result_node-> value. type = TYPE_NUMBER;
        tcb-> result_node-> value. n    = 0;
        raise_exception (dialog_return_event);
      }
}


/*************************   BEGIN EVALUATING NODE   *************************/

MODULE begin_evaluating_node (THREAD *thread)
{
    RESULT_NODE
        *new_node;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> script_node = tcb-> evaluate_node;
    new_node = new_result_node ();
    ASSERT (new_node);
    init_result_node (new_node);

    new_node-> parent = tcb-> result_node;

    *tcb-> result_ptr = new_node;
    new_node-> script_node = tcb-> script_node;

    new_node-> indent      = tcb-> script_node-> spaces;

    tcb-> result_node = new_node;

    if (tcb-> script_node-> constant)
      {
        copy_value (& new_node-> value, & tcb-> script_node-> result);
        raise_exception (dialog_return_event);
      }
}


/************************   GENERATE CALL TYPE EVENT   ***********************/

MODULE generate_call_type_event (THREAD *thread)
{
    SYMBOL
        *symbol = NULL;
    char
        *scope,
        *name;
    GSL_FUNCTION
        *gsl_function;
    int
        argc;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    must_be_defined (tcb-> result_node-> scope);
    must_be_defined (tcb-> result_node-> name);
    if (exception_raised)
        return;

    scope = tcb-> script_node-> scope
          ? string_value (& tcb-> result_node-> scope-> value)
          : NULL;
    name  = string_value (& tcb-> result_node-> name-> value);

    /*  See if this is a GSL macro/function.  If no scope is specified,      */
    /*  try local macros followed by global macros.                          */
    if (scope)
      {
        if (lexcmp (scope, "global") == 0)
            symbol = sym_lookup_symbol (global_macros, name);
      }
    else
      {
        symbol = sym_lookup_symbol (tcb-> macros, name);
        if (! symbol)
            symbol = sym_lookup_symbol (global_macros, name);
      }

    if (symbol)
      {
        tcb-> result_node-> macro = (SCRIPT_MACRO *) symbol-> data;
        the_next_event = macro_event;
        return;
      }

    /*  Otherwise look for in-built function.  */
    gsl_function = locate_method (tcb-> result_node);
    if (! gsl_function)
      {
        report_error (error_event, "Unknown function: %s%s%s",
                      scope ? scope : "",
                      scope ?"." : "",
                      name);
        return;
      }
    argc = build_method_arguments (  tcb-> script_node-> op1,
                                   & tcb-> result_node-> argv);
    tcb-> result_node-> argc = argc;

    if ((argc >= gsl_function-> min_parmc)
    && ((gsl_function-> max_parmc == 0) || (argc <= gsl_function-> max_parmc)))
      {
        tcb-> result_node-> gsl_function = gsl_function;
        the_next_event = method_event;
      }
    else
        report_error (error_event, "Parameter mismatch for function: %s%s%s",
                      scope ? scope : "",
                      scope ?"." : "",
                      name);
}


/****************************   GET FIRST CHILD   ****************************/

MODULE get_first_child (THREAD *thread)
{
    SCOPE_BLOCK
        *scope_block;
    XML_ITEM
        *parent;
    char
        *name,
        *alias;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    /*  .(for | sort) <scope>.<name> as <as> where <op1> by <op2> */

    must_be_defined (tcb-> result_node-> scope);
    must_be_defined (tcb-> result_node-> name);
    must_be_defined (tcb-> result_node-> as);
    if (exception_raised)
        return;

    parent = lookup_from_xml (tcb-> result_node-> scope);
    if (! parent)
        return;

    name = tcb-> script_node-> name
         ? string_value (& tcb-> result_node-> name-> value)
         : NULL;

    if (tcb-> result_node-> as)
      {
        string_value (& tcb-> result_node-> as-> value);
        if (tcb-> result_node-> as-> value. s [0] != 0)
            alias = tcb-> result_node-> as-> value. s;
        else
            alias = NULL;
      }
    else
        alias = name;

    if ((! alias)
    && (! tcb-> script_node-> stacked))
      {
        report_unaliased_unstacked_scope ();
        return;
      }

    scope_block = create_scope_block (& tcb-> scope_stack, 
                                      GG_FOR, alias);
    scope_block-> stacked  = tcb-> script_node-> stacked;
    scope_block-> xml_item = xml_first_child (parent);

    tcb-> result_node-> item_nbr = 0;
    tcb-> sort_type = TYPE_UNKNOWN;

    get_matching_child (scope_block);
}


static void
get_matching_child (SCOPE_BLOCK *scope_block)
{
    char
        *name = tcb-> result_node-> name
              ? string_value (& tcb-> result_node-> name-> value)
              : NULL,
        *xml_name;

    while (scope_block-> xml_item)
      {
        xml_name = xml_item_name (scope_block-> xml_item);
        if (xml_name)
      {
            if (name)
          {
                if (tcb-> ignorecase)
                  {
                    if (lexcmp (xml_name, name) == 0)
                        break;
                  }
                else
                  {
                    if (streq (xml_name, name))
                        break;
                  }
          }
            else
                /*  If no child name specified, take all named children  */
                break;
      }
        scope_block-> xml_item = xml_next_sibling (scope_block-> xml_item);
      }
    tcb-> result_node-> item_nbr++;

    if (scope_block-> xml_item)
      {
        scope_block-> scope_item
            = create_scope_item (scope_block,
                                 scope_block-> xml_item,
                                 tcb-> result_node-> item_nbr);
        ASSERT (scope_block-> scope_item);

        the_next_event = ok_event;
      }
    else
        the_next_event = none_event;
}


/***************************   CLOSE SCRIPT FILE   ***************************/

MODULE close_script_file (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    close_script (tcb-> script_source);
}


/*************************   FINISH EVALUATING NODE   ************************/

MODULE finish_evaluating_node (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> script_node
    &&  tcb-> result_node-> constant && ! tcb-> script_node-> constant)
      {
        tcb-> script_node-> constant = TRUE;
        copy_value_ (tcb-> script_node-> dynamic
                       ? tcb-> scratch_memtrn
                       : tcb-> script_source-> memtrn,
                     & tcb-> script_node-> result,
                     & tcb-> result_node-> value);
      }
}


/****************************   POP CURRENT NODE   ***************************/

MODULE pop_current_node (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    pop_the_result_node ();
}


static void
pop_the_result_node (void)
{
    if (tcb-> node_stack)
      {
        tcb-> result_node = tcb-> node_stack;
        tcb-> node_stack  = tcb-> node_stack-> next;
        tcb-> result_node-> next = NULL;

        tcb-> result_root = tcb-> result_node;
        while (tcb-> result_root-> parent)
            tcb-> result_root = tcb-> result_root-> parent;

        tcb-> script_node = tcb-> result_node-> script_node;
        tcb-> script_root = tcb-> result_root-> script_node;
      }
    else
        raise_exception (anomaly_event);
}


/************************   REQUEST LOAD SCRIPT FILE   ***********************/

MODULE request_load_script_file (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (load_script_file (tcb-> script_name,
                          tcb-> template,
                          (Bool) !tcb-> execute_full,
                          TRUE,         /*  Keep parsed file                 */
                          0,
                          thread-> queue))
        raise_exception (anomaly_event);
}


/**************************   DESTROY SCRIPT TEXT   **************************/

MODULE destroy_script_text (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    mem_strfree (& tcb-> script_name);
    mem_strfree (& tcb-> script_text);
}


/****************************   START NEW SCRIPT   ***************************/

MODULE start_new_script (THREAD *thread)
{
    struct_ggscrp_done
       *ggscrp_done;
    SCRIPT_SOURCE
       *script_source;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_ggscrp_done (thread-> event-> body, & ggscrp_done);
    script_source = (SCRIPT_SOURCE *) ggscrp_done-> script_source;
    free_ggscrp_done (& ggscrp_done);

    if (! list_empty (& script_source-> line_head))
      {
        tcb-> script_source = script_source;
        tcb-> script_line   = (SCRIPT_LINE *) & script_source-> line_head;

        xml_put_attr (tcb-> xml_gsl,
                      "script",
                      script_source-> name);
      }
}


/*************************   FLAG ERROR OCCURRENCE   *************************/

MODULE flag_error_occurrence (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    tcb-> error_occurred = TRUE;
}


/*****************************   LOG LOAD ERROR   ****************************/

MODULE log_load_error (THREAD *thread)
{
    struct_ggscrp_error
       *ggscrp_error;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_ggscrp_error (thread-> event-> body, & ggscrp_error);
    /*  Use line from new file if there is one, otherwise line from curent   */
    /*  file, if there is one.                                               */
    if (ggscrp_error-> error_line)
        send_ggcode_error (tcb-> replyq ? & tcb-> replyq-> qid : NULL,
                           tcb-> job,
                           ggscrp_error-> error_name,
                           ggscrp_error-> error_line,
                           ggscrp_error-> error_text);
    else
        send_ggcode_error (tcb-> replyq ? & tcb-> replyq-> qid : NULL,
                           tcb-> job,
                           tcb-> script_source
                               ? tcb-> script_source-> name : NULL,
                           (word) (tcb-> script_line
                               ? tcb-> script_line  -> line : 0),
                           ggscrp_error-> error_text);
    free_ggscrp_error (& ggscrp_error);
}


/***************************   DESTROY NEW SCRIPT   **************************/

MODULE destroy_new_script (THREAD *thread)
{
    struct_ggscrp_done
       *ggscrp_done;
    SCRIPT_SOURCE
       *script_source;

    tcb = thread-> tcb;                 /*  Point to thread's context        */

    get_ggscrp_done (thread-> event-> body, & ggscrp_done);
    script_source = (SCRIPT_SOURCE *) ggscrp_done-> script_source;
    free_ggscrp_done (& ggscrp_done);

    if (script_source)
      {
        script_source-> keep = FALSE;
        close_script (script_source);
      }

    tcb-> script_source = NULL;
    tcb-> script_line   = NULL;

    xml_put_attr (tcb-> xml_gsl,
                  "script",
                  NULL);
}


/************************   REQUEST LOAD SCRIPT TEXT   ***********************/

MODULE request_load_script_text (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (load_script_text (tcb-> script_name,
                          tcb-> script_text,
                          tcb-> template,
                          (Bool) !tcb-> execute_full,
                          0,
                          thread-> queue))
        raise_exception (anomaly_event);
}


/***************************   RETURN OK FEEDBACK   **************************/

MODULE return_ok_feedback (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    send_ggcode_ok (tcb-> replyq ? & tcb-> replyq-> qid : NULL,
                    tcb-> job);
}


/********************   FINISH EXCEPTION IF EXECUTE FULL   *******************/

MODULE finish_exception_if_execute_full (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (tcb-> execute_full)
        raise_exception (finish_event);
}


/***********************   CHECK FOR SHUTDOWN REQUEST   **********************/

MODULE check_for_shutdown_request (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (thread-> queue-> shutdown)
        raise_exception (shutdown_event);
}


/*************************   BEFORE EXECUTING LINE   *************************/

MODULE before_executing_line (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    ASSERT (tcb-> execute_level >= 0);
    tcb-> execute_level++;
}


/**************************   AFTER EXECUTING LINE   *************************/

MODULE after_executing_line (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    if (--tcb-> execute_level == 0)
      {
        mem_rollback (tcb-> scratch_memtrn);
        tcb-> scratch_memtrn = mem_new_trans ();
      }
}


/**************************   TERMINATE THE THREAD   *************************/

MODULE terminate_the_thread (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    the_next_event = terminate_event;
}


/*************************   SIGNAL INTERNAL ERROR   *************************/

MODULE signal_internal_error (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */

    report_error (_LR_NULL_EVENT,
                  "%s", "Internal error");
}


/**************************   INVOKE ABORT HANDLER   *************************/

MODULE invoke_abort_handler (THREAD *thread)
{
    tcb = thread-> tcb;                 /*  Point to thread's context        */
    if (abort_fct)
        (abort_fct) ();                 /*  Call abort handler if defined    */
}


/*  ---------------------------------------------------------------------[<]-
    Function: gg_send_output

    Synopsis: Redirects stdout output to a specified OUTPUT_FCT function.
    If the specified address is NULL, redirects back to the stdout stream.
    If the echo argument is TRUE, stdout output is also sent to stdout as
    normal.
    ---------------------------------------------------------------------[>]-*/

void
gg_send_output (THREAD *gsl_thread, OUTPUT_FCT *new_output_fct, Bool echo)
{
    tcb = gsl_thread-> tcb;             /*  Point to thread's context        */

    tcb-> output_fct  = new_output_fct;
    tcb-> stdout_echo = echo;           /*  Copy to stdout                   */
}


/*  ---------------------------------------------------------------------[<]-
    Function: gg_set_handler

    Synopsis: Sets-up an event handler for an internal GSL event.  Currently
    only supports one event, EVENT_ABORT.
    ---------------------------------------------------------------------[>]-*/

void
gg_set_handler (HANDLER_FCT *handler_fct, int event)
{
    switch (event) {
        case EVENT_ABORT:
            abort_fct = handler_fct;
            break;
    }
}

