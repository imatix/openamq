/*===========================================================================*
 *                                                                           *
 *  ggcode.c - Code generator                                                *
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

#include "sfl.h"                        /*  Universal include file           */
#if defined (JAVASCRIPT)
#include "ggjs.h"                       /*  Support for Javascript           */
#endif
#include "ggpars.h"                     /*  Script parser header file        */
#include "ggcomm.h"                     /*  Common declarations              */
#include "ggeval.h"                     /*  Evaluation functions             */
#include "ggcode.h"                     /*  Include header file              */
#include "ggcode.d"                     /*  Include dialog data              */

/*- Type definitions --------------------------------------------------------*/


/*- Function prototypes -----------------------------------------------------*/

#if defined (JAVASCRIPT)
static int       initialise_javascript          (void);
#endif
static void      initialise_predefined_scopes   (void);
static int       prepare_initial_script         (void);
static int       prepare_script                 (char *name);
static void      start_interpreter              (void);
static int       start_dialog                   (void);
static Bool      free_data                      (SYMBOL *symbol);
static void      destroy_data_structures        (void);
static int       skip_control_block             (CONTROL_TYPE open_type);
static int       match_one_line                 (SCRIPT_NODE_TYPE type,
                                                 CONTROL_BLOCK *control);
static void      unmatched_control_error        (CONTROL_BLOCK *control);
static void      open_output_file               (char *mode);
static Bool      sort_compare                   (LIST *t1, LIST *t2);
static int       substitute_parameters_into_xml (XML_ITEM *xml_item,
                                                 SCRIPT_NODE *args,
                                                 SCRIPT_NODE *params);
static void      substitute_parameter_into_xml  (XML_ITEM *xml_item,
                                                 SCRIPT_NODE *arg,
                                                 SCRIPT_NODE *param);
static void      close_macro_or_function_block  (CONTROL_TYPE type);
static int       parse_locator                  (void);
static void      replace_referred_xml_by_null   (XML_ITEM **xml_referred, 
                                                 XML_ITEM *xml_delete);
static void      copy_xml                       (XML_ITEM *to,
                                                 XML_ITEM *from);
static void      print_to_output_or_stdout      (char *text,
                                                 Bool line_break);
static void      gg_handle_signal               (int the_signal);

/*- Macros ------------------------------------------------------------------*/


/*- Global variables used in this source file only --------------------------*/

static int
    output_line,                        /*  Line number in output file       */
    last_line;                          /*  Most recently written line       */

static char
    *output_name;

static XML_ITEM
    *xml_root,
    *xml_switches,
    *the_parent,
    *the_sibling;

static FILE
    *output;

static SCRIPT_NODE
    *script_root;


/********************************   M A I N   ********************************/

int
gg_code (XML_ITEM *xml, XML_ITEM *switches)
{
    Bool
        error = FALSE;

    signal (SIGSEGV, gg_handle_signal);
    signal (SIGABRT, gg_handle_signal);
    xml_root     = xml;
    xml_switches = switches;

    feedback = 0;                       /*  No errors so far                 */

    /*  Initialise a whole bunch of stuff.  We have to do this explicitly    */
    /*  rather than with initial values so that the code can be called more  */
    /*  than once.                                                           */
    reset_scope_stack ();
    reset_script_stack ();

    /*  Create macro symbol table                                            */
    macros = sym_create_table ();

    output_name     = NULL;
    output          = NULL;
    script_root     = NULL;
    output_line     = 1;
    last_line       = 0;

    if (!xml_root)
      {
        gg_report_error ('E', "No XML data.");
        return feedback;
      }

#if defined (JAVASCRIPT)
    if (initialise_javascript ())
        error = TRUE;
#endif

    initialise_predefined_scopes ();
    if (prepare_initial_script ())
        error = TRUE;

    if (! error)
        start_interpreter ();

    /*  Now get rid of macro definitions  */
    sym_exec_all (macros, free_data);
    sym_delete_table (macros);

    close_output_file ();
    destroy_data_structures ();

    return feedback;
}


#if defined (JAVASCRIPT)
static int
initialise_javascript (void)
{
    JSBool
        ok;

    static JSClass global_class = {
        "global", 0,
        JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,   JS_FinalizeStub
    };

    ok = JS_FALSE;
    rt = JS_NewRuntime (8L * 1024L * 1024L);
    if (rt)
      {
        cx = JS_NewContext (rt, 8192);
        if (cx)
          {
            JS_SetErrorReporter (cx, js_error);
            obj = JS_NewObject(cx, &global_class, NULL, NULL);
            if (obj)
                ok = JS_InitStandardClasses (cx, obj);
          }
      }
    if (ok)
      {
        if (!JS_DefineFunction (cx, obj, "value", js_valueof, 2, 0))
            ok = JS_FALSE;
        if (!JS_DefineFunction (cx, obj, "write", js_write, 1, 0))
            ok = JS_FALSE;
      }

    if (!ok)
        gg_report_error ('E', "Error initialising JavaScript.");

    return feedback;
}
#endif


static void
initialise_predefined_scopes (void)
{
    SCOPE_BLOCK
        *global_block,
        *root_block;
    XML_ITEM
        *xml_global;
    XML_ATTR
        *xml_switch;
    char
        *name,
        *value;

    /*  Create initial global block  */
    xml_global   = xml_new (NULL, "global", "");
    global_block = create_scope_block ("global");
    global_block-> xml_item = xml_global;

    /*  Copy switches into global block  */
    if (xml_switches)
        FORATTRIBUTES (xml_switch, xml_switches)
      {
        name = xml_attr_name  (xml_switch);
        value = xml_attr_value (xml_switch);
        xml_put_attr (xml_global,
                      name,
                      value);
      }

    /*  Create root XML block  */
    root_block = create_scope_block (xml_item_name (xml_root));
    root_block-> children = TRUE;
    create_scope_item (root_block, xml_root, 1);
    first_scope_item  (root_block);
}


static int
prepare_initial_script ()
{
    char
        *initial_script,
        *value;
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;


    /*  Find initial script name, first in switches, then in XML             */
    initial_script = xml_get_attr (xml_item, "script", NULL);
    if (!initial_script)                /*  For compatibility                */
        initial_script = xml_get_attr (xml_root, "script", NULL);

    if (!initial_script)
      {
        gg_report_error ('E', "No script specified.");
        return feedback;
      }
    if (prepare_script (initial_script))
        return -1;

    /*  Lookup initial template flag                                         */
    value = xml_get_attr (xml_item, "template", NULL) ;
    if (value)
        cur_script_block.template = atoi (value);
    else
        cur_script_block.template = 0;

    return feedback;
}


static int
prepare_script (char *name)
{
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;

    if (!open_script_file (name))
      {
        gg_report_error ('E', "Can't open script file %s", name);
        return -1;
      }
    else
        if (xml_item)
            xml_put_attr (xml_item,
                          "script",
                          cur_script_file-> name);
    return 0;
}


static void
start_interpreter (void)
{
    start_dialog ();
    the_next_event = _LR_NULL_EVENT;
}


static int
start_dialog (void)
{
#   include "ggcode.i"                  /*  Include dialog interpreter       */
}


static Bool
free_data (SYMBOL *symbol)
{
    mem_free (symbol-> data);
    return TRUE;
}


static void
destroy_data_structures (void)
{
    while (! list_empty (& script_stack))
        pop_script_block ();
    pop_script_block ();      /*  One more time to kill control stack */

    destroy_script_data ();

    while (!list_empty (&scope_stack))
        destroy_scope_block ();

#if defined (JAVASCRIPT)
    JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);
    JS_ShutDown();
#endif
}


/*************************   INITIALISE THE PROGRAM   ************************/

MODULE initialise_the_program (void)
{
    the_next_event = ok_event;
}



/*************************   GET NEXT SCRIPT LINE   **************************/

