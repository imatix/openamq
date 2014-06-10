/*===========================================================================*
 *                                                                           *
 *  ggpars.c - Script parser                                                 *
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

#include "sfl.h"                        /*  Public definitions               */
#include "ggpars.h"                     /*  Include public type declarations */
#include "ggpars.d"                     /*  Include dialog data              */

/*  Types */

typedef enum { EXPRESSION,
               TEXT,
               SUBSTITUTE,
               OPERAND,
               SIMPLE_OPERAND,
               BRACKETED_IDENTIFIER,
               CONTINUE_ATTRIBUTE,
               CONTINUE_SCOPE,
               CONTINUE_SCOPE_OR_ATTRIBUTE,
               QUOTED,
               IDENTIFIER,
               CALL,
               ARGUMENTS,
               MODIFIER,
               DEFINE,
               MACRO,
               INVOKE,
               SAVE,
               NEW,
               FOR,
               IMPLICIT_ASSIGN_OR_INVOKE,
               LOCATOR,
               LOAD,
               XML,
               SORT}
    STATE;

typedef struct {
    char
        token[16];
    event_t
        event;
} TOKEN_EVENT;
        
/*  Global variables */

static event_t state_event [] = {
    expression_event,
    text_event,
    substitute_event,
    operand_event,
    simple_operand_event,
    bracketed_identifier_event,
    continue_attribute_event,
    continue_scope_event,
    continue_scope_or_attribute_event,
    quoted_event,
    identifier_event,
    call_event,
    arguments_event,
    modifier_event,
    define_event,
    macro_event,
    invoke_event,
    save_event,
    new_event,
    for_event,
    implicit_assign_or_invoke_event,
    locator_event,
    load_event,
    xml_event,
    sort_event
};


int priority[] = {
    8,                                  /*  UNDEFINED      */
    7, 7,                               /*  TIMES, DIVIDE  */
    6, 6,                               /*  PLUS, MINUS    */
    5,                                  /*  DEFAULT        */
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /*  Comparisons    */
    3,                                  /*  NOT            */
    2, 2,                               /*  AND, OR        */
    1 };                                /*  NEXT_ARG       */


/*  Pointers into parse tree */
static SCRIPT_NODE **the_node_ptr,      /*  Ptr to ptr to current node       */
                   *the_parent,         /*  Ptr to parent of current node    */
                   *the_root,           /*  Ptr to root of parse tree        */
                   *feedback;           /*  Value returned to caller         */

/*  Variables concerning current token  */
static int         script_ptr;          /*  Offset to next char to parse     */
static int         prev_posn;           /*  Offset before parsing last token */
static int         token_posn;          /*  Offset of last token parsed      */
static int         token_length;        /*  Length of last token parsed      */
static int         token_spaces;        /*  Spaces before last token parsed  */
static char        the_token;           /*  Current expression token         */
static long        the_number;          /*  Current number value             */
static OPERATOR    the_operator;        /*  Expression operator              */
static OPERATOR    the_sign;            /*  Operand sign                     */
static int         brackets;            /*  Number of brackets to open       */
static int         deferred_length;     /*  Length of earlier tokens         */
static int         deferred_spaces;     /*  Spaces before earlier tokens     */

/*  Other variables  */
static event_t     start_event;         /*  What to start parsing            */
static SCRIPT_READ *script_read;        /*  The script reading function      */
static char        *script_line;
static char        buffer [LINE_MAX + 1];
static MEMTRN      *transaction;        /*  Transaction for memory rollback  */
static char        error_message [LINE_MAX + 1];

static LIST        state_stack,         /*  Dialog state stack               */
                   quote_stack;         /*  Quote character                  */


/*  Internal function prototypes  */

static void print_node                            (FILE *stream,
                                                   int   level,
                                                   SCRIPT_NODE *node);
static void print_string                          (FILE *stream,
                                                   int level,
                                                   char *text);
static void print_indented                        (FILE *stream, 
                                                   int level, 
                                                   char *format, ...);
static SCRIPT_NODE *
            gg_start_parsing                      (void);
static void collect_spaces                        (void);
static void generate_text_event                   (char quote);
static void generate_extend_event                 (void);
static void generate_end_of_line_event            (void);
static void generate_other_event                  (void);
static void generate_simple_token_event           (const char *text,
                                                   event_t event);
static void generate_sign_event                   (void);
static void generate_number_event                 (void);
static void generate_literal_event                (void);
static void recognise_script_command_token        (void);
static event_t recognise_token                    (char *token,
                                                   TOKEN_EVENT token_event[],
                                                   int num);
static void generate_spaces_event                 (void);
static void generate_assign_event                 (void);
static void generate_unary_operator_event         (void);
static void generate_operator_event               (void);
static OPERATOR recognise_operator                (char ch);
static void generate_next_arg_event               (void);
static void recognise_script_token                (void);
static void generate_extra_literal_event          (void);
static void generate_modifier_text_event          (void);
static void generate_comment_text_event           (void);
static void insert_simple_node                    (SCRIPT_NODE_TYPE type);
static void insert_the_node                       (SCRIPT_NODE *app_node,
                                                   OPERATOR operator);
static void find_operator_insertion_point         (OPERATOR operator);
static int  return_one_level                      (void);

static void error_exception                       (char *format, ...);


/*  ---------------------------------------------------------------------[<]-
    Function: gg_free

    Synopsis: Frees all memory allocated to a parse tree.  Assumes that all
    pointer fields are either NULL or allocated with separate calls to
    sflmem memory allocation functions.
    ---------------------------------------------------------------------[>]-*/