MODULE get_next_script_line (void)
{
    char
        *value;
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;
    static event_t type_event [] = {
        comment_event,        /*  GG_COMMENT        */
        text_event,           /*  GG_TEXT           */
        text_event,           /*  GG_SUBSTITUTE     */
        text_event,           /*  GG_OPERATOR       */
        _LR_NULL_EVENT,       /*  GG_LITERAL        */
        _LR_NULL_EVENT,       /*  GG_NUMBER         */
        _LR_NULL_EVENT,       /*  GG_SYMBOL         */
        _LR_NULL_EVENT,       /*  GG_MEMBER         */
        call_event,           /*  GG_CALL           */
        _LR_NULL_EVENT,       /*  GG_OPERAND        */
        output_event,         /*  GG_OUTPUT         */
        append_event,         /*  GG_APPEND         */
        close_event,          /*  GG_CLOSE          */
        if_event,             /*  GG_IF             */
        elsif_event,          /*  GG_ELSIF          */
        else_event,           /*  GG_ELSE           */
        end_if_event,         /*  GG_END_IF         */
        for_event,            /*  GG_FOR            */
        end_for_event,        /*  GG_END_FOR        */
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
        _LR_NULL_EVENT,       /*  GG_RETURN         */
        gsl_event,            /*  GG_GSL            */
        direct_event,         /*  GG_DIRECT         */
        xml_event,            /*  GG_XML            */
        javascript_event,     /*  GG_JAVASCRIPT     */
        _LR_NULL_EVENT,       /*  GG_SOURCE         */
        template_event,       /*  GG_TEMPLATE       */
        end_template_event,   /*  GG_END_TEMPLATE   */
        echo_event,           /*  GG_ECHO           */
        abort_event,          /*  GG_ABORT          */
        define_event,         /*  GG_DEFINE         */
        save_event,           /*  GG_SAVE           */
        sort_event,           /*  GG_SORT           */
        _LR_NULL_EVENT };     /*  GG_UNDEFINED      */
      

    /*  Lookup ignore case flag                                              */
    value = xml_item ? xml_get_attr (xml_item, "ignorecase", NULL) : NULL;
    ignorecase = value ? atoi (value) : 0;

    /*  Lookup shuffle flag                                                  */
    value = xml_item ? xml_get_attr (xml_item, "shuffle", NULL) : NULL;
    shuffle = value ? atoi (value) : 0;

    if (parse_next_script_line ())
      {
        script_root = cur_script_file-> cur_line-> node;
        if (!script_root)
          {
            gg_report_error ('E', "%s", gg_error ());
            
            the_next_event = anomaly_event;
            return;
          }
      }
    else
      {
        the_next_event = end_of_script_event;
        return;
      }

    ASSERT (script_root-> type <  tblsize (type_event));
    the_next_event = type_event [script_root-> type];
    if (the_next_event == _LR_NULL_EVENT)
      {
        gg_report_error ('E', "Unknown script construct");
        raise_exception (anomaly_event);
      }
}


/************************   STORE SYMBOL DEFINITION   ************************/

MODULE store_symbol_definition (void)
{
    XML_ITEM
        *from_xml,
        *xml_value,
        *xml_child;
    SCOPE_BLOCK
        *scope_block;
    char
        *name = NULL;
    SCRIPT_NODE
        *node;

    /*  <op1>.<op2> [operator]= <op3>  */

    if ((evaluate_script (script_root-> op1))
    ||  (evaluate_script (script_root-> op2)))
      {
        raise_exception (anomaly_event);
        return;
      }

    if (script_root-> op2)
        name = string_result (script_root-> op2);

    if (script_root-> op1)
      {
	from_xml = extended_scope_xml (script_root-> op1);
	if (! from_xml)
          {
            gg_report_error ('E', "Unknown data scope: %s",
		                   extended_scope_string (script_root-> op1));

            raise_exception (anomaly_event);
            return ;
          }
      }
    else
      {
        from_xml = NULL;
        if (name)
          {
            /*  Look for a scope block which defines this identifier.  */
            FORLIST (scope_block, scope_stack)
                if (scope_block-> xml_item)
                    if (xml_attr_ic (scope_block-> xml_item,
                                     name,
                                     ignorecase) != NULL)
                        {
                          from_xml = scope_block-> xml_item;
                          break;
                        }
            if (from_xml == NULL)
                /*  Use outermost .for block  */
                from_xml = first_scope_block ()-> xml_item;
          }
        else
          {
            /*  Look for a scope block with an item value  */
            FORLIST (scope_block, scope_stack)
              {
                if (scope_block-> xml_item)
                    FORVALUES (xml_child, scope_block-> xml_item)
                        {
                          from_xml = scope_block-> xml_item;
                          break;
                        }
                if (from_xml)
                    break;
              }
            if (from_xml == NULL)
                /*  Use innmost .for block  */
                from_xml = last_scope_block ()-> xml_item;
          }
      }
    
    if (from_xml == NULL)     /*  XML has been deleted  */
      {
        gg_report_error ('E', "Scope has been deleted.");
        raise_exception (anomaly_event);
        return ;
      }

    if (script_root-> operator != OP_UNDEFINED)
      {
        /*  Asignment with an operator must have a second operand            */
        if (! script_root-> op3)
          {
            gg_report_error ('E', "Missing operand.");
            raise_exception (anomaly_event);
            return ;
          }
        node = mem_alloc (sizeof (*node));
        ASSERT (node);
        init_script_node (*node);
        
        node-> type     = GG_OPERATOR;
        node-> operator = script_root-> operator;

        node-> op1 = mem_alloc (sizeof (*node-> op1));
        ASSERT (node-> op1);
        init_script_node (*node-> op1);

        if (name)
            node-> op1-> result_s = mem_strdup (
                                        xml_get_attr_ic (
                                            from_xml,
                                            name,
                                            NULL,
                                            ignorecase));
        else
            node-> op1-> result_s = mem_strdup (
                                        compound_item_value (
                                            from_xml));
        if (node-> op1-> result_s)
            node-> op1-> result_type = TYPE_STRING;
        else
          {
            node-> op1-> result_type = TYPE_UNDEFINED;
            node-> op1-> culprit     = script_root;
          }

        node-> op2 = script_root-> op3;
        
        if (evaluate_script_node (node) != 0)
          {
            node-> op2 = NULL;   /*  Don't want to free this node  */
            gg_free (node);
            raise_exception (anomaly_event);
            return;
          }
      }
    else
      {
        node = script_root-> op3;
        if (evaluate_script_node (node) != 0)
          {
            raise_exception (anomaly_event);
            return;
          }
      }

    /*  An undefined result gives an error except in three specific cases:   */
    /*      1. '.define x ='                                                 */
    /*      2. '.define x = y ?'                                             */
    /*      2. '.define x ?= y ?'                                            */
    if ((node)                                        /*  Covers case 1      */
    &&  (node-> result_type == TYPE_UNDEFINED)
    &&  ! (((script_root-> operator == OP_UNDEFINED)  /*  Covers case 2      */
        ||  (script_root-> operator == OP_DEFAULT))   /*  Covers case 3      */
        && ((script_root-> op3-> type     == GG_OPERATOR)
        &&  (script_root-> op3-> operator == OP_DEFAULT)
        &&  (script_root-> op3-> op2      == NULL)) ))
      {
        undefined_expression_error (node);
        raise_exception (anomaly_event);
        if (script_root-> operator != OP_UNDEFINED)
          {
            node-> op2 = NULL;   /*  Don't want to free this node  */
            gg_free (node);
          }
        return;
      }

    if (name)
        xml_put_attr_ic (from_xml,
                         name,
                         (node &&
                          node-> result_type != TYPE_UNDEFINED)
                             ? string_result (node) : NULL,
                         ignorecase, TRUE);
    else
      {
        /*  Modifying an item's value means getting rid of any existing      */
        /*  value nodes then adding one value node with the new value.       */
        xml_child = xml_first_child (from_xml);
        while (xml_child)
          {
            name = xml_item_name (xml_child);
            xml_value = xml_child;
            xml_child = xml_next_sibling (xml_child);
            if (! name)           /*  This is a value node  */
                xml_free (xml_value);
          }
        xml_new (from_xml,
                 NULL,       /*  Value node  */
                 (node)
                     ? string_result (node) : NULL);
      }

    if (script_root-> operator != OP_UNDEFINED)
      {
        node-> op2 = NULL;   /*  Don't want to free this node  */
        gg_free (node);
      }
}


/*************************   STORE MACRO DEFINITION   ************************/

MODULE store_macro_definition (void)
{
    SYMBOL
        *symbol;

    if (evaluate_script (script_root-> op1))
      {
        raise_exception (anomaly_event);
        return;
      }

    symbol = sym_assume_symbol (macros,
                                string_result (script_root-> op1), 
                                NULL);
    ASSERT (symbol);

    if (!symbol-> data)       /*  Macro not already defined  */
        symbol-> data = mem_alloc (sizeof (MACRO));

    ASSERT (symbol-> data);

    ((MACRO *) (symbol-> data))-> position = script_position ();
    ((MACRO *) (symbol-> data))-> args     = script_root-> op2;
    ((MACRO *) (symbol-> data))-> template = 1;
}


/***********************   STORE FUNCTION DEFINITION   ***********************/

MODULE store_function_definition (void)
{
    SYMBOL
        *symbol;

    if (evaluate_script (script_root-> op1))
      {
        raise_exception (anomaly_event);
        return;
      }

    symbol = sym_assume_symbol (macros,
                                string_result (script_root-> op1), 
                                NULL);
    ASSERT (symbol);

    if (!symbol-> data)       /*  Macro not already defined  */
        symbol-> data = mem_alloc (sizeof (MACRO));

    ASSERT (symbol-> data);

    ((MACRO *) (symbol-> data))-> position = script_position ();
    ((MACRO *) (symbol-> data))-> args     = script_root-> op2;
    ((MACRO *) (symbol-> data))-> template = 0;
}


/****************************   SKIP MACRO BODY   ****************************/

MODULE skip_macro_body (void)
{
    int
        template;

    template = cur_script_block.template;
    cur_script_block.template = 1;
    skip_control_block (CTL_MACRO);
    cur_script_block.template = template;
}


static int
skip_control_block (CONTROL_TYPE open_type)
{
    CONTROL_BLOCK
        control;
    char
        *terminator;

    push_script_block ();               /*  Make dummy block  */
    push_control (open_type, NULL);
    while (!list_empty (& cur_script_block. control_stack))
      {
        if (parse_next_script_line ())
          {
            script_root = cur_script_file-> cur_line-> node;
            if (!script_root)
              {
                gg_report_error ('E', "%s", gg_error ());
                raise_exception (anomaly_event);
                pop_script_block ();
                return -1;
              }
            if (match_one_line (script_root-> type, & control))
              {
                pop_script_block ();
                raise_exception (anomaly_event);
                return -1;
              }
            /*  If an incoming (<<) block is found then skip it  */
            if ((script_root-> op1)
            &&  (script_root-> op1-> type == GG_SOURCE)
            &&  (script_root-> op1-> op2))
              {
                if (evaluate_script (script_root-> op1-> op2))
                  {
                    raise_exception (anomaly_event);
                    return -1;
                  }
                terminator = string_result (script_root-> op1-> op2);
                while (read_next_script_line ())
                    if (strncmp (cur_script_file-> cur_line-> text,
                                 terminator,
                                 strlen (terminator)) == 0)
                        break;
              }
          }
        else  /*  End-of-file  */
          {
            pop_control (& control);
            unmatched_control_error (& control);
            pop_script_block ();
            raise_exception (anomaly_event);
            return -1;
          }
      }
    pop_script_block ();
    return 0;
}


static int
match_one_line (SCRIPT_NODE_TYPE type, CONTROL_BLOCK *control)
{
    Bool
        error = FALSE;

    switch (type)
      {
        case GG_IF:           push_control (CTL_IF,       NULL);     break;
        case GG_FOR:          push_control (CTL_FOR,      NULL);     break;
        case GG_WHILE:        push_control (CTL_WHILE,    NULL);     break;
        case GG_MACRO:        push_control (CTL_MACRO,    NULL);     break;
        case GG_FUNCTION:     push_control (CTL_FUNCTION, NULL);     break;
        case GG_NEW:          push_control (CTL_NEW,      NULL);     break;
        case GG_TEMPLATE:     push_template_control ();
                              if (exception_raised)
                                  return -1;
                              break;
        case GG_END_IF:       pop_control (control);
                              if ((control-> type != CTL_IF)
                              &&  (control-> type != CTL_ELSE))
                                  error = TRUE;
                              break;
        case GG_ELSE:         pop_control (control);
                              if (control-> type != CTL_IF)
                                  error = TRUE;
                              else
                              if (!list_empty (& cur_script_block. control_stack))
                                  push_control (CTL_ELSE, NULL);
                              break;
        case GG_ELSIF:        pop_control (control);
                              if (control-> type != CTL_IF)
                                  error = TRUE;
                              else
                              if (!list_empty (& cur_script_block. control_stack))
                                  push_control (CTL_IF, NULL);
                              break;
        case GG_END_FOR:      pop_control (control);
                              if (control-> type != CTL_FOR)
                                  error = TRUE;
                              break;
        case GG_END_WHILE:    pop_control (control);
                              if (control-> type != CTL_WHILE)
                                  error = TRUE;
                              break;
        case GG_END_MACRO:    pop_control (control);
                              if (control-> type != CTL_MACRO)
                                  error = TRUE;
                              break;
        case GG_END_FUNCTION: pop_control (control);
                              if (control-> type != CTL_FUNCTION)
                                  error = TRUE;
                              break;
        case GG_END_NEW:      pop_control (control);
                              if (control-> type != CTL_NEW)
                                  error = TRUE;
                              break;
        case GG_END_TEMPLATE: pop_control (control);
                              if (control-> type != CTL_TEMPLATE)
                                  error = TRUE;
                              else
                                  cur_script_block.template = (long) control-> extra;
                              break;
        default:              break;
      }
    if (error)
      {
        unmatched_control_error (control);
        return -1;
      }
    return 0;
}


static void
unmatched_control_error (CONTROL_BLOCK *control)
{
    if (control-> position)
        gg_report_error ('E', "Unmatched control at (%s %u).",
                              control-> position-> parent-> name,
                              control-> position-> line);
    else
        gg_report_error ('E', "Unmatched control.");
}


/***************************   SKIP FUNCTION BODY   **************************/

MODULE skip_function_body (void)
{
    int
        template;

    template = cur_script_block.template;
    cur_script_block.template = 0;
    skip_control_block (CTL_FUNCTION);
    cur_script_block.template = template;
}


/**************************   OPEN FILE FOR OUTPUT   *************************/

MODULE open_file_for_output (void)
{
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;

    /*  .output <op1>  */

    if (evaluate_script (script_root-> op1))
      {
        raise_exception (anomaly_event);
        return;
      }

    open_output_file (FOPEN_WRITE_TEXT);
    output_line = 1;
    last_line = 0;
    line_length = 0;

    if (xml_item)
        put_numeric_attr (xml_item,
                          "line",
                          output_line);
}


static void
open_output_file (char *mode)
{
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;

    if (output)
        close_output_file ();

    output_name = mem_strdup (string_result (script_root-> op1));

    if (output_name && *output_name)
        output = fopen (output_name, mode);

    if (output)
      {
        if (xml_item)
            xml_put_attr (xml_item,
                          "outfile",
                          output_name);
      }
    else
      {
        gg_report_error ('E', "Can't open output file %s", output_name);
        raise_exception (anomaly_event);
        return;
      }
}


/**************************   OPEN FILE FOR APPEND   *************************/

MODULE open_file_for_append (void)
{
    FILE
        *current;
    char
        buffer [LINE_MAX + 1];
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;

    /*  .append <op1>  */

    if (evaluate_script (script_root-> op1))
      {
        raise_exception (anomaly_event);
        return;
      }

    /*  Count line number for extended file  */
    output_line = 0;
    last_line = 0;
    line_length = 0;
    current = fopen (string_result (script_root-> op1), FOPEN_READ_TEXT);
    if (current)
      {
        while (file_read (current, buffer))
            output_line++;
        file_close (current);
      }

    open_output_file (FOPEN_APPEND_TEXT);

    if (xml_item)
        put_numeric_attr (xml_item,
                          "line",
                          output_line);
}