void gg_free (SCRIPT_NODE *node)
{
    if (node)
      {
        mem_free (node-> result_s);
        mem_free (node-> text);
        gg_free (node-> op1);
        gg_free (node-> op2);
        gg_free (node-> op3);
        gg_free (node-> pretty);
        gg_free (node-> format);
        mem_free (node);
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: gg_clean

    Synopsis: Cleans up a parse tree, that is, frees additional memory
    allocated since parsing.  This allows an expression to be re-evaluated
    without first being re-parsed.
    ---------------------------------------------------------------------[>]-*/

void gg_clean (SCRIPT_NODE *node)
{
    if (node && (! node-> constant))
      {
        node-> result_type = TYPE_UNDEFINED;
        if (node-> result_s)
            mem_strfree (&node-> result_s);

        if (node-> op1)
            gg_clean (node-> op1);
        if (node-> op2)
            gg_clean (node-> op2);
        if (node-> op3)
            gg_clean (node-> op3);
        if (node-> pretty)
            gg_clean (node-> pretty);
        if (node-> format)
            gg_clean (node-> format);
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: gg_print

    Synopsis: Prints a textual representation of a parse tree to the
    specified stream.
    ---------------------------------------------------------------------[>]-*/

void gg_print (FILE *stream, SCRIPT_NODE *node)
{
    print_node (stream, 0, node);
}


void print_node (FILE *stream, int level, SCRIPT_NODE *node)
{
    if (!node)
        return;

    print_node_type (stream, level, node-> type);
    print_indented (stream, level, "  length: %u\n", node-> length);
    if (node-> brackets)
        print_indented (stream, level, "  brackets: %u\n", node-> brackets);
    if (node-> spaces)
        print_indented (stream, level, "  spaces: %u\n", node-> spaces);
    if (node-> extend)
        print_indented (stream, level, "  extend: TRUE\n");
    if (node-> line_break)
        print_indented (stream, level, "  line break: TRUE\n");
    if (node-> operator)
      {
        print_indented (stream, level, "  operator: ", level*4, "");
        fprintf (stream, "%s", operator_text (node-> operator));
        fprintf (stream, "\n");
      }
    if (node-> text)
        print_indented (stream, level, "  text: %s\n", node-> text);
    if (node-> op1)
      {
        print_string (stream, level, "  op1:");
        print_node (stream, level + 1, node-> op1);
      }
    if (node-> op2)
      {
        print_string (stream, level, "  op2:");
        print_node (stream, level + 1, node-> op2);
      }
    if (node-> op3)
      {
        print_string (stream, level, "  op3:");
        print_node (stream, level + 1, node-> op3);
      }
    if (node-> pretty)
      {
        print_string (stream, level, "  pretty:");
        print_node (stream, level + 1, node-> pretty);
      }
    if (node-> format)
      {
        print_string (stream, level, "  format:");
        print_node (stream, level + 1, node-> format);
      }
}


void print_node_type (FILE *stream, int level, SCRIPT_NODE_TYPE type)
{
    switch (type)
      {
        case GG_COMMENT    :  print_string (stream, level, "COMMENT");   break;
        case GG_TEXT       :  print_string (stream, level, "TEXT");      break;
        case GG_SUBSTITUTE :  print_string (stream, level, "SUBSTITUTE");break;
        case GG_LITERAL    :  print_string (stream, level, "LITERAL");   break;
        case GG_NUMBER     :  print_string (stream, level, "NUMBER");    break;
        case GG_SYMBOL     :  print_string (stream, level, "SYMBOL");    break;
        case GG_MEMBER     :  print_string (stream, level, "MEMBER");    break;
        case GG_CALL   :  print_string (stream, level, "CALL");  break;
        case GG_OPERAND    :  print_string (stream, level, "OPERAND");   break;
        case GG_OPERATOR   :  print_string (stream, level, "OPERATOR");  break;
        case GG_CLOSE      :  print_string (stream, level, "CLOSE");     break;
        case GG_ELSE       :  print_string (stream, level, "ELSE");      break;
        case GG_END_IF     :  print_string (stream, level, "END_IF");    break;
        case GG_END_FOR    :  print_string (stream, level, "END_FOR");   break;
        case GG_END_MACRO  :  print_string (stream, level, "END_MACRO"); break;
        case GG_END_FUNCTION :
                              print_string (stream, level, "END_FUNCTION");
                                                                         break;
        case GG_END_NEW    :  print_string (stream, level, "END_NEW");   break;
        case GG_END_WHILE  :  print_string (stream, level, "END_WHILE"); break;
        case GG_OUTPUT     :  print_string (stream, level, "OUTPUT");    break;
        case GG_APPEND     :  print_string (stream, level, "APPEND");    break;
        case GG_GSL        :  print_string (stream, level, "GSL");       break;
        case GG_DIRECT     :  print_string (stream, level, "DIRECT");    break;
        case GG_XML        :  print_string (stream, level, "XML");       break;
        case GG_JAVASCRIPT :  print_string (stream, level, "JAVASCRIPT"); break;
        case GG_SOURCE     :  print_string (stream, level, "SOURCE");    break;
        case GG_TEMPLATE   :  print_string (stream, level, "TEMPLATE");  break;
        case GG_END_TEMPLATE :
                              print_string (stream, level, "END_TEMPLATE");
                                                                         break;
        case GG_ECHO       :  print_string (stream, level, "ECHO");      break;
        case GG_DEFINE     :  print_string (stream, level, "DEFINE");    break;
        case GG_MACRO      :  print_string (stream, level, "MACRO");     break;
        case GG_FUNCTION   :  print_string (stream, level, "FUNCTION");  break;
        case GG_RETURN     :  print_string (stream, level, "RETURN");    break;
        case GG_SAVE       :  print_string (stream, level, "SAVE");      break;
        case GG_NEW        :  print_string (stream, level, "NEW");       break;
        case GG_DELETE     :  print_string (stream, level, "DELETE");    break;
        case GG_MOVE       :  print_string (stream, level, "MOVE");    break;
        case GG_COPY       :  print_string (stream, level, "COPY");      break;
        case GG_IF         :  print_string (stream, level, "IF");        break;
        case GG_ELSIF      :  print_string (stream, level, "ELSIF");     break;
        case GG_FOR        :  print_string (stream, level, "FOR");       break;
        case GG_WHILE      :  print_string (stream, level, "WHILE");     break;
        case GG_NEXT       :  print_string (stream, level, "NEXT");      break;
        case GG_LAST       :  print_string (stream, level, "LAST");      break;
        case GG_ABORT      :  print_string (stream, level, "ABORT");     break;
        case GG_SORT       :  print_string (stream, level, "SORT");      break;
        case GG_UNDEFINED  :  print_string (stream, level, "UNDEFINED"); break;
      }
}


void print_string (FILE *stream, int level, char *text)
{
    print_indented (stream, level, "%s\n", text);
}


void print_indented (FILE *stream, int level, char *format, ...)
{
    va_list argptr;                     /*  Argument list pointer            */

    fprintf (stream, "%*s", level * 4, "");
    va_start (argptr, format);          /*  Start variable arguments list    */
    vfprintf (stream, format, argptr);
    va_end   (argptr);                  /*  End variable arguments list      */
}


char *
operator_text (OPERATOR op)
{
    switch (op)
      {
        case OP_TIMES              : return "*";
        case OP_DIVIDE             : return "/";
        case OP_PLUS               : return "+";
        case OP_MINUS              : return "-";
        case OP_EQUALS             : return "=";
        case OP_NOT_EQUALS         : return "<>";
        case OP_GREATER_THAN       : return ">";
        case OP_LESS_THAN          : return "<";
        case OP_GREATER_EQUAL      : return ">=";
        case OP_LESS_EQUAL         : return "<=";
        case OP_SAFE_EQUALS        : return "?=";
        case OP_SAFE_NOT_EQUALS    : return "?<>";
        case OP_SAFE_GREATER_THAN  : return "?>";
        case OP_SAFE_LESS_THAN     : return "?<";
        case OP_SAFE_GREATER_EQUAL : return "?>=";
        case OP_SAFE_LESS_EQUAL    : return "?<=";
        case OP_OR                 : return "|";
        case OP_AND                : return "&";
        case OP_NOT                : return "!";
        case OP_DEFAULT            : return "?";
        case OP_NEXT_ARG           : return ",";
        default                    : return "";
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: gg_error

    Synopsis: Returns a pointer to the last error message generated during 
    a call to gg_parse_template, gg_parse_gsl or gg_parse_expression.
    ---------------------------------------------------------------------[>]-*/

char *gg_error (void)
{
    return error_message;
}


/*  ---------------------------------------------------------------------[<]-
    Function: gg_parse_template

    Synopsis: Parses the supplied template line into a script parse tree.
    If an error occurs, returns NULL and generates an error message which
    may be read with gg_error.
    ---------------------------------------------------------------------[>]-*/

SCRIPT_NODE *
gg_parse_template (SCRIPT_READ *read)
{
    start_event = template_event;
    script_read = read;
    script_line = buffer;
    return gg_start_parsing ();
}


/*  ---------------------------------------------------------------------[<]-
    Function: gg_parse_gsl

    Synopsis: Parses the supplied GSL line into a script parse tree.  If
    an error occurs, returns NULL and generates an error message which may
    be read with gg_error.
    ---------------------------------------------------------------------[>]-*/

SCRIPT_NODE *
gg_parse_gsl (SCRIPT_READ *read)
{
    start_event = gsl_event;
    script_read = read;
    script_line = buffer;
    return gg_start_parsing ();
}


/*  ---------------------------------------------------------------------[<]-
    Function: gg_parse_expression

    Synopsis: Parses the supplied GSL expression into a script parse tree.
    If an error occurs, returns NULL and generates an error message which
    may be read with gg_error.
    ---------------------------------------------------------------------[>]-*/

SCRIPT_NODE *
gg_parse_expression (char *line)
{
    start_event = expression_event;
    script_read = NULL;
    script_line = line;
    script_ptr  = 0;                    /*  Start of line                    */
    return gg_start_parsing ();
}


SCRIPT_NODE *
gg_start_parsing (void)
{
    feedback = NULL;

#   include "ggpars.i"                  /*  Do dialog manager                */
}


/*************************   INITIALISE THE PROGRAM   ************************/

MODULE initialise_the_program (void)
{
    brackets        = 0;
    deferred_length = 0;
    deferred_spaces = 0;

    transaction = mem_new_trans ();

    the_root     = NULL;
    the_parent   = NULL;
    the_node_ptr = & the_root;

    list_reset (& state_stack);
    list_reset (& quote_stack);

    the_next_event = start_event;
}


/****************************   READ SCRIPT LINE   ***************************/

MODULE read_script_line (void)
{
    script_ptr = 0;                     /*  Start of line                    */
    if (script_read)
        if ((script_read) (script_line))
            return;

    prev_posn       = 0;
    token_posn      = 0;
    token_length    = 0;
    token_spaces    = 0;
    script_line [0] = '\0';
    raise_exception (end_of_file_event);
}


/***********************   GENERATE SCRIPT EXCEPTION   ***********************/

MODULE generate_script_exception (void)
{
    prev_posn = script_ptr;
    the_token = script_line [script_ptr];

    if (the_token == '.')
      {
        raise_exception (script_event);
        token_posn   = script_ptr++;
        token_length = 1;
      }
}


/***********************   GENERATE COMMENT EXCEPTION   **********************/

MODULE generate_comment_exception (void)
{
    prev_posn = script_ptr;
    the_token = script_line [script_ptr];

    if (the_token == '-')
      {
        raise_exception (comment_event);
        token_posn   = script_ptr++;
        token_length = 1;
      }
}


/************************   GENERATE TEXT EXCEPTION   ************************/

MODULE generate_text_exception (void)
{
    prev_posn = script_ptr;

    collect_spaces ();
    the_token = script_line [script_ptr];

    if (the_token == '>')
      {
        raise_exception (text_event);
        token_posn   = script_ptr++;
        token_length = 1;
      }
    else
        script_ptr = prev_posn;
}


/***************************   GET TEMPLATE TOKEN   **************************/

MODULE get_template_token (void)
{
    prev_posn = script_ptr;
    collect_spaces ();

    the_token      = script_line [script_ptr];
    the_next_event = _LR_NULL_EVENT;

    generate_text_event ('\0');
    generate_simple_token_event ("$(", substitute_event);
    generate_extend_event ();
    generate_end_of_line_event ();

    generate_other_event ();
}


void collect_spaces (void)
{
    token_spaces = 0;
    while (script_line [script_ptr] == ' ')
      {
        script_ptr++;
        token_spaces++;
      }
}


void generate_text_event (char quote)
{
    if (the_next_event == _LR_NULL_EVENT)
      {
        token_posn   = script_ptr;
        token_length = 0;

        while ((the_token = script_line [script_ptr++]) != '\0')
            if (the_token == '\\')
                if (script_line [script_ptr] == '\0')       /*  Extend?  */
                    break;
                else
                  {
                    script_ptr++;
                    token_length += 2;
                  }
            else
                if (((the_token == '$')  /*  Substitute?  */
                &&   (script_line [script_ptr] == '('))
                ||   (the_token == quote)
                ||   (the_token == ' '))
                    break;
                else
                    token_length++;
        
        script_ptr--;               /*  Back to point at terminator      */
        if (token_length > 0)
            the_next_event = text_event;
      }
}


void generate_extend_event (void)
{
    if ((the_next_event == _LR_NULL_EVENT)
    &&  (the_token      == '\\')
    &&  (script_line [script_ptr + 1] == '\0'))
      {
        the_next_event = extend_event;
        token_posn     = script_ptr++;
        token_length   = 1;
      }
}


void generate_end_of_line_event (void)
{
    if ((the_next_event == _LR_NULL_EVENT)
    &&  (the_token      == '\0'))
        the_next_event = end_of_line_event;
}


void generate_other_event (void)
{
    if (the_next_event == _LR_NULL_EVENT)
      {
        the_next_event = other_event;
        token_posn   = script_ptr;
        token_length = 0;
      }
}


/************************   GET SCRIPT COMMAND TOKEN   ***********************/

MODULE get_script_command_token (void)
{
    prev_posn = script_ptr;
    collect_spaces ();

    the_token      = script_line [script_ptr];
    the_next_event = _LR_NULL_EVENT;

    generate_simple_token_event ("#", comment_event);
    generate_simple_token_event ("[", open_bracket_event);
    generate_simple_token_event (".", point_event);
    generate_simple_token_event ("->", member_event);
    generate_simple_token_event ("/*", open_comment_event);
    generate_sign_event ();
    generate_number_event ();
    generate_simple_token_event ("$(", substitute_event);
    generate_extend_event ();
    generate_end_of_line_event ();
    generate_literal_event ();
    recognise_script_command_token ();

    generate_other_event ();
}


void generate_simple_token_event (const char *text,
                                  event_t event)
{
    if ((the_next_event == _LR_NULL_EVENT)
    &&  (strncmp (script_line + script_ptr, text, strlen (text)) == 0))
      {
        the_next_event = event;
        token_posn     = script_ptr;
        token_length   = strlen (text);
        script_ptr    += token_length;
      }
}


void generate_sign_event (void)
{
    if (the_next_event == _LR_NULL_EVENT)
      {
        if (the_token == '+')
            the_operator = OP_PLUS;
        else
        if (the_token == '-')
            the_operator = OP_MINUS;
        else
            return;

        token_posn     = script_ptr++;
        token_length   = 1;
        the_next_event = sign_event;
      }
}


void generate_number_event (void)
{
    char
        *ptr;

    if (the_next_event == _LR_NULL_EVENT)
      {
        the_number   = strtol (& script_line [script_ptr], & ptr, 10);
        token_length = ptr - & script_line [script_ptr];
        if (token_length > 0)
          {
            token_posn     = script_ptr;
            script_ptr    += token_length;
            the_next_event = number_event;
          }
      }
}


void generate_literal_event (void)
{
    if (the_next_event == _LR_NULL_EVENT)
      {
        token_posn   = script_ptr;
        token_length = 0;

        while ((the_token = script_line [script_ptr++]) != '\0')
            if (the_token == '\\')      /*  Ignore next char unless EOL      */
                if (script_line [script_ptr] == '\0')
                    break;
                else
                  {
                    script_ptr++;
                    token_length += 2;
                  }
            else
                if (((the_token >= 'a') && (the_token <= 'z'))
                ||  ((the_token >= 'A') && (the_token <= 'Z'))
                ||  ((the_token >= '0') && (the_token <= '9'))
                ||   (the_token == '_'))
                    token_length++;
                else
                    break;
        
        script_ptr--;                   /*  Back to point at terminator      */
        if (token_length > 0)
            the_next_event = literal_event;
      }
}


void recognise_script_command_token (void)
{
    event_t
        the_event;

    TOKEN_EVENT token_events [] = {
      { "abort",       abort_event },
      { "append",      append_event },
      { "close",       close_event },
      { "copy",        copy_event },
      { "define",      define_event },
      { "delete",      delete_event },
      { "echo",        echo_event },
      { "else",        else_event },
      { "elsif",       elsif_event },
      { "endfor",      end_for_event },
      { "endfunction", end_function_event },
      { "endif",       end_if_event },
      { "endmacro",    end_macro_event },
      { "endnew",      end_new_event },
      { "endtemplate", end_template_event },
      { "endwhile",    end_while_event },
      { "for",         for_event },
      { "function",    function_event },
      { "gsl",         gsl_event },
      { "if",          if_event },
      { "include",     include_event },
      { "invoke",      invoke_event },
      { "javascript",  javascript_event },
      { "last",        last_event },
      { "literal",     direct_event },
      { "load",        load_event },
      { "macro",       macro_event },
      { "move",        move_event },
      { "new",         new_event },
      { "next",        next_event },
      { "output",      output_event },
      { "return",      return_event },
      { "save",        save_event },
      { "sort",        sort_event },
      { "template",    template_event },
      { "while",       while_event },
      { "xml",         xml_event },
    };

    char
        save;

    if (the_next_event == literal_event)
      {
        save = script_line [token_posn + token_length];
        script_line [token_posn + token_length] = 0;
        the_event = recognise_token (& script_line [token_posn],
                                     token_events,
                                     tblsize (token_events));
        script_line [token_posn + token_length] = save;

        if (the_event != _LR_NULL_EVENT)
            the_next_event = the_event;
      }
}


event_t recognise_token (char *token, TOKEN_EVENT token_events [], int num)
{
    int
        min = 0,
        max = num,
        chop = 0,
        cmp = -1;

    while (max > min)
      {
        chop = (max + min) / 2;
        cmp = strcmp (token, token_events [chop]. token);
        if (cmp < 0)
            max = chop;
        else if (cmp > 0)
            min = chop + 1;
        else
            break;
      }

    if (cmp == 0)
        return token_events [chop]. event;
    else
        return _LR_NULL_EVENT;
}


/**************************   GET EXPRESSION TOKEN   *************************/

MODULE get_expression_token (void)
{
    prev_posn      = script_ptr;
    the_token      = script_line [script_ptr];
    the_next_event = _LR_NULL_EVENT;

    generate_spaces_event ();
    generate_simple_token_event ("#", comment_event);
    generate_simple_token_event ("(", open_event);
    generate_simple_token_event (")", close_event);
    generate_simple_token_event ("[", open_bracket_event);
    generate_simple_token_event ("]", close_bracket_event);
    generate_simple_token_event (".", point_event);
    generate_simple_token_event ("->", member_event);
    generate_simple_token_event ("/*", open_comment_event);
    generate_unary_operator_event ();
    generate_sign_event ();
    generate_operator_event ();
    generate_next_arg_event ();
    generate_number_event ();
    generate_simple_token_event ("'", quote_event);
    generate_simple_token_event ("\"", quote_event);
    generate_simple_token_event ("$(", substitute_event);
    generate_simple_token_event ("<<", incoming_event);
    generate_extend_event ();
    generate_end_of_line_event ();
    generate_literal_event ();
    recognise_script_token ();

    generate_other_event ();
}


/****************************   GET SCRIPT TOKEN   ***************************/

MODULE get_script_token (void)
{
    prev_posn      = script_ptr;
    the_token      = script_line [script_ptr];
    the_next_event = _LR_NULL_EVENT;

    generate_spaces_event ();
    generate_simple_token_event ("#", comment_event);
    generate_simple_token_event ("(", open_event);
    generate_simple_token_event (")", close_event);
    generate_simple_token_event ("[", open_bracket_event);
    generate_simple_token_event ("]", close_bracket_event);
    generate_simple_token_event (".", point_event);
    generate_simple_token_event ("->", member_event);
    generate_simple_token_event ("/*", open_comment_event);
    generate_assign_event ();
    generate_sign_event ();
    generate_number_event ();
    generate_simple_token_event ("'", quote_event);
    generate_simple_token_event ("\"", quote_event);
    generate_simple_token_event ("$(", substitute_event);
    generate_simple_token_event ("<<", incoming_event);
    generate_extend_event ();
    generate_end_of_line_event ();
    generate_literal_event ();
    recognise_script_token ();

    generate_other_event ();
}


void generate_spaces_event (void)
{
    if ((the_next_event == _LR_NULL_EVENT)
    &&  (the_token == ' '))
      {
        collect_spaces ();
        token_posn     = script_ptr;
        token_length   = 0;
        the_next_event = spaces_event;
      }
}


void generate_assign_event (void)
{
    OPERATOR
        op;

    if (the_next_event == _LR_NULL_EVENT)
      {
        token_posn   = script_ptr++;
        token_length = 1;
        op = OP_UNDEFINED;

        if (the_token == '=')
            the_next_event = assign_event;
        else
          {
            op = recognise_operator (the_token);

            if ((op != OP_UNDEFINED)
            &&  (op != OP_LESS_THAN)
            &&  (op != OP_GREATER_THAN)
            &&  (op != OP_NOT)
            &&  (script_line [script_ptr] == '='))
              {
                the_next_event = assign_event;
                script_ptr++;
                token_length++;
              }
            else
              {
                op = OP_UNDEFINED;
                script_ptr   = token_posn;
              }
          }
        the_operator = op;
      }           
}


void generate_unary_operator_event (void)
{
    if (the_next_event == _LR_NULL_EVENT)
      {
        if (the_token == '!')
            the_operator = OP_NOT;
        else
            return;

        token_posn     = script_ptr++;
        token_length   = 1;
        the_next_event = unary_operator_event;
      }
}


void generate_operator_event (void)
{
    Bool
        safe = FALSE;
    char
        the_char;
    OPERATOR
        op;

    if (the_next_event == _LR_NULL_EVENT)
      {
        token_posn   = script_ptr++;
        token_length = 1;
        op = OP_UNDEFINED;

        if (the_token == '?')
          {
            the_char = script_line [script_ptr];
            if ((the_char == '<') || (the_char == '>') || (the_char == '='))
              {
                token_length++;
                script_ptr++;
                the_token = the_char;
                safe = TRUE;
              }
          }

        op = recognise_operator (the_token);

        if ((op == OP_DIVIDE)
        &&  (script_line [script_ptr] == '*'))  /*  Open comment  */
            op = OP_UNDEFINED;

        if (op == OP_UNDEFINED)
            script_ptr = token_posn;

        if (op == OP_LESS_THAN)
          {
            if (script_line [script_ptr] == '>')
              {
                op = OP_NOT_EQUALS;
                script_ptr++;
                token_length++;
              }
            else if (script_line [script_ptr] == '=')
              {
                op = OP_LESS_EQUAL;
                script_ptr++;
                token_length++;
              }
          }
        if ((op == OP_GREATER_THAN)
        &&  (script_line [script_ptr] == '='))
          {
            op = OP_GREATER_EQUAL;
            script_ptr++;
            token_length++;
          }
        if (safe)
          {
            switch (op)
              {
                case OP_EQUALS        : op = OP_SAFE_EQUALS;
                                        break;
                case OP_NOT_EQUALS    : op = OP_SAFE_NOT_EQUALS;
                                        break;
                case OP_GREATER_THAN  : op = OP_SAFE_GREATER_THAN;
                                        break;
                case OP_LESS_THAN     : op = OP_SAFE_LESS_THAN;
                                        break;
                case OP_GREATER_EQUAL : op = OP_SAFE_GREATER_EQUAL;
                                        break;
                case OP_LESS_EQUAL    : op = OP_SAFE_LESS_EQUAL;
                                        break;
                default               : op = OP_UNDEFINED;
                                        script_ptr = token_posn;
              }
          }
        if (op == OP_DEFAULT)
            the_next_event = default_operator_event;
        else
        if (op != OP_UNDEFINED)
            the_next_event = operator_event;

        if (op != OP_UNDEFINED)
            the_operator = op;
      }
}


OPERATOR recognise_operator (char ch)
{
    switch (the_token)
      {
        case '*' : return OP_TIMES;
        case '/' : return OP_DIVIDE;
        case '+' : return OP_PLUS;
        case '-' : return OP_MINUS;
        case '=' : return OP_EQUALS;
        case '<' : return OP_LESS_THAN;
        case '>' : return OP_GREATER_THAN;
        case '|' : return OP_OR;
        case '&' : return OP_AND;
        case '!' : return OP_NOT;
        case '?' : return OP_DEFAULT;
        default  : return OP_UNDEFINED;
      }
}


void generate_next_arg_event (void)
{
    if ((the_next_event == _LR_NULL_EVENT)
    &&  (the_token == ','))
      {
        the_next_event = next_arg_event;
        token_posn     = script_ptr++;
        token_length   = 1;
        the_operator   = OP_NEXT_ARG;
      }
}
    

void recognise_script_token (void)
{
    event_t
        the_event;

    TOKEN_EVENT token_events [] = {
      { "after",       after_event },
      { "as",          as_event },
      { "before",      before_event },
      { "by",          by_event },
      { "from",        from_event },
      { "to",          to_event },
      { "where",       where_event },
    };

    char
        save;

    if (the_next_event == literal_event)
      {
        save = script_line [token_posn + token_length];
        script_line [token_posn + token_length] = 0;
        the_event = recognise_token (& script_line [token_posn],
                                     token_events,
                                     tblsize (token_events));
        script_line [token_posn + token_length] = save;

        if (the_event != _LR_NULL_EVENT)
            the_next_event = the_event;
      }
}


/*********************   GET BRACKETED IDENTIFIER TOKEN   ********************/

MODULE get_bracketed_identifier_token (void)
{
    prev_posn      = script_ptr;
    the_token      = script_line [script_ptr];
    the_next_event = _LR_NULL_EVENT;

    generate_extra_literal_event ();
    generate_simple_token_event ("]", close_bracket_event);

    generate_other_event ();
}


void generate_extra_literal_event (void)
{
    if (the_next_event == _LR_NULL_EVENT)
      {
        token_posn   = script_ptr;
        token_length = 0;

        while ((the_token = script_line [script_ptr++]) != '\0')
            if (the_token == '\\')      /*  Ignore next char unless EOL      */
                if (script_line [script_ptr] == '\0')
                    break;
                else
                  {
                    script_ptr++;
                    token_length += 2;
                  }
            else
                if (the_token != ']')
                    token_length++;
                else
                    break;
        
        script_ptr--;                   /*  Back to point at terminator      */
        if (token_length > 0)
            the_next_event = literal_event;
      }
}


/****************************   GET QUOTED TOKEN   ***************************/

MODULE get_quoted_token (void)
{
    char
        the_quote[2] = " ";

    prev_posn = script_ptr;
    collect_spaces ();

    the_token      = script_line [script_ptr];
    the_next_event = _LR_NULL_EVENT;

    generate_simple_token_event ("$(", substitute_event);
    generate_extend_event ();
    generate_end_of_line_event ();
    if (the_next_event == _LR_NULL_EVENT)
      {
        list_pop (&quote_stack, the_quote[0]);
        generate_simple_token_event (the_quote, quote_event);
        generate_text_event (the_quote[0]);
        if (the_next_event != quote_event)
            list_push (&quote_stack, the_quote[0]);
      }
    
    generate_other_event ();
}


/***************************   GET MODIFIER TOKEN   **************************/

MODULE get_modifier_token (void)
{
    prev_posn = script_ptr;
    collect_spaces ();

    the_token      = script_line [script_ptr];
    the_next_event = _LR_NULL_EVENT;

    generate_simple_token_event (":", modifier_event);
    generate_simple_token_event ("%", modifier_event);
    generate_simple_token_event (")", close_event);
    generate_simple_token_event ("$(", substitute_event);
    generate_extend_event ();
    generate_modifier_text_event ();

    generate_other_event ();
}


void generate_modifier_text_event (void)
{
    if (the_next_event == _LR_NULL_EVENT)
      {
        token_posn   = script_ptr;
        token_length = 0;

        while ((the_token = script_line [script_ptr++]) != '\0')
            if ((the_token == ':')              /*  Modifer?  */
            ||  (the_token == '%')
            ||  (the_token == ')'))             /*  Close?  */
                break;
            else
            if (the_token == '\\')
                if (script_line [script_ptr] == '\0')       /*  Extend?  */
                    break;
                else
                  {
                    script_ptr++;
                    token_length += 2;
                  }
            else
                if (((the_token == '$')  /*  Substitute?  */
                &&   (script_line [script_ptr] == '('))
                ||   (the_token == ' '))
                    break;
                else
                    token_length++;
        
        script_ptr--;               /*  Back to point at terminator      */
        if (token_length > 0)
            the_next_event = text_event;
      }
}


/***************************   GET COMMENT TOKEN   ***************************/

MODULE get_comment_token (void)
{
    prev_posn = script_ptr;
    collect_spaces ();

    the_token      = script_line [script_ptr];
    the_next_event = _LR_NULL_EVENT;

    generate_simple_token_event ("*/", close_comment_event);
    generate_end_of_line_event ();
    generate_comment_text_event ();
}


void generate_comment_text_event (void)
{
    if (the_next_event == _LR_NULL_EVENT)
      {
        token_posn   = script_ptr;
        token_length = 0;

        while ((the_token = script_line [script_ptr++]) != '\0')
            if ((the_token == '*')  /*  Close?  */
            &&  (script_line [script_ptr] == '/'))
                break;
            else
                token_length++;
        
        script_ptr--;               /*  Back to point at terminator      */
        if (token_length > 0)
            the_next_event = text_event;
      }
}


/***************************   EXPECT END OF LINE  ***************************/

MODULE expect_end_of_line (void)
{
    collect_spaces ();

    the_token = script_line [script_ptr];
    if (the_token != '\0')
        raise_exception (other_event);
}


/***************************   POP PREVIOUS STATE   **************************/

MODULE pop_previous_state (void)
{
    STATE
        state;

    if (list_empty (& state_stack))
      {
        the_next_event = empty_stack_event;
        return;
      }

    list_pop (&state_stack, state);
    the_next_event = state_event [state];
}


/**************************   PUSH QUOTE CHARACTER   *************************/

MODULE push_quote_character (void)
{
    list_push (&quote_stack, the_token);
}


/*************************   INSERT EMPTY TEXT NODE   ************************/

MODULE insert_empty_text_node (void)
{
    insert_simple_node (GG_TEXT);
}


/****************************   INSERT TEXT NODE   ***************************/

MODULE insert_text_node (void)
{
    char
        save;

    insert_simple_node (GG_TEXT);
    /*  After calling insert_simple_node, the_parent points to the new node  */

    /*  Convoluted way of extracting string straight from source line  */
    save = script_line [token_posn + token_length];
    script_line [token_posn + token_length] = 0;
    if (*the_node_ptr)
        (*the_node_ptr)-> text  = memt_strdup (transaction, & script_line [token_posn]);
    else
        the_parent-> text  = memt_strdup (transaction, & script_line [token_posn]);
    script_line [token_posn + token_length] = save;
}


void insert_simple_node (SCRIPT_NODE_TYPE type)
{
    SCRIPT_NODE
        *new_leaf;

    new_leaf = memt_alloc (transaction, sizeof (*new_leaf));
    ASSERT (new_leaf);
    init_script_node (*new_leaf);

    new_leaf-> type     = type;
    new_leaf-> brackets = brackets;

    brackets = 0;

    insert_the_node (new_leaf, OP_UNDEFINED);
    (*the_node_ptr)-> brackets += brackets;
    brackets = 0;
}


/*  insert_the_node - does the tree-building work.  the_node_ptr is a        */
/*  pointer to a pointer (usually in another node, but may be the global     */
/*  variable the_root.  The value of *the_node_ptr may either be NULL, in    */
/*  which case the node *app_node is inserted, or non-NULL, in which case    */
/*  an operator node is inserted between **the_node_ptr and *app_node,       */
/*  which may by NULL.  If the_node_ptr is NULL then there is no current     */
/*  insertion point indicating an internal error.                            */

void insert_the_node (SCRIPT_NODE *app_node, OPERATOR operator)
{
    SCRIPT_NODE
        *new_node;

    ASSERT (the_node_ptr);
    if ((operator      != OP_UNDEFINED)
    ||  (*the_node_ptr != NULL))
      {
/*          if (*the_node_ptr) JS 1999/10/08 */
            find_operator_insertion_point (operator);
        
        new_node = memt_alloc (transaction, sizeof (*new_node));
        ASSERT (new_node);
        init_script_node (*new_node);
        
        new_node-> type     = GG_OPERATOR;
        new_node-> operator = operator;
        new_node-> op1      = *the_node_ptr;
        new_node-> op2      = app_node;
        new_node-> parent   = the_parent;
        new_node-> length   = deferred_length;
        new_node-> spaces   = deferred_spaces;
        deferred_length = 0;
        deferred_spaces = 0;
        
        /*  Migrate brackets, spaces and length to highest level  */
        if (*the_node_ptr)
          {
            new_node-> spaces  += (*the_node_ptr)-> spaces;
            new_node-> length  += (*the_node_ptr)-> length;
            new_node-> brackets = (*the_node_ptr)-> brackets;
            (*the_node_ptr)-> spaces   = 0;
            (*the_node_ptr)-> brackets = 0;
            (*the_node_ptr)-> parent   = new_node;
          }

        *the_node_ptr = new_node;

        if (app_node)
          {
            app_node -> parent = new_node;

            the_node_ptr = &new_node-> op2;
            the_parent   = new_node;
          }
      }
    else
      {
        ASSERT (app_node);
        app_node-> parent = the_parent;
        app_node-> length += deferred_length;
        app_node-> spaces += deferred_spaces;
        deferred_length = 0;
        deferred_spaces = 0;

        *the_node_ptr     = app_node;
      }
}


/*  This function finds the tree node to insert a new operator.             */
void find_operator_insertion_point (OPERATOR operator)
{
    while ( (the_parent)
             ? (   (*the_node_ptr ? (*the_node_ptr)-> brackets == 0 : TRUE)
                && (the_parent-> type == GG_OPERATOR)
                && ( (priority [the_parent-> operator] >= priority [operator])
                || (!the_parent-> op1) ) )   /*  Skip unitary operator  */
             : FALSE )
        if (return_one_level ())
            return;
}


int return_one_level (void)
{
    ASSERT (the_parent);

    if (brackets > 0)
      {
        error_exception ("Unexpected token.");
        return -1;
      }
    if (*the_node_ptr ? (*the_node_ptr)-> brackets > 0 : FALSE)
      {
        error_exception ("Mismatched brackets.");
        return -1;
      }

    /*  Length and spaces of child is added to length of parent  */
    if (*the_node_ptr)
        the_parent-> length += (*the_node_ptr)-> spaces
                             + (*the_node_ptr)-> length;

    /*  And deferred length & spaces  */
    the_parent-> length += deferred_spaces
                         + deferred_length;
    deferred_spaces = 0;
    deferred_length = 0;

    /*  Find out which child the current parent is */
    if (the_parent-> parent == NULL)
        the_node_ptr = & the_root;
    else
    if (the_parent == the_parent-> parent-> op1)
        the_node_ptr = & the_parent-> parent-> op1;
    else
    if (the_parent == the_parent-> parent-> op2)
        the_node_ptr = & the_parent-> parent-> op2;
    else
    if (the_parent == the_parent-> parent-> op3)
        the_node_ptr = & the_parent-> parent-> op3;
    else
    if (the_parent == the_parent-> parent-> pretty)
        the_node_ptr = & the_parent-> parent-> pretty;
    else
    if (the_parent == the_parent-> parent-> format)
        the_node_ptr = & the_parent-> parent-> format;
    else
      {
        raise_exception (anomaly_event);
        return -1;
      }
    the_parent = the_parent-> parent;
/*     print_node_type (stdout, 0, (*the_node_ptr)-> type); */

    return 0;
}


/*************************   INSERT SUBSTITUTE NODE   ************************/

MODULE insert_substitute_node (void)
{
    insert_simple_node (GG_SUBSTITUTE);
}


/**************************   READY FOR OP1 FIELD   **************************/

MODULE ready_for_op1_field (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    the_parent   =    *the_node_ptr;
    the_node_ptr = & (*the_node_ptr)-> op1;
}


/**************************   READY FOR OP2 FIELD   **************************/

MODULE ready_for_op2_field (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    the_parent   =    *the_node_ptr;
    the_node_ptr = & (*the_node_ptr)-> op2;
}


/**************************   READY FOR OP3 FIELD   **************************/

MODULE ready_for_op3_field (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    the_parent   =    *the_node_ptr;
    the_node_ptr = & (*the_node_ptr)-> op3;
}


/*************************   READY FOR PRETTY FIELD   ************************/

MODULE ready_for_pretty_field (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    the_parent   =    *the_node_ptr;
    the_node_ptr = & (*the_node_ptr)-> pretty;
}


/*************************   READY FOR FORMAT FIELD   ************************/

MODULE ready_for_format_field (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    the_parent   =    *the_node_ptr;
    the_node_ptr = & (*the_node_ptr)-> format;
}


/****************************   MOVE OP1 TO OP2   ****************************/

MODULE move_op1_to_op2 (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    (*the_node_ptr)-> op2 = (*the_node_ptr)-> op1;
    (*the_node_ptr)-> op1 = NULL;
}


/*****************************   SAVE THE SIGN   *****************************/

MODULE save_the_sign (void)
{
    the_sign = the_operator;
}


/****************************   RESTORE THE SIGN   ***************************/

MODULE restore_the_sign (void)
{
    the_operator = the_sign;
}


/***************************   COPY THE OPERATOR   ***************************/

MODULE copy_the_operator (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    (*the_node_ptr)-> operator = the_operator;
}


/***********************   READY FOR MODIFIER FIELD   ************************/

MODULE ready_for_modifier_field (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    the_parent = *the_node_ptr;

    if (the_token == ':')
        the_node_ptr = & (*the_node_ptr)-> pretty;
    else
    if (the_token == '%')
        the_node_ptr = & (*the_node_ptr)-> format;
    else
      {
        raise_exception (anomaly_event);
        return;
      }

    if (*the_node_ptr != NULL)
        error_exception ("Duplicate modifier");
}


/****************************   SET EXTEND FLAG   ****************************/

MODULE set_extend_flag (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    (*the_node_ptr)-> extend = TRUE;
}


/**************************   SET LINE BREAK FLAG   **************************/

MODULE set_line_break_flag (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    (*the_node_ptr)-> line_break = TRUE;
}


/************************   INSERT SPACES ONLY NODE   ************************/

MODULE insert_spaces_only_node (void)
{
    if ((token_spaces > 0)
    ||  (*the_node_ptr == NULL))
      {
        insert_simple_node (GG_TEXT);
        (*the_node_ptr)-> spaces += token_spaces;
        token_spaces = 0;
      }
}


/**************************   RETURN TO ROOT NODE   **************************/

MODULE return_to_root_node (void)
{
    while (the_parent)
        if (return_one_level ())
            return;

/*     if (*the_node_ptr) */
/*       gg_print (stdout, *the_node_ptr); */
/*     else */
/*       printf ("<NULL>\n"); */
}


/***************************   RETURN TO OPERAND   ***************************/

MODULE return_to_operand (void)
{
    while (the_parent)
        if ((the_parent-> type     == GG_OPERATOR)
        &&  (the_parent-> operator == OP_UNDEFINED))
          {
            if (return_one_level ())
                return;
          }
        else
            break;

/*     if (*the_node_ptr) */
/*       gg_print (stdout, *the_node_ptr); */
/*     else */
/*       printf ("<NULL>\n"); */
}


/***************************   RETURN TO OPERATOR   **************************/

MODULE return_to_operator (void)
{
    if (the_parent)
        if ((the_parent-> type     == GG_LITERAL)
        ||  (the_parent-> type     == GG_NUMBER)
        ||  (the_parent-> type     == GG_OPERAND)
        ||  (the_parent-> type     == GG_SYMBOL))
            if (return_one_level ())
                return;

/*     if (*the_node_ptr) */
/*       gg_print (stdout, *the_node_ptr); */
/*     else */
/*       printf ("<NULL>\n"); */
}


/****************************   RETURN ONE MORE   ****************************/

MODULE return_one_more (void)
{
    if (the_parent)
        return_one_level ();
}


/****************************   RETURN TO SCOPE   ****************************/

MODULE return_to_scope (void)
{
    while (the_parent)
        if (the_parent-> type == GG_MEMBER)
          {
            if (return_one_level ())
                return;
          }
        else
            break;

/*     if (*the_node_ptr) */
/*       gg_print (stdout, *the_node_ptr); */
/*     else */
/*       printf ("<NULL>\n"); */
}


/**************************   RETURN TO ARGUMENTS   **************************/

MODULE return_to_arguments (void)
{
    while (the_parent)
        if ((the_parent-> type     == GG_OPERATOR)
        &&  (the_parent-> operator == OP_NEXT_ARG))
          {
            if (return_one_level ())
                return;
          }
        else
            break;

/*     if (*the_node_ptr) */
/*       gg_print (stdout, *the_node_ptr); */
/*     else */
/*       printf ("<NULL>\n"); */
}


/**************************   RETURN TO EXPRESSION   *************************/

MODULE return_to_expression (void)
{
    while (the_parent)
        if ((the_parent-> type     == GG_OPERATOR)
        &&  (the_parent-> operator != OP_NEXT_ARG))
          {
            if (return_one_level ())
                return;
          }
        else
            break;

/*     if (*the_node_ptr) */
/*       gg_print (stdout, *the_node_ptr); */
/*     else */
/*       printf ("<NULL>\n"); */
}


/***********************   COMMIT MEMORY ALLOCATIONS   ***********************/

MODULE commit_memory_allocations (void)
{
    mem_commit (transaction);
    feedback = the_root;
}


/**************************   INSERT LITERAL NODE   **************************/

MODULE insert_literal_node (void)
{
    insert_simple_node (GG_LITERAL);
}


/***************************   INSERT NUMBER NODE   **************************/

MODULE insert_number_node (void)
{
    insert_simple_node (GG_NUMBER);
}


/***************************   COUNT TOKEN LENGTH   **************************/

MODULE count_token_length (void)
{
    ASSERT (the_node_ptr);

    if (*the_node_ptr)
      {
        (*the_node_ptr)-> length += token_length;
        (*the_node_ptr)-> spaces += token_spaces;
      }
    else
      {
        deferred_length += token_length;
        deferred_spaces += token_spaces;
      }
    token_spaces = 0;
}


/*********************   COUNT SPACES AND TOKEN LENGTH   *********************/

MODULE count_spaces_and_token_length (void)
{
    ASSERT (the_node_ptr);

    if (*the_node_ptr)
        (*the_node_ptr)-> length += token_length + token_spaces;
    else
        deferred_length += token_length + token_spaces;

    token_spaces = 0;
}


/***************************   INSERT SYMBOL NODE   **************************/

MODULE insert_symbol_node (void)
{
    insert_simple_node (GG_SYMBOL);
}


/***************************   INSERT MEMBER NODE   **************************/

MODULE insert_member_node (void)
{
    SCRIPT_NODE
        *new_node;

    new_node = memt_alloc (transaction, sizeof (*new_node));
    ASSERT (new_node);
    init_script_node (*new_node);
        
    new_node-> type    = GG_MEMBER;
    new_node-> op1     = *the_node_ptr;
    new_node-> parent  = the_parent;

    if (*the_node_ptr)
      {
        new_node-> spaces   = (*the_node_ptr)-> spaces;
        new_node-> length   = (*the_node_ptr)-> length;
        new_node-> brackets = (*the_node_ptr)-> brackets;
        (*the_node_ptr)-> brackets = 0;
        (*the_node_ptr)-> parent = new_node;
      }

    *the_node_ptr = new_node;
}


/**********************   INSERT UNKNOWN OPERAND NODE   **********************/

MODULE insert_unknown_operand_node (void)
{
    insert_simple_node (GG_OPERAND);
}


/****************************   ADD ONE BRACKET   ****************************/

MODULE add_one_bracket (void)
{
    brackets++;
}


/**************************   REWIND TO SAME TOKEN   *************************/

MODULE rewind_to_same_token (void)
{
    script_ptr = prev_posn;
}


/*********************   CHANGE OPERAND TO SYMBOL NODE   *********************/

MODULE change_operand_to_symbol_node (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    if (((*the_node_ptr)-> type == GG_OPERAND)
    ||  ((*the_node_ptr)-> type == GG_NUMBER))
      {
        (*the_node_ptr)-> type = GG_SYMBOL;

        /*  If there is a sign, it belongs to the scope  */
        (*the_node_ptr)-> op1-> operator = (*the_node_ptr)-> operator;
        (*the_node_ptr)-> operator = OP_UNDEFINED;
      }
}


/**************************   CHANGE TO CALL NODE   **************************/

MODULE change_to_call_node (void)
{
    ASSERT (the_node_ptr);
    ASSERT (*the_node_ptr);

    if ((*the_node_ptr)-> op1  == NULL)
        (*the_node_ptr)-> type = GG_CALL;
    else
        error_exception ("Unexpected token.");
}


/***********************   INSERT UNARY OPERATOR NODE   **********************/

MODULE insert_unary_operator_node (void)
{
    insert_simple_node (GG_OPERATOR);
    
    (*the_node_ptr)-> operator = the_operator;
}


/**************************   INSERT OPERATOR NODE   *************************/

MODULE insert_operator_node (void)
{
    insert_the_node (NULL, the_operator);

    (*the_node_ptr)-> brackets += brackets;
    brackets = 0;
}


/***************************   CLOSE ONE BRACKET   ***************************/

MODULE close_one_bracket (void)
{
    ASSERT (the_node_ptr);

    if (*the_node_ptr == NULL)
        if (brackets > 0)
            error_exception ("Unexpected token.");
        else
            raise_exception (no_bracket_event);
    else
      {
        while ( (the_parent)
                 ? (((*the_node_ptr)-> brackets == 0)
                   && (the_parent-> type     == GG_OPERATOR)
                   && (the_parent-> operator != OP_NEXT_ARG))
                 : FALSE )
            if (return_one_level ())
                return;

        if ((*the_node_ptr)-> brackets > 0)
            (*the_node_ptr)-> brackets--;
        else
            raise_exception (no_bracket_event);
      }
}


/***********************   ERROR IF EMPTY EXPRESSION   ***********************/

MODULE error_if_empty_expression (void)
{
    if (! the_root)
        error_exception ("Unexpected token.");
}


/**********************   INSERT EMPTY NODE IF NEEDED   **********************/

MODULE insert_empty_node_if_needed (void)
{
    if (the_node_ptr)
        if (!*the_node_ptr)
          {
            insert_simple_node (GG_TEXT);
            (*the_node_ptr)-> text = memt_strdup (transaction, "");
          }
}


/**********************   CONFIRM MODIFIER SPECIFIED   ***********************/

MODULE confirm_modifier_specified (void)
{
    if (!the_node_ptr)
        error_exception ("Unexpected token.");
}


/**************************   INSERT COMMENT NODE   **************************/

MODULE insert_comment_node (void)
{
    insert_simple_node (GG_COMMENT);
}


/***************************   INSERT OUTPUT NODE   **************************/

MODULE insert_output_node (void)
{
    insert_simple_node (GG_OUTPUT);
}


/***************************   INSERT APPEND NODE   **************************/

MODULE insert_append_node (void)
{
    insert_simple_node (GG_APPEND);
}


/***************************   INSERT CLOSE NODE   ***************************/

MODULE insert_close_node (void)
{
    insert_simple_node (GG_CLOSE);
}


/*****************************   INSERT IF NODE   ****************************/

MODULE insert_if_node (void)
{
    insert_simple_node (GG_IF);
}


/***************************   INSERT ELSIF NODE   ***************************/

MODULE insert_elsif_node (void)
{
    insert_simple_node (GG_ELSIF);
}


/****************************   INSERT ELSE NODE   ***************************/

MODULE insert_else_node (void)
{
    insert_simple_node (GG_ELSE);
}


/***************************   INSERT END IF NODE   **************************/

MODULE insert_end_if_node (void)
{
    insert_simple_node (GG_END_IF);
}



/****************************   INSERT FOR NODE   ****************************/

MODULE insert_for_node (void)
{
    insert_simple_node (GG_FOR);
}


/**************************   INSERT END FOR NODE   **************************/

MODULE insert_end_for_node (void)
{
    insert_simple_node (GG_END_FOR);
}


/****************************   INSERT NEW NODE   ****************************/

MODULE insert_new_node (void)
{
    insert_simple_node (GG_NEW);
}


/**************************   INSERT END NEW NODE   **************************/

MODULE insert_end_new_node (void)
{
    insert_simple_node (GG_END_NEW);
}


/***************************   INSERT WHILE NODE   ***************************/

MODULE insert_while_node (void)
{
    insert_simple_node (GG_WHILE);
}


/*************************   INSERT END WHILE NODE   *************************/

MODULE insert_end_while_node (void)
{
    insert_simple_node (GG_END_WHILE);
}


/****************************   INSERT NEXT NODE   ***************************/

MODULE insert_next_node (void)
{
    insert_simple_node (GG_NEXT);
}


/****************************   INSERT LAST NODE   ***************************/

MODULE insert_last_node (void)
{
    insert_simple_node (GG_LAST);
}


/***************************   INSERT MACRO NODE   ***************************/

MODULE insert_macro_node (void)
{
    insert_simple_node (GG_MACRO);
}


/*************************   INSERT END MACRO NODE   *************************/

MODULE insert_end_macro_node (void)
{
    insert_simple_node (GG_END_MACRO);
}


/**************************   INSERT FUNCTION NODE   *************************/

MODULE insert_function_node (void)
{
    insert_simple_node (GG_FUNCTION);
}


/************************   INSERT END FUNCTION NODE   ***********************/

MODULE insert_end_function_node (void)
{
    insert_simple_node (GG_END_FUNCTION);
}


/***************************   INSERT RETURN NODE   **************************/

MODULE insert_return_node (void)
{
    insert_simple_node (GG_RETURN);
}


/****************************   INSERT CALL NODE   ***************************/

MODULE insert_call_node (void)
{
    insert_simple_node (GG_CALL);
}


/****************************   INSERT GSL NODE   ****************************/

MODULE insert_gsl_node (void)
{
    insert_simple_node (GG_GSL);
}


/***************************   INSERT DIRECT NODE   **************************/

MODULE insert_direct_node (void)
{
    insert_simple_node (GG_DIRECT);
}


/***************************   INSERT SOURCE NODE   **************************/

MODULE insert_source_node (void)
{
    insert_simple_node (GG_SOURCE);
}


/****************************   INSERT XML NODE   ****************************/

MODULE insert_xml_node (void)
{
    insert_simple_node (GG_XML);
}


/*************************   INSERT JAVASCRIPT NODE   ************************/

MODULE insert_javascript_node (void)
{
    insert_simple_node (GG_JAVASCRIPT);
}


/**************************   INSERT TEMPLATE NODE   *************************/

MODULE insert_template_node (void)
{
    insert_simple_node (GG_TEMPLATE);
}


/************************   INSERT END TEMPLATE NODE   ***********************/

MODULE insert_end_template_node (void)
{
    insert_simple_node (GG_END_TEMPLATE);
}


/****************************   INSERT ECHO NODE   ***************************/

MODULE insert_echo_node (void)
{
    insert_simple_node (GG_ECHO);
}


/***************************   INSERT ABORT NODE   ***************************/

MODULE insert_abort_node (void)
{
    insert_simple_node (GG_ABORT);
}


/***************************   INSERT DEFINE NODE   **************************/

MODULE insert_define_node (void)
{
    insert_simple_node (GG_DEFINE);
}


/****************************   INSERT MOVE NODE   ***************************/

MODULE insert_move_node (void)
{
    insert_simple_node (GG_MOVE);
}


/****************************   INSERT COPY NODE   ***************************/

MODULE insert_copy_node (void)
{
    insert_simple_node (GG_COPY);
}


/***************************   INSERT DELETE NODE   **************************/

MODULE insert_delete_node (void)
{
    insert_simple_node (GG_DELETE);
}


/****************************   INSERT SAVE NODE   ***************************/

MODULE insert_save_node (void)
{
    insert_simple_node (GG_SAVE);
}


/****************************   INSERT SORT NODE   ***************************/

MODULE insert_sort_node (void)
{
    insert_simple_node (GG_SORT);
}


/*************************   SIGNAL INTERNAL ERROR   *************************/

MODULE signal_internal_error (void)
{
    sprintf (error_message, "Internal parser error.\n");
}


/**********************   ROLLBACK MEMORY ALLOCATIONS   **********************/

MODULE rollback_memory_allocations (void)
{
    STATE
        state;
    char
        ch;

    mem_rollback (transaction);
    while (!list_empty (& state_stack))
        list_pop (&state_stack, state);

    while (!list_empty (& quote_stack))
        list_pop (&quote_stack, ch);

    feedback = NULL;
}


/*************************   SIGNAL INVALID TOKEN   **************************/

MODULE signal_invalid_token (void)
{
    error_exception ("Unexpected token.");
}


/*********************   SIGNAL UNEXPECTED END OF FILE   *********************/

MODULE signal_unexpected_end_of_file (void)
{
    error_exception ("Unexpected end of file.");
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

/*****************************************************************************/

void
error_exception (char *format, ...)
{
    char
        *mess_ptr;
    int
        offset;
    va_list
        argptr;

    mess_ptr = error_message; 
    if (*script_line)
      {
        offset = sprintf (mess_ptr, "%s\n",  script_line);
        mess_ptr += offset;
        offset = sprintf (mess_ptr, "%*s\n", token_posn + 1, "^");
        mess_ptr += offset;
      }
    va_start (argptr, format);          /*  Start variable arguments list    */
    offset = vsprintf (mess_ptr, format, argptr);
    va_end   (argptr);                  /*  End variable arguments list      */
    mess_ptr += offset;

    sprintf (mess_ptr, "\n");
    raise_exception (error_event);
}

/*************************   PUSH EXPRESSION STATE   *************************/

MODULE push_expression_state (void)
{
    STATE
        state = EXPRESSION;

    list_push  (&state_stack, state);
}


/****************************   PUSH TEXT STATE   ****************************/

MODULE push_text_state (void)
{
    STATE
        state = TEXT;

    list_push  (&state_stack, state);
}


/*************************   PUSH SUBSTITUTE STATE   *************************/

MODULE push_substitute_state (void)
{
    STATE
        state = SUBSTITUTE;

    list_push  (&state_stack, state);
}


/***************************   PUSH OPERAND STATE   **************************/

MODULE push_operand_state (void)
{
    STATE
        state = OPERAND;

    list_push  (&state_stack, state);
}


/***********************   PUSH SIMPLE OPERAND STATE   ***********************/

MODULE push_simple_operand_state (void)
{
    STATE
        state = SIMPLE_OPERAND;

    list_push  (&state_stack, state);
}


/********************   PUSH BRACKETED IDENTIFIER STATE   ********************/

MODULE push_bracketed_identifier_state (void)
{
    STATE
        state = BRACKETED_IDENTIFIER;

    list_push  (&state_stack, state);
}


/*********************   PUSH CONTINUE ATTRIBUTE STATE   *********************/

MODULE push_continue_attribute_state (void)
{
    STATE
        state = CONTINUE_ATTRIBUTE;

    list_push  (&state_stack, state);
}


/***********************   PUSH CONTINUE SCOPE STATE   ***********************/

MODULE push_continue_scope_state (void)
{
    STATE
        state = CONTINUE_SCOPE;

    list_push  (&state_stack, state);
}


/*****************   PUSH CONTINUE SCOPE OR ATTRIBUTE STATE   ****************/

MODULE push_continue_scope_or_attribute_state (void)
{
    STATE
        state = CONTINUE_SCOPE_OR_ATTRIBUTE;

    list_push  (&state_stack, state);
}


/***************************   PUSH QUOTED STATE   ***************************/

MODULE push_quoted_state (void)
{
    STATE
        state = QUOTED;

    list_push  (&state_stack, state);
}


/*************************   PUSH IDENTIFIER STATE   *************************/

MODULE push_identifier_state (void)
{
    STATE
        state = IDENTIFIER;

    list_push  (&state_stack, state);
}


/****************************   PUSH CALL STATE   ****************************/

MODULE push_call_state (void)
{
    STATE
        state = CALL;

    list_push  (&state_stack, state);
}


/**************************   PUSH ARGUMENTS STATE   *************************/

MODULE push_arguments_state (void)
{
    STATE
        state = ARGUMENTS;

    list_push  (&state_stack, state);
}


/**************************   PUSH MODIFIER STATE   **************************/

MODULE push_modifier_state (void)
{
    STATE
        state = MODIFIER;

    list_push  (&state_stack, state);
}


/***************************   PUSH DEFINE STATE   ***************************/

MODULE push_define_state (void)
{
    STATE
        state = DEFINE;

    list_push  (&state_stack, state);
}


/****************************   PUSH MACRO STATE   ***************************/

MODULE push_macro_state (void)
{
    STATE
        state = MACRO;

    list_push  (&state_stack, state);
}


/***************************   PUSH INVOKE STATE   ***************************/

MODULE push_invoke_state (void)
{
    STATE
        state = INVOKE;

    list_push  (&state_stack, state);
}


/****************************   PUSH SAVE STATE   ****************************/

MODULE push_save_state (void)
{
    STATE
        state = SAVE;

    list_push  (&state_stack, state);
}


/*****************************   PUSH NEW STATE   ****************************/

MODULE push_new_state (void)
{
    STATE
        state = NEW;

    list_push  (&state_stack, state);
}


/*****************************   PUSH FOR STATE   ****************************/

MODULE push_for_state (void)
{
    STATE
        state = FOR;

    list_push  (&state_stack, state);
}


/******************   PUSH IMPLICIT ASSIGN OR INVOKE STATE   *****************/

MODULE push_implicit_assign_or_invoke_state (void)
{
    STATE
        state = IMPLICIT_ASSIGN_OR_INVOKE;

    list_push  (&state_stack, state);
}

/***************************   PUSH LOCATOR STATE   **************************/

MODULE push_locator_state (void)
{
    STATE
        state = LOCATOR;

    list_push  (&state_stack, state);
}


/****************************   PUSH LOAD STATE   ****************************/

MODULE push_load_state (void)
{
    STATE
        state = LOAD;

    list_push  (&state_stack, state);
}


/*****************************   PUSH XML STATE   ****************************/

MODULE push_xml_state (void)
{
    STATE
        state = XML;

    list_push  (&state_stack, state);
}

/****************************   PUSH SORT STATE   ****************************/

MODULE push_sort_state (void)
{
    STATE
        state = SORT;

    list_push  (&state_stack, state);
}