/***************************   CLOSE SCRIPT FILE   ***************************/

MODULE close_script_file (void)
{
    close_script ();
}


/**************************   PUSH SCRIPT POSITION   *************************/

MODULE push_script_position (void)
{
    cur_script_block. position = script_position ();
    push_script_block ();
}


/**************************   POP SCRIPT POSITION   **************************/

MODULE pop_script_position (void)
{
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;

    pop_script_block ();
    if (cur_script_block. position)
      {
        restore_position (cur_script_block. position);

        if (xml_item)
            xml_put_attr (xml_item,
                          "script",
                          cur_script_file-> name);
      }
    else
        raise_exception (script_stack_empty_event);
}


/************************   PREPARE TO INTERPRET GSL   ***********************/

MODULE prepare_to_interpret_gsl (void)
{
    char
        buffer [32];
    SCRIPT_LINE
        *position;

    if (script_root-> op1-> type == GG_SOURCE)
      {
        /*  .gsl from <op1->op1>  */
        if (! script_root-> op1-> op1)
          {
            gg_report_error ('E', "Illegal GSL source.");
            raise_exception (anomaly_event);
            return;
          }
        if (evaluate_script (script_root-> op1-> op1))
          {
            raise_exception (anomaly_event);
            return;
          }
        if (prepare_script (string_result (script_root-> op1-> op1)))
            raise_exception (anomaly_event);
      }
    else
      {
        /*  .gsl <op1> */
        if (evaluate_script (script_root-> op1))
          {
            raise_exception (anomaly_event);
            return;
          }

        position = script_position ();
        snprintf (buffer, 32, "(%s %u)",
                              position-> parent-> name,
                              position-> line);
        open_script_text (buffer, string_result (script_root-> op1));
      }
}


/*************************   COPY DIRECT TO OUTPUT   *************************/

MODULE copy_direct_to_output (void)
{
    char
        line [LINE_MAX + 2];
    FILE
        *file;

    if (script_root-> op1-> type == GG_SOURCE)
      {
        if ((evaluate_script (script_root-> op1-> op1))
        ||  (evaluate_script (script_root-> op1-> op2)))
          {
            raise_exception (anomaly_event);
            return;
          }

        if (script_root-> op1-> op1)
          {
            /*  .literal from <op1->op1>  */
            file = file_locate (PATH, string_result (script_root-> op1-> op1),
                                NULL);
            if (! file)
              {
                gg_report_error ('E', "Can't open literal file %s",
                                      script_root-> op1-> op1-> result_s);
                raise_exception (anomaly_event);
                return;
              }
            while (file_read (file, line))
                print_to_output_or_stdout (line, TRUE);

            file_close (file);
          }
        if (script_root-> op1-> op2)
          {
            /*  .literal   << <op1->op2>  */
            string_result (script_root-> op1-> op2);
            while (read_next_script_line ())
                if (strncmp (cur_script_file-> cur_line-> text,
                             script_root-> op1-> op2-> result_s,
                             strlen (script_root-> op1-> op2-> result_s)) == 0)
                    break;
                else
                    print_to_output_or_stdout (cur_script_file-> cur_line-> text,
                                               TRUE);
          }
      }
    else
      {
        /*  .literal <op1> */
        if (evaluate_script (script_root-> op1))
          {
            raise_exception (anomaly_event);
            return;
          }

        print_to_output_or_stdout (string_result (script_root-> op1),
                                   TRUE);
      }
}


/****************************   LOAD SOURCE XML   ****************************/

MODULE load_source_xml (void)
{
    char
       *buffer;
    XML_ITEM
        *xml_item,
        *xml_next,
        *file_root;
    int
        rc;
    
    /* .xml [to <op2>] [as <op3>] [before <pretty> | after <format>] ... */

    if ((evaluate_script (script_root-> op2))
    ||  (evaluate_script (script_root-> op3))
    ||  (evaluate_script (script_root-> pretty))
    ||  (evaluate_script (script_root-> format)))
      {
        raise_exception (anomaly_event);
        return;
      }
    if (parse_locator ())
      {
        raise_exception (anomaly_event);
        return;
      }

    if ((script_root-> op1-> type == GG_SOURCE)
    &&  (script_root-> op1-> op1))
      {
        /*  .xml from <op1->op1>  */
        if (evaluate_script (script_root-> op1-> op1))
          {
            raise_exception (anomaly_event);
            return;
          }

        file_root = xml_create (NULL, NULL);
        rc = xml_load_file (& file_root,
                            PATH,
                            string_result (script_root-> op1-> op1),
                            TRUE);
      }
    else
      {
        if ((script_root-> op1-> type == GG_SOURCE)
        &&  (script_root-> op1-> op2))
          {
            /*  .xml   << <op1->op2>  */
            if (evaluate_script (script_root-> op1-> op2))
              {
                raise_exception (anomaly_event);
                return;
              }

            if (collect_source_text (& buffer,
                                     string_result (script_root-> op1-> op2)))
              {
                raise_exception (anomaly_event);
                return;
              }
          }
        else
          {
            /*  .xml <op1> */
            if (evaluate_script (script_root-> op1))
              {
                raise_exception (anomaly_event);
                return;
              }
            buffer = string_result (script_root-> op1);
          }

        file_root = xml_create (NULL, NULL);
        rc = xml_load_string (& file_root,
                              buffer,
                              TRUE);
      }
    if ((script_root-> op1-> type == GG_SOURCE)
    &&  (script_root-> op1-> op2))
        mem_free (buffer);
        
    if (rc)
      {
        gg_report_error ('E', "%s", xml_error ());
        raise_exception (anomaly_event);
      }
    else
      {
        xml_item = xml_first_child (file_root);
        while (xml_item)
          {
            xml_next = xml_next_sibling (xml_item);
            xml_detach (xml_item);
            if (script_root-> op3)
                xml_rename (xml_item,
                            string_result (script_root-> op3));

            if (the_sibling)
                xml_attach_sibling (the_sibling, xml_item);
            else
                xml_attach_child   (the_parent,  xml_item);
            xml_item = xml_next;
          }
      }
    xml_free (file_root);
}


/*************************   LOAD SOURCE JAVASCRIPT   ************************/

MODULE load_source_javascript (void)
{
#if defined (JAVASCRIPT)
    char
        token  [16],
        point;
    int
        start_line;
    JSScript
        *script;
    JSBool
        ok;
    jsval 
        result;
    char
       *buffer = NULL;

    /* .javascript [to <op2>] [as <op3>] [before <pretty> | after <format>] */

    if ((script_root-> op1-> type == GG_SOURCE)
    &&  (script_root-> op1-> op1))
      {
        if (evaluate_script (script_root-> op1-> op1))
          {
            raise_exception (anomaly_event);
            return;
          }

        /*  This is off the top of my head - probably wrong  */
        script = JS_CompileFile(cx, obj,
                                string_result (script_root-> op1-> op1),
                                cur_script_file-> name, start_line);
      }
    else
      {
        if ((script_root-> op1-> type == GG_SOURCE)
        &&  (script_root-> op1-> op2))
          {
            if (evaluate_script (script_root-> op1-> op2))
              {
                raise_exception (anomaly_event);
                return;
              }

            if (collect_source_text (& buffer,
                                     string_result (script_root-> op1-> op2)))
              {
                raise_exception (anomaly_event);
                return;
              }
          }
        else
          {
            if (evaluate_script (script_root-> op1))
              {
                raise_exception (anomaly_event);
                return;
              }
            buffer = string_result (script_root-> op1);
          }

        script = JS_CompileScript(cx, obj,
                                  buffer, strlen (buffer),
                                  cur_script_file-> name, start_line);

        if ((script_root-> op1-> type == GG_SOURCE)
        &&  (script_root-> op1-> op2))
            mem_free (buffer);
      }
        
    if (!script)
        ok = JS_FALSE;
    else
      {
        ok = JS_ExecuteScript(cx, obj, script, &result);
        JS_DestroyScript(cx, script);
      }
    if (!ok)                  /* Assume JS has already reported the error    */
        raise_exception (anomaly_event);
#else
    gg_report_error ('E', "Javascript not supported.");
    raise_exception (anomaly_event);
#endif
}




/*************************   INITIALISE MACRO BLOCK   ************************/

MODULE initialise_macro_block (void)
{
    SYMBOL
        *symbol;
    SCOPE_BLOCK
        *scope_block;

    if ((evaluate_script      (script_root-> op2))
    ||  (evaluate_script_node (script_root-> op3)))
      {
        raise_exception (anomaly_event);
        return;
      }

    symbol = sym_lookup_symbol (macros, string_result (script_root-> op2));
    if (!symbol)
      {
        gg_report_error ('E', "Undefined macro: %s", 
                              string_result (script_root-> op2));
        raise_exception (anomaly_event);
        return;
      }

    scope_block = create_scope_block (string_result (script_root-> op2));
    scope_block-> xml_item = xml_new (NULL, "", "");
    if (substitute_parameters_into_xml (scope_block-> xml_item,
                                        ((MACRO *) (symbol-> data))-> args,
                                        script_root-> op3))
      {
        raise_exception (anomaly_event);
        return;
      }
    gg_clean (script_root);

    restore_position (((MACRO *) (symbol-> data))-> position);
    cur_script_block. template = ((MACRO *) (symbol-> data))-> template;

    /*  And push a MACRO or FUNCTION control onto control stack              */
    if (cur_script_block. template)
        push_control (CTL_MACRO, NULL);
    else
        push_control (CTL_FUNCTION, NULL);
}


static int
substitute_parameters_into_xml (XML_ITEM *xml_item,
                                SCRIPT_NODE *args,
                                SCRIPT_NODE *params)
{
    SCRIPT_NODE
        *arg,
        *param,
        *param_parent = NULL;

    if (! params)
        return 0;

    arg   = args;
    param = params;
    /*  Must be at least as many args as params  */
    while (param)
      {
        if (! arg)
          {
            pop_script_position ();
            gg_report_error ('E', "Mismatched parameters.");
            return -1;
          }
        if ((param-> type == GG_OPERATOR)
        &&  (param-> operator == OP_NEXT_ARG))
          {
            param_parent = param;
            param = param-> op1;
            arg   = arg  -> op1;
          }
        else
            break;
      }
    while ((arg)
       && (arg-> type == GG_OPERATOR)
       && (arg-> operator == OP_NEXT_ARG))
        arg = arg-> op1;

    substitute_parameter_into_xml (xml_item, arg, param);
    while (param != params)
      {
        param = param_parent;
        param_parent = param-> parent;
        arg   = arg-> parent;
        substitute_parameter_into_xml (xml_item, arg-> op2, param-> op2);
      }
    return 0;
}


static void
substitute_parameter_into_xml (XML_ITEM *xml_item,
                                SCRIPT_NODE *arg,
                                SCRIPT_NODE *param)
{
    char
        *name;

    name = arg-> text;                  /*  Don't evaluate simple text       */
    ASSERT (name);
    xml_put_attr_ic (xml_item,
                     name,
                     param
                     ? (param-> result_type != TYPE_UNDEFINED
                        ? string_result (param)
                        : NULL)
                     : NULL,
                     ignorecase, TRUE);
}


/*************************   PUSH TEMPLATE CONTROL   *************************/

MODULE push_template_control (void)
{
    if (evaluate_script (script_root-> op1))
      {
        raise_exception (anomaly_event);
        return;
      }
    push_control (CTL_TEMPLATE, (void *) (long) cur_script_block.template);
    cur_script_block.template = script_root-> op1
                              ? (int) number_result (script_root-> op1) : 1;
}


/**************************   POP TEMPLATE CONTROL   *************************/

MODULE pop_template_control (void)
{
    CONTROL_BLOCK
        control = {CTL_UNDEFINED, NULL, NULL};

    if (!list_empty (& cur_script_block. control_stack))
        pop_control (& control);

    if (control.type != CTL_TEMPLATE)
      {
        unmatched_control_error (& control);
        raise_exception (anomaly_event);
        return;
      }

    cur_script_block.template = (long) control.extra;
}


/***************************   CLOSE OUTPUT FILE   ***************************/

MODULE close_output_file (void)
{
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;

    if (output)
        file_close (output);

    output = NULL;
    mem_strfree (&output_name);
    if (xml_item)
      {
        xml_put_attr (xml_item,
                      "outfile",
                      NULL);
        xml_put_attr (xml_item,
                      "line",
                      NULL);
      }
}


/**************************   INITIALISE FOR BLOCK   *************************/

MODULE initialise_for_block (void)
{
    SCOPE_BLOCK
        *scope_block;
    Bool
        where_condition;
    SCOPE_ITEM
        *scope_item;
    XML_ITEM
        *from_xml,
        *xml_item;
    char
        *xml_name,
        *scope_name;
    long
        item;

    /*  .for <op1>.<op2> as <op3> where <pretty> by <format> */

    if ((evaluate_script (script_root-> op1))
    ||  (evaluate_script (script_root-> op2))
    ||  (evaluate_script (script_root-> op3)))
      {
        raise_exception (anomaly_event);
        return;
      }

    from_xml = lookup_from_xml (script_root-> op1);
    if (! from_xml)
      {
        raise_exception (anomaly_event);
        return;
      }

    scope_name = script_root-> op2
	           ? string_result (script_root-> op2)
		   : NULL;
    scope_block = create_scope_block (script_root-> op3
                                      ? string_result (script_root-> op3)
                                      : scope_name
                                          ? scope_name
                                          : "");
    scope_block-> children = TRUE;
    xml_item = xml_first_child (from_xml);
    item = 0;
    while (xml_item)
      {
        while (xml_item)
          {
            xml_name = xml_item_name (xml_item);
            if (xml_name)
              {
                if (scope_name)
                  {
                    if (ignorecase)
                      {
                        if (lexcmp (xml_name, scope_name) == 0)
                            break;
                      }
                    else
                      {
                        if (streq (xml_name, scope_name))
                            break;
                      }
                  }
                else
                    /*  If no child name specified, take all named children  */
                    break;
              }

            xml_item = xml_next_sibling (xml_item);
          }
        item++;

        if (xml_item)
          {
            scope_item = create_scope_item (scope_block, xml_item, item);
            scope_block-> scope_item = scope_item;
            scope_block-> xml_item   = xml_item;
          
            if (script_root-> pretty)  /*  Where clause  */
              {
                if (evaluate_script (script_root-> pretty) != 0)
                  {
                    raise_exception (anomaly_event);
                    return;
                  }
                where_condition = (Bool) number_result (script_root-> pretty);
                gg_clean (script_root-> pretty);
                if (! where_condition)
                  {
                    destroy_scope_item (scope_item);
                    xml_item = xml_next_sibling (xml_item);
                    continue;
                  }
              }
          }
        
        if (xml_item)
          {
            if (script_root-> format)      /*  By clause  */
              {
                if (evaluate_script (script_root-> format) != 0)
                  {
                    raise_exception (anomaly_event);
                    return;
                  }
                string_result (script_root-> format);
                number_result (script_root-> format);
                scope_item-> result_type = script_root-> format-> result_type;
                scope_item-> result_n    = script_root-> format-> result_n;
                scope_item-> result_s    = mem_strdup (script_root-> format-> result_s);
                gg_clean (script_root-> format);
              }

            xml_item = xml_next_sibling (xml_item);
          }
      }         

    /*  Sort if necessary  */
    if (script_root-> format)
        list_sort (& scope_block-> item_list, sort_compare);

    if (first_scope_item (scope_block))
        push_control (CTL_FOR, NULL);
    else
      {
        skip_control_block (CTL_FOR);
        destroy_scope_block ();
      }
}


static Bool
sort_compare (LIST *t1, LIST *t2)
{
    if ((((SCOPE_ITEM *) t1)-> result_type == TYPE_NUMBER)
    &&  (((SCOPE_ITEM *) t1)-> result_type == TYPE_NUMBER))
        return (((SCOPE_ITEM *) t1)-> result_n
             >  ((SCOPE_ITEM *) t2)-> result_n);
    else
        return (strcmp (((SCOPE_ITEM *) t1)-> result_s,
                        ((SCOPE_ITEM *) t2)-> result_s) > 0);
}


/***************************   ITERATE FOR BLOCK   ***************************/

MODULE iterate_for_block (void)
{
    CONTROL_BLOCK
        control = {CTL_UNDEFINED, NULL, NULL};

    if (!list_empty (& cur_script_block. control_stack))
        pop_control (& control);

    if (control.type != CTL_FOR)
      {
        unmatched_control_error (& control);
        raise_exception (anomaly_event);
        return;
      }

    if (next_scope_item (last_scope_block ()))
      {
        /*  Push same control back  */
        list_push (& cur_script_block. control_stack, control);
        restore_position (control. position);
      }
    else
        destroy_scope_block ();
}


/*************************   INITIALISE WHILE BLOCK   ************************/

MODULE initialise_while_block (void)
{
    Bool
        where_condition;

    if (evaluate_script (script_root-> op1) != 0)
      {
        raise_exception (anomaly_event);
        return;
      }

    where_condition = (Bool) number_result (script_root-> op1);
    gg_clean (script_root-> op1);
    if (where_condition)
        push_control (CTL_WHILE, script_root-> op1);
    else
        skip_control_block (CTL_WHILE);
}


/**************************   ITERATE WHILE BLOCK   **************************/

MODULE iterate_while_block (void)
{
    CONTROL_BLOCK
        control = {CTL_UNDEFINED, NULL, NULL};
    Bool
        condition;

    if (!list_empty (& cur_script_block. control_stack))
        pop_control (& control);

    if (control.type != CTL_WHILE)
      {
        unmatched_control_error (& control);
        raise_exception (anomaly_event);
        return;
      }

    if (evaluate_script ((SCRIPT_NODE *) control. extra) != 0)
      {
        raise_exception (anomaly_event);
        return;
      }

    condition = (Bool) number_result ((SCRIPT_NODE *) control. extra);
    gg_clean ((SCRIPT_NODE *) control. extra);
    if (condition)
      {
        /*  Push same control back */
        list_push (& cur_script_block. control_stack, control);
        restore_position (control. position);
      }
}


/***********************   ITERATE FOR OR WHILE BLOCK   **********************/

MODULE iterate_for_or_while_block (void)
{
    CONTROL_BLOCK
        control = {CTL_UNDEFINED, NULL, NULL};
    Bool
        condition;

    while (!list_empty (& cur_script_block. control_stack))
      {
        pop_control (& control);
        if ((control.type == CTL_FOR)
        ||  (control.type == CTL_WHILE))
            break;
      }

    if ((control.type != CTL_FOR)
    &&  (control.type != CTL_WHILE))
      {
        gg_report_error ('E', "No loop for .next");
        raise_exception (anomaly_event);
        return;
      }

    restore_position (control. position);
    if (control.type == CTL_FOR)
        if (next_scope_item (last_scope_block ()))
            /*  Push same control back  */
            list_push (& cur_script_block. control_stack, control);
        else
          {
            skip_control_block (CTL_FOR);
            destroy_scope_block ();
          }
    else
      {
        if (evaluate_script ((SCRIPT_NODE *) control. extra) != 0)
          {
            raise_exception (anomaly_event);
            return;
          }

        condition = (Bool) number_result ((SCRIPT_NODE *) control. extra);
        gg_clean ((SCRIPT_NODE *) control. extra);
        if (condition)
            /*  Push same control back */
            list_push (& cur_script_block. control_stack, control);
        else
            skip_control_block (CTL_WHILE);
      }
}


/**********************   TERMINATE FOR OR WHILE BLOCK   *********************/

MODULE terminate_for_or_while_block (void)
{
    CONTROL_BLOCK
        control = {CTL_UNDEFINED, NULL, NULL};

    while (!list_empty (& cur_script_block. control_stack))
      {
        pop_control (& control);
        if ((control.type == CTL_FOR)
        ||  (control.type == CTL_WHILE))
            break;
      }

    if ((control.type != CTL_FOR)
    &&  (control.type != CTL_WHILE))
      {
        gg_report_error ('E', "No loop for .last");
        raise_exception (anomaly_event);
        return;
      }

    restore_position (control. position);
    if (control.type == CTL_FOR)
      {
        skip_control_block (CTL_FOR);
        destroy_scope_block ();
      }
    else
        skip_control_block (CTL_WHILE);
}


/*************************   SKIP IF BLOCK IF FALSE   ************************/

MODULE skip_if_block_if_false (void)
{
    if (evaluate_script (script_root-> op1))
      {
        raise_exception (anomaly_event);
        return;
      }

    if (number_result (script_root-> op1))
        push_control (CTL_IF, NULL);
    else
      {
        skip_control_block (CTL_IF);

        /*  Now skip .elsif until one condition is TRUE  */
        while (script_root ? (script_root-> type == GG_ELSIF) : FALSE)
          {
            if (evaluate_script (script_root-> op1) != 0)
              {
                raise_exception (anomaly_event);
                return;
              }
            if (number_result (script_root-> op1))
              {
                push_control (CTL_IF, NULL);
                return;
              }
            else
                skip_control_block (CTL_IF);
          }

        /*  If we hit a .else then we continue processing  */
        if (script_root && script_root-> type == GG_ELSE)
            push_control (CTL_ELSE, NULL);
      }
}


/*************************   SKIP ELSE BLOCK ALWAYS   ************************/

MODULE skip_else_block_always (void)
{
    CONTROL_BLOCK
        control = {CTL_UNDEFINED, NULL, NULL};

    if (!list_empty (& cur_script_block. control_stack))
        pop_control (& control);

    if (control.type != CTL_IF)
      {
        unmatched_control_error (& control);
        raise_exception (anomaly_event);
      }
    else
        skip_control_block (CTL_ELSE);
}


/************************   SKIP ELSIF BLOCK ALWAYS   ************************/

MODULE skip_elsif_block_always (void)
{
    CONTROL_BLOCK
        control = {CTL_UNDEFINED, NULL, NULL};

    if (!list_empty (& cur_script_block. control_stack))
        pop_control (& control);

    if (control.type != CTL_IF)
      {
        unmatched_control_error (& control);
        raise_exception (anomaly_event);
      }
    else
        while ((script_root)
           && ((script_root-> type == GG_ELSIF)
           ||  (script_root-> type == GG_ELSE)))
            skip_control_block (CTL_IF);
}


/*****************************   CLOSE IF BLOCK   ****************************/

MODULE close_if_block (void)
{
    CONTROL_BLOCK
        control = {CTL_UNDEFINED, NULL, NULL};

    if (!list_empty (& cur_script_block. control_stack))
        pop_control (& control);

    if ((control.type != CTL_IF)
    &&  (control.type != CTL_ELSE))
      {
        unmatched_control_error (& control);
        raise_exception (anomaly_event);
      }
}


/**************************   ECHO TEXT TO CONSOLE   *************************/

MODULE echo_text_to_console (void)
{
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;
    char
        *me = xml_item ? xml_get_attr (xml_item, "me", "") : "";

    if (evaluate_script (script_root-> op1))
      {
        raise_exception (anomaly_event);
        return;
      }

    if (script_root-> op1)
        coprintf ("%s M: %s", me, string_result (script_root-> op1));
    else
        coprintf ("%s M:", me);
}


/**************************   COPY LINE TO OUTPUT   **************************/

MODULE copy_line_to_output (void)
{
    Bool
        cobol;                          /*  Are we producing COBOL code?     */
    int
        indent;                         /*  Desired line indentation         */
    char
        *value,
        *text,                          /*  Script text line                 */
        *buffer;                        /*  Output buffer                    */
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;

    script_root-> shuffle_in = line_length;
    if (evaluate_script (script_root))
      {
        raise_exception (anomaly_event);
        return;
      }

    indent = script_root-> indent;
    text   = string_result (script_root);
    value = xml_item ? xml_get_attr (xml_item, "cobol", NULL) : NULL;
    cobol = (value != NULL);

    if (cobol && (output_line > last_line))
      {
        if (indent >= 6)
            indent -= 6;
        else
          {
            if (strlen (text) > 6)
                text += (6 - indent);
            else
                text = "";

            indent = 0;
          }
        buffer = mem_alloc (6 + indent + strlen (text) + 2);
        ASSERT (buffer);
        line_length += sprintf (buffer, "%04d00%*s%s",
                                output_line, indent, "", text);
        last_line = output_line;
      }
    else
      {
        buffer = mem_alloc (indent + strlen (text) + 2);
        ASSERT (buffer);
        line_length += sprintf (buffer, "%*s%s", indent, "", text);
      }

    if (! script_root-> extend)
      {
        output_line++;
        line_length = 0;
        if (xml_item)
            put_numeric_attr (xml_item,
                              "line", 
                              output_line);
      }

    print_to_output_or_stdout (buffer, (Bool) (! script_root-> extend));
    mem_free (buffer);
}


void
print_to_output_or_stdout (char *text, Bool line_break)
{
    if (output)
      {
        fprintf (output, "%s", text);
        if (line_break)
            fprintf (output, "\n");
      }
    else
      {
        if (stdout_fct)
          {
            (stdout_fct) (text);
            if (line_break)
                (stdout_fct) ("\n");
          }
        if (stdout_echo)
          {
            fprintf (stdout, "%s", text);
            if (line_break)
                fprintf (stdout, "\n");
          }
      }
}


/***************************   CLOSE MACRO BLOCK   ***************************/

MODULE close_macro_block (void)
{
    close_macro_or_function_block (CTL_MACRO);
}


static void
close_macro_or_function_block (CONTROL_TYPE type)
{
    CONTROL_BLOCK
        control = {CTL_UNDEFINED, NULL, NULL};

    if (!list_empty (& cur_script_block. control_stack))
        pop_control (& control);

    if (control.type != type)
      {
        unmatched_control_error (& control);
        raise_exception (anomaly_event);
        return;
      }

    /*  Free xml parameter block  */
    destroy_scope_block ();
}


/**************************   CLOSE FUNCTION BLOCK   *************************/

MODULE close_function_block (void)
{
    close_macro_or_function_block (CTL_FUNCTION);
}


/**************************   INITIALISE NEW BLOCK   *************************/

MODULE initialise_new_block (void)
{
    SCOPE_BLOCK
        *scope_block;
    XML_ITEM
        *xml_item;

    /*  .new <op1> to <op2> as <op3> [before <pretty> | after <format>] */

    if ((evaluate_script (script_root-> op1))
    ||  (evaluate_script (script_root-> op2))
    ||  (evaluate_script (script_root-> op3))
    ||  (evaluate_script (script_root-> pretty))
    ||  (evaluate_script (script_root-> format)))
      {
        raise_exception (anomaly_event);
        return;
      }

    if (parse_locator ())
      {
        raise_exception (anomaly_event);
        return;
      }

    xml_item = xml_create (script_root-> op1
                           ? script_root-> op1-> result_s
                           : NULL,
                           NULL);
    ASSERT (xml_item);

    if (the_sibling)
        xml_attach_sibling (the_sibling, xml_item);
    else
        xml_attach_child   (the_parent,  xml_item);

    scope_block = create_scope_block (script_root-> op3
                                      ? string_result (script_root-> op3)
                                      : script_root-> op1
                                          ? string_result (script_root-> op1)
                                          : "");
    scope_block-> children = TRUE;
    scope_block-> xml_item = xml_item;

    /*  And push a NEW control onto control stack                            */
    push_control (CTL_NEW, NULL);
}


static int
parse_locator (void)
{
    SCRIPT_NODE
        *sibling_node;

    if (script_root-> op2)
      {
        the_parent = lookup_from_xml (script_root-> op2);
        if (! the_parent)
            return -1;
      }

    if (! (script_root-> op2
        || script_root-> pretty
        || script_root-> format))
      {
        the_sibling = NULL;
        the_parent  = lookup_from_xml (NULL);
        if (! the_parent)
            return -1;
        return 0;
      }            

    if (script_root-> pretty && script_root-> format)
      {
        gg_report_error ('E',
                         "Only one of 'before' and 'after' may be specified.");
        return -1;
      }

    if (script_root-> pretty || script_root-> format)
      {
        sibling_node = script_root-> pretty ? script_root-> pretty
                                            : script_root-> format;
        the_sibling = lookup_from_xml (sibling_node);
        if (! the_sibling)
            return -1;

        if (script_root-> op2)
          {
            if (the_parent != xml_parent (the_sibling))
              {
                gg_report_error ('E', "Scope %s is not a child of scope %s",
                                 string_result (sibling_node),
                                 extended_scope_string (script_root-> op2));
                return -1;
              }
          }
        else
          {
            the_parent = xml_parent (the_sibling);
            if (! the_parent)
              {
                gg_report_error ('E', "Scope %s has no parent.",
                                 string_result (sibling_node));
                return -1;
              }
          }
      }
    else                       /*  No 'before' nor 'after'  */
        the_sibling = NULL;

    if (script_root-> format)
        the_sibling = xml_next_sibling (the_sibling);

    return 0;
}


/****************************   CLOSE NEW BLOCK   ****************************/

MODULE close_new_block (void)
{
    CONTROL_BLOCK
        control = {CTL_UNDEFINED, NULL, NULL};

    if (!list_empty (& cur_script_block. control_stack))
        pop_control (& control);

    if (control.type != CTL_NEW)
      {
        unmatched_control_error (& control);
        raise_exception (anomaly_event);
        return;
      }

    last_scope_block ()-> xml_item = NULL;   /*  Retain the new item  */
    destroy_scope_block ();
}


/****************************   DELETE XML ITEM   ****************************/

MODULE delete_xml_item (void)
{
    SCOPE_BLOCK
        *scope_block;
    SCOPE_ITEM
        *scope_item;
    XML_ITEM
        *xml_delete;

    /*  .delete <op1>  */

    if (evaluate_script (script_root-> op1))
      {
        raise_exception (anomaly_event);
        return;
      }

    xml_delete = lookup_from_xml (script_root-> op1);
    if (! xml_delete)
      {
        raise_exception (anomaly_event);
        return;
      }

    FORLIST (scope_block, scope_stack)
      {
        if (! list_empty (& scope_block-> item_list))
          {
            FORLIST (scope_item, scope_block-> item_list)
                replace_referred_xml_by_null (& scope_item-> xml_item,
                                              xml_delete);
            scope_block-> xml_item = scope_block-> scope_item-> xml_item;
          }
        else
            replace_referred_xml_by_null (& scope_block-> xml_item,
                                          xml_delete);
      }
    xml_free (xml_delete);
}


void 
replace_referred_xml_by_null (XML_ITEM **xml_referred, XML_ITEM *xml_delete)
{
    XML_ITEM
        *xml_scan;

    xml_scan = *xml_referred;
    while (xml_scan)
        if (xml_scan == xml_delete)
          {
            *xml_referred = NULL;
            break;
          }
        else
            xml_scan = xml_parent (xml_scan);
}


/*****************************   MOVE XML ITEM   *****************************/

MODULE move_xml_item (void)
{
    XML_ITEM
        *xml_item,
        *xml_scan;

    /*  .move <op1> to <op2> as <op3> [before <pretty> | after <format>] */

    if ((evaluate_script (script_root-> op1))
    ||  (evaluate_script (script_root-> op2))
    ||  (evaluate_script (script_root-> op3))
    ||  (evaluate_script (script_root-> pretty))
    ||  (evaluate_script (script_root-> format)))
      {
        raise_exception (anomaly_event);
        return;
      }

    xml_item = lookup_from_xml (script_root-> op1);
    if (! xml_item)
      {
        raise_exception (anomaly_event);
        return;
      }

    if (parse_locator ())
      {
        raise_exception (anomaly_event);
        return;
      }

    xml_scan = the_parent;
    while (xml_scan)
      {
        if (xml_scan == xml_item)
          {
            gg_report_error ('E', "Attempt to make object its own child.");
            raise_exception (anomaly_event);
            return;
          }
        xml_scan = xml_parent (xml_scan);
      }

    if (the_sibling == xml_item)
        the_sibling = xml_next_sibling (the_sibling);

    xml_detach (xml_item);
    if (the_sibling)
        xml_attach_sibling (the_sibling, xml_item);
    else
        xml_attach_child   (the_parent,  xml_item);

    if (script_root-> op3)
        xml_rename (xml_item,
                    string_result (script_root-> op3));
}


/*****************************   COPY XML ITEM   *****************************/

MODULE copy_xml_item (void)
{
    XML_ITEM
        *to_item,
        *from_item;

    /*  .copy <op1> to <op2> as <op3> [before <pretty> | after <format>] */

    if ((evaluate_script (script_root-> op1))
    ||  (evaluate_script (script_root-> op2))
    ||  (evaluate_script (script_root-> op3))
    ||  (evaluate_script (script_root-> pretty))
    ||  (evaluate_script (script_root-> format)))
      {
        raise_exception (anomaly_event);
        return;
      }

    from_item = lookup_from_xml (script_root-> op1);
    if (! from_item)
      {
        raise_exception (anomaly_event);
        return;
      }

    if (parse_locator ())
      {
        raise_exception (anomaly_event);
        return;
      }

    to_item = xml_create (script_root-> op3
                          ? string_result (script_root-> op3)
                          : xml_item_name (from_item),
                          NULL);
    ASSERT (to_item);
    copy_xml (to_item, from_item);

    if (the_sibling)
        xml_attach_sibling (the_sibling, to_item);
    else
        xml_attach_child   (the_parent,  to_item);
}


void
copy_xml (XML_ITEM *to, XML_ITEM *from)
{
    XML_ATTR
        *attr;
    XML_ITEM
        *to_child,
        *from_child;

    FORATTRIBUTES (attr, from)
        xml_put_attr (to,
                      xml_attr_name  (attr),
                      xml_attr_value (attr));

    for (from_child  = xml_first_child (from);
         from_child != NULL;
         from_child  = xml_next_sibling (from_child)) 
      {
        to_child = xml_new (to,
                            xml_item_name  (from_child),
                            xml_item_value (from_child));
        ASSERT (to_child);
        copy_xml (to_child, from_child);
      }
}


/*****************************   SAVE XML FILE   *****************************/

MODULE save_xml_file (void)
{
    char
        *filename;
    XML_ITEM
        *xml_item;

    if ((evaluate_script (script_root-> op1))
    ||  (evaluate_script (script_root-> op2)))
      {
        raise_exception (anomaly_event);
        return;
      }

    xml_item = lookup_from_xml (script_root-> op1);
    if (! xml_item)
      {
        raise_exception (anomaly_event);
        return;
      }

    if (script_root-> op2)
        filename = string_result (script_root-> op2);
    else
      {
        filename = mem_alloc (strlen (string_result (script_root-> op1)) + 5);
        strcpy (filename, script_root-> op1-> result_s);
        strcat (filename, ".xml");
      }
    xml_save_file (xml_item, filename);

    if (! script_root-> op2)
        mem_free (filename);
}


/*****************************   SORT XML ITEMS   ****************************/

MODULE sort_xml_items (void)
{
    SCOPE_BLOCK
        *scope_block;
    SCOPE_ITEM
        *scope_item;
    XML_ITEM
        *from_xml,
        *xml_item;
    char
        *xml_name,
        *scope_name;
    long
        item;

    /*  .sort <op1>.<op2> as <op3> by <format> */

    if ((evaluate_script (script_root-> op1))
    ||  (evaluate_script (script_root-> op2))
    ||  (evaluate_script (script_root-> op3)))
      {
        raise_exception (anomaly_event);
        return;
      }

    from_xml = lookup_from_xml (script_root-> op1);
    if (! from_xml)
      {
        raise_exception (anomaly_event);
        return;
      }

    scope_name = script_root-> op2
	           ? string_result (script_root-> op2)
		   : NULL;
    scope_block = create_scope_block (script_root-> op3
                                      ? string_result (script_root-> op3)
                                      : scope_name
                                          ? scope_name
                                          : "");
    scope_block-> children = TRUE;
    xml_item = xml_first_child (from_xml);
    item = 0;
    while (xml_item)
      {
        while (xml_item)
          {
            xml_name = xml_item_name (xml_item);
            if (xml_name)
              {
                if (scope_name)
                  {
                    if (ignorecase)
                      {
                        if (lexcmp (xml_name, scope_name) == 0)
                            break;
                      }
                    else
                      {
                        if (streq (xml_name, scope_name))
                            break;
                      }
                  }
                else
                    /*  If no child name specified, take all named children  */
                    break;
              }

            xml_item = xml_next_sibling (xml_item);
          }
        item++;

        if (xml_item)
          {
            scope_item = create_scope_item (scope_block, xml_item, item);
            scope_block-> scope_item = scope_item;
            scope_block-> xml_item   = xml_item;
          }
        
        if (xml_item)
          {
            if (script_root-> format)      /*  By clause  */
              {
                if (evaluate_script (script_root-> format) != 0)
                  {
                    raise_exception (anomaly_event);
                    return;
                  }
                string_result (script_root-> format);
                number_result (script_root-> format);
                scope_item-> result_type = script_root-> format-> result_type;
                scope_item-> result_n    = script_root-> format-> result_n;
                scope_item-> result_s    = mem_strdup (script_root-> format-> result_s);
                gg_clean (script_root-> format);
              }

            xml_item = xml_next_sibling (xml_item);
          }
      }         

    /*  Sort if necessary  */
    if (script_root-> format)
        list_sort (& scope_block-> item_list, sort_compare);

    if (first_scope_item (scope_block))
      {
        xml_detach (scope_block-> xml_item);
        xml_attach_child (from_xml, scope_block-> xml_item);
        while (next_scope_item (scope_block))
          {
            xml_detach (scope_block-> xml_item);
            xml_attach_child (from_xml, scope_block-> xml_item);
          }
      }
    destroy_scope_block ();
}


/***********************   VERIFY ALL CONTROLS CLOSED   **********************/

MODULE verify_all_controls_closed (void)
{
    CONTROL_BLOCK
        control;

    if (!list_empty (& cur_script_block. control_stack))
      {
        pop_control (& control);
        unmatched_control_error (& control);
        raise_exception (anomaly_event);
      }
}


/*************************   RETURN ABORT FEEDBACK   *************************/

MODULE return_abort_feedback (void)
{
    feedback = 1;
}


/************************   RETURN ANOMALY FEEDBACK   ************************/

MODULE return_anomaly_feedback (void)
{
    feedback = 1;
}


/***************************   GET EXTERNAL EVENT   **************************/

MODULE get_external_event (void)
{
}


/*************************   TERMINATE THE PROGRAM    ************************/

MODULE terminate_the_program (void)
{
    the_next_event = terminate_event;
}


/*  -------------------------------------------------------------------------
 *  gg_handle_signal -- internal
 *
 */

static void
gg_handle_signal (int the_signal)
{
    signal (SIGSEGV, SIG_DFL);
    signal (SIGABRT, SIG_DFL);
    if (the_signal == SIGSEGV)
        gg_report_error ('E', "Segmentation violation");
    else
    if (the_signal == SIGABRT)
        gg_report_error ('E', "Abort");
    else
        gg_report_error ('E', "Unknown signal");

    abort ();
}


/*  ---------------------------------------------------------------------[<]-
    Function: gg_send_stdout

    Synopsis: Redirects stdout output to a specified CONSOLE_FCT function.
    If the specified address is NULL, redirects back to the stdout stream.
    If the echo argument is TRUE, stdout output is also sent to stdout as
    normal.
    ---------------------------------------------------------------------[>]-*/

void
gg_send_stdout (CONSOLE_FCT *new_stdout_fct, Bool echo)
{
    stdout_fct  = new_stdout_fct;
    stdout_echo = echo;                 /*  Copy to stdout                   */
}
