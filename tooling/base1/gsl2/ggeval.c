/*===========================================================================*
 *                                                                           *
 *  ggeval.c - Script evaluator                                              *
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
#include "ggeval.h"                     /*  Include header file              */


/*- Types -------------------------------------------------------------------*/

typedef int (EVAL_FUNCTION (SCRIPT_NODE *node));

typedef struct {
    char
        name [16];
    int
        min_params,
        max_params;
    EVAL_FUNCTION
        *evaluate;
} GSL_FUNCTION;

/*- Function prototypes -----------------------------------------------------*/

static int  evaluate_substitute_node (SCRIPT_NODE *node);
static void pretty_print             (char *text,
				      SCRIPT_NODE *pretty,
				      char *example);
static void strneat                  (char *text);
static int  evaluate_literal_node    (SCRIPT_NODE *node);
static int  evaluate_number_node     (SCRIPT_NODE *node);
static int  evaluate_operand_node    (SCRIPT_NODE *node);
static int  evaluate_symbol_node     (SCRIPT_NODE *node);
static int  evaluate_member_node     (SCRIPT_NODE *node);
static int  evaluate_function_node   (SCRIPT_NODE *node);
static int  eval_count               (SCRIPT_NODE *node);
static Bool verify_function_parameters
                                     (SCRIPT_NODE *fn_node, int min, int max);
static int  eval_env                 (SCRIPT_NODE *node);
static int  eval_index               (SCRIPT_NODE *node);
static SCOPE_BLOCK *lookup_data_scope_from_parameter (SCRIPT_NODE *node);
static int  eval_item                (SCRIPT_NODE *node);
static int  eval_name                (SCRIPT_NODE *node);
static int  eval_exists              (SCRIPT_NODE *node);
static int  eval_timestamp           (SCRIPT_NODE *node);
static int  eval_length              (SCRIPT_NODE *node);
static int  eval_defined             (SCRIPT_NODE *node);
static int  eval_deleted             (SCRIPT_NODE *node);
static int  eval_substr              (SCRIPT_NODE *node);
static SCRIPT_NODE *function_parameter 
                                     (SCRIPT_NODE *node, int n);
static Bool node_is_countable        (SCRIPT_NODE *node);
static int  eval_trim                (SCRIPT_NODE *node);
static int  eval_justify             (SCRIPT_NODE *node);
static int  eval_macro               (SCRIPT_NODE *node);
#if defined (JAVASCRIPT)
static int  evaluate_javascript_function (SCRIPT_NODE *node);
#endif
static int  evaluate_operator        (SCRIPT_NODE *node);
static int  evaluate_text_node       (SCRIPT_NODE *node);

/*- Global variables --------------------------------------------------------*/

static GSL_FUNCTION gsl_functions [] = 
{
    {"count",     1, 2, eval_count},
    {"env",       1, 1, eval_env},
    {"index",     0, 1, eval_index},
    {"item",      0, 1, eval_item},
    {"name",      0, 1, eval_name},
    {"exists",    1, 1, eval_exists},
    {"timestamp", 1, 1, eval_timestamp},
    {"length",    1, 1, eval_length},
    {"defined",   1, 1, eval_defined},
    {"deleted",   0, 1, eval_deleted},
    {"substr",    2, 4, eval_substr},
    {"trim",      1, 1, eval_trim},
    {"justify",   3, 3, eval_justify},
    {"macro",     1, 1, eval_macro},
    {"",          0, 0, NULL}
};

/*- Functions ---------------------------------------------------------------*/

int evaluate_script (SCRIPT_NODE *node)
{
    int
        result;

    if (node)
      {
        result = evaluate_script_node (node);
        if (result)
            return result;
        if (node-> result_type == TYPE_UNDEFINED)
          {
            undefined_expression_error (node);
            return -1;
          }
      }
    return 0;
}


int
evaluate_script_node (SCRIPT_NODE *node)
{
    if (node == NULL)
        return 0;

    node-> indent = node-> spaces;
    if (node-> indent > shuffle)
      {
        if (node-> indent - node-> shuffle_in > shuffle)
          {
            node-> indent -= node-> shuffle_in;
            node-> shuffle_in = 0;
          }
        else
          {
            node-> shuffle_in -= (node-> indent - shuffle - 1);
            node-> indent = shuffle + 1;
          }
      }

    if (! node-> constant)
      {
        if (node-> type == GG_SUBSTITUTE)
            if (evaluate_substitute_node (node) != 0)
                return -1;

        if (node-> type == GG_LITERAL)
            if (evaluate_literal_node (node) != 0)
                return -1;

        if (node-> type == GG_NUMBER)
            if (evaluate_number_node (node) != 0)
                return -1;

        if (node-> type == GG_OPERAND)
            if (evaluate_operand_node (node) != 0)
                return -1;

        if (node-> type  == GG_SYMBOL)
            if (evaluate_symbol_node (node) != 0)
                return -1;

        if (node-> type  == GG_MEMBER)
            if (evaluate_member_node (node) != 0)
                return -1;

        if (node-> type == GG_CALL)
            if (evaluate_function_node (node) != 0)
                return -1;

        if (node-> type == GG_OPERATOR)
            if (evaluate_operator (node) != 0)
                return -1;

        if (node-> type == GG_TEXT)
            if (evaluate_text_node (node) != 0)
                return -1;
      }

    if (node-> line_break)
        node-> shuffle_out = 0;
    else
        if (node-> result_type == TYPE_STRING)
            node-> shuffle_out = node-> shuffle_in
                               + strlen (node-> result_s)
                               - node-> length;

    return 0;
}


static int
evaluate_substitute_node (SCRIPT_NODE *node)
{
    char
        buffer [LINE_MAX + 1],
        *example = NULL;
    int
        length;
    SCRIPT_NODE
        *symbol_node;

    if (evaluate_script_node (node-> op1) != 0)
        return -1;
    if (evaluate_script_node (node-> pretty) != 0)
        return -1;
    if (evaluate_script_node (node-> format) != 0)
        return -1;

    node-> constant =                   node-> op1-> constant
                    && (node-> pretty ? node-> pretty-> constant : TRUE)
                    && (node-> format ? node-> format-> constant : TRUE);

    copy_result (node, node-> op1);
        
    /*  Only match the case if ignorecase is TRUE and the expression         */
    /*  consists of a single identifier, possibly with a default clause.     */
    if (node-> result_type == TYPE_STRING)
      {
        if (ignorecase)
          {
            symbol_node = node-> op1;
            if ((symbol_node-> type     == GG_OPERATOR)
                &&  (symbol_node-> operator == OP_DEFAULT))
                symbol_node = symbol_node-> op1;

            if ((symbol_node-> type == GG_SYMBOL)
            &&  (symbol_node-> op2))
                example = string_result (symbol_node-> op2);
          }
        pretty_print (node-> result_s, node-> pretty, example);
      }

    if (node-> op1-> result_type == TYPE_UNDEFINED)
      {
        /*  A top-level default operator with no second operand means don't  */
        /*  panic about NULL                                                 */
        if ((node-> op1-> type     == GG_OPERATOR)
        &&  (node-> op1-> operator == OP_DEFAULT)
        &&  (node-> op1-> op2      == NULL))
          {
            node-> result_type = TYPE_STRING;
            node-> result_s    = mem_strdup ("");
          }
        else
            node-> culprit = node-> op1-> culprit;
      }

    if (node-> format)
      {
        length = format_output (buffer, LINE_MAX, node);
        if (length < 0)
            return -1;

        mem_free (node-> result_s);
        node-> result_s = mem_alloc (length + 1);
        ASSERT (node-> result_s);
        strcpy (node-> result_s, buffer);
        node-> result_type = TYPE_STRING;
      }
    return 0;
}


/*  pretty_print: Reformats a string according to a format string consisting */
/*  of a comma-separated series of specifiers.  Currently accepted formats:  */
/*         lower - lower case                                                */
/*         UPPER - UPPER CASE                                                */
/*         Neat  - Neat Case                                                 */
/*         c     - Valid_c_identifier                                        */
/*         COBOL - VALID-COBOL-IDENTIFIER                                    */
/*                                                                           */
/*  If no case modifier is provided and pretty is not empty and the expr-    */
/*  is a single identifier, its case is used as an example to match for      */
/*  lower, upper or neat case.                                               */

static void
pretty_print (char *text, SCRIPT_NODE *pretty, char *example)
{
    char
        *tokens,
        *token,
        *c;
    Bool
        use_example;

    ASSERT (text);

    if (*text == 0)                     /* Do nothing to null string         */
        return;

    use_example = (example != NULL);

    if (pretty)
      {
        if (strlen (string_result (pretty)) == 0)
            use_example = FALSE;

        tokens = mem_strdup (pretty-> result_s);
        token = strtok (tokens, ", ");
        while (token)
          {
            strlwc (token);
            if (streq (token, "lower"))
              {
                use_example = FALSE;
                strlwc (text);
              }
            else
            if (streq (token, "upper"))
              {
                use_example = FALSE;
                strupc (text);
              }
            else
            if (streq (token, "neat"))
              {
                use_example = FALSE;
                strneat (text);
              }
            else
            if (streq (token, "c"))
              {
                c = text;
                if (!isalpha (*c))
                    *c = '_';
                
                while (*c)
                  {
                    if (!(isalpha (*c) || isdigit (*c)))
                        *c = '_';

                    c++;
                  }
              }
            else
            if (streq (token, "cobol"))
              {
                c = text;
                if (!isalpha (*c))
                    *c = '-';
                
                while (*c)
                  {
                    if (!(isalpha (*c) || isdigit (*c)))
                        *c = '-';

                    c++;
                  }
              }

            token = strtok (NULL, ", ");
          }
        mem_free (tokens);
      }

    if ((use_example)
    &&  (strlen (example) > 1))
      {
        c = example;

        if (isupper (*c))
            while ((isupper (*c) || !isalpha (*c)) && (*c))
                c++;

        if (*c == 0)
            strupc (text);
        else
        if (c == example + 1)
          {
            if (islower (*c))
              {
                while ((islower (*c) || !isalpha (*c)) && (*c))
                    c++;
                if (!isupper (*c))
                    strneat (text);
              }
          }
        else
            if (c == example)
              {
                if (islower (*c))
                    while ((islower (*c) || !isalpha (*c)) && (*c))
                        c++;

                if (*c == 0)
                    strlwc (text);
              }
      }
}


static void
strneat (char *text)
{
    char
        *c;

    c = text;
    while (*c)
      {
        while ((!isalpha (*c)) && (*c))
            c++;
        *c = toupper (*c);
        c++;
        while (isalpha (*c) && (*c))
          {
            *c = tolower (*c);
            c++;
          }
      }
}


static int
evaluate_literal_node (SCRIPT_NODE *node)
{
    if (evaluate_script_node (node-> op1) != 0)
        return -1;

    node-> constant    = node-> op1-> constant;
    node-> result_type = node-> op1-> result_type;
    if (node-> op1-> result_s)
      {
        node-> result_s = mem_alloc (node-> op1-> indent +
                                     strlen (node-> op1-> result_s) + 1);
        memset (node-> result_s, ' ', node-> op1-> indent);
        strcpy (node-> result_s + node-> op1-> indent, 
                node-> op1-> result_s);
      }
    node-> result_n    = node-> op1-> result_n;
    node-> culprit     = node-> op1-> culprit;

    return 0;
}


static int
evaluate_number_node (SCRIPT_NODE *node)
{
    if (evaluate_script_node (node-> op1) != 0)
        return -1;
    if (evaluate_script_node (node-> op2) != 0)
        return -1;

    node-> constant =  (node-> op1 ? node-> op1-> constant : TRUE)
                    && (node-> op2 ? node-> op2-> constant : TRUE);
        
    node-> result_s = mem_alloc (
                         (node-> op1 ? strlen (node-> op1-> result_s)     : 1)
                       + (node-> operator == OP_MINUS ? 1 : 0)
                       + (node-> op2 ? strlen (node-> op2-> result_s) + 1 : 0)
                       + 1);
    ASSERT (node-> result_s);
    node-> result_s [0] = '\0';
    if (node-> operator == OP_MINUS)
        strcat (node-> result_s, "-");
    strcat (node-> result_s, node-> op1 ? node-> op1-> result_s : "0");
    if (node-> op2)
      {
        strcat (node-> result_s, ".");
        strcat (node-> result_s, node-> op2-> result_s);
      }
    node-> result_type = TYPE_STRING;

    number_result (node);
    return 0;
}


static int
evaluate_operand_node (SCRIPT_NODE *node)
{
    int
        rc;

    if (evaluate_script (node-> op2) != 0)
        return -1;
       
    if (! node-> op2-> constant)
      {
        gg_free (node-> op3);
        node-> op3 = NULL;
      }

    if (! node-> op3)
        node-> op3 = gg_parse_expression (string_result (node-> op2));

    if (node-> op3)
      {
        rc = evaluate_script_node (node-> op3);
        if (rc == 0)
          {
            copy_result (node, node-> op3);
            node-> indent   =  node-> op3-> indent;
            node-> constant =  node-> op2-> constant
                            && node-> op3-> constant;
          }
        return rc;
      }
    else
      {
        gg_report_error ('E', "Error in operand: %s", 
                              node-> op2-> result_s);
        gg_report_error ('E', "%s", gg_error ());
        return -1;
      }
}


static int
evaluate_symbol_node (SCRIPT_NODE *node)
{
    if (node-> op1)
      {
        if (evaluate_script_node (node-> op1) != 0)
            return -1;

        if (node-> op1-> result_type == TYPE_UNDEFINED)
          {
            node-> result_type = TYPE_UNDEFINED;
            node-> culprit     = node-> op1-> culprit;
            return 0;
          }
      }

    if (node-> op2)
      {
        if (evaluate_script_node (node-> op2) != 0)
            return -1;

        if (node-> op2-> result_type == TYPE_UNDEFINED)
          {
            node-> result_type = TYPE_UNDEFINED;
            node-> culprit     = node-> op2-> culprit;
            return 0;
          }
      }

    node-> result_s = mem_strdup (symbol_value (node));
    if (node-> result_s)
        node-> result_type = TYPE_STRING;
    else
      {
        node-> result_type = TYPE_UNDEFINED;
        node-> culprit     = node;
      }

    return 0;
}


static int
evaluate_member_node (SCRIPT_NODE *node)
{
    if (evaluate_script_node (node-> op1) != 0)
        return -1;
    if (evaluate_script_node (node-> op2) != 0)
        return -1;

    node-> constant =  (node-> op1 ? node-> op1-> constant : TRUE)
                    && (node-> op2 ? node-> op2-> constant : TRUE);

    if (node-> op2 && node-> op2-> result_type == TYPE_UNDEFINED)
      {
        node-> result_type = TYPE_UNDEFINED;
	node-> culprit     = node-> op2-> culprit;
      }
    else
    if (node-> op1 && node-> op1-> result_type == TYPE_UNDEFINED)
      {
        node-> result_type = TYPE_UNDEFINED;
	node-> culprit     = node-> op1-> culprit;
      }
    else
        node-> result_type = TYPE_OK;
	
    return 0;
}


static int
evaluate_function_node (SCRIPT_NODE *node)
{
    GSL_FUNCTION
        gsl_function;
    int
        i;

    if (evaluate_script_node (node-> op2) != 0)
        return -1;
    strlwc (string_result (node-> op2));

    i = 0;
    gsl_function = gsl_functions [i];
    while (gsl_function. evaluate != NULL)
      {
        if (streq (node-> op2-> result_s, gsl_function. name))
            break;
        i++;
        gsl_function = gsl_functions [i];
      }
        
    if (gsl_function. evaluate != NULL)
        if (verify_function_parameters (node-> op3,
                                        gsl_function. min_params,
                                        gsl_function. max_params))
            return gsl_function. evaluate (node);
        else
            return -1;
    else
#if defined (JAVASCRIPT)
        return evaluate_javascript_function (node);
#else
      {
        gg_report_error ('E', "Unknown function: %s", node-> op2-> result_s);
        return -1;
      }
#endif
}                    


static Bool
verify_function_parameters (SCRIPT_NODE *fn_node, int min, int max)
{
    int
        count;
    SCRIPT_NODE
        *node;

    count = 0;
    node = fn_node;

    if (node)
        count++;
    while (node != NULL)
      {
        if ((node-> type     == GG_OPERATOR)
        &&  (node-> operator == OP_NEXT_ARG))
          {
            count++;
            node = node-> op1;
          }
        else
            break;
      }
    
    if (count < min)
      {
        gg_report_error ('E', "Missing function parameter.");
        return FALSE;
      }
    if (count > max)
      {
        gg_report_error ('E', "Too many function parameters.");
        return FALSE;
      }
    return TRUE;
}


static int
eval_count (SCRIPT_NODE *node)
{
    long
        item,
        n;
    SCRIPT_NODE
        *identifier,
        *condition;
    char
        *name,
        *xml_name;
    SCOPE_BLOCK
        *for_block;
    SCOPE_ITEM
        *scope_item;
    XML_ITEM
        *from_xml,
        *xml_item;
    Bool
        error = FALSE;

    identifier = function_parameter (node-> op3, 1);
    condition  = function_parameter (node-> op3, 2);

    if (identifier-> type == GG_SYMBOL)
      {
        if ((evaluate_script_node (identifier-> op1) != 0)
        ||  (evaluate_script_node (identifier-> op2) != 0))
            return -1;

	from_xml = lookup_from_xml (identifier-> op1);
	if (! from_xml)
            return -1;
        name  = (identifier-> op2) ? string_result (identifier-> op2) : NULL;
      }
    else
      {
        gg_report_error ('E', "Function argument must be an identifier.");
        return -1;
      }

    n = 0;
    if (from_xml)
      {
        for_block = create_scope_block ("count");
        for_block-> children = TRUE;
        xml_item = xml_first_child (from_xml);
        item = 0;
        while (xml_item)
          {
            while (xml_item)
              {
                xml_name = xml_item_name (xml_item);
                if (xml_name && name)
                  {
                    if (ignorecase)
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
                    /*  Take all named children; others are part of value  */
                    if (xml_name && (! name))
                        break;

                xml_item = xml_next_sibling (xml_item);
              }
            item++;
            
            if (xml_item)
              {
                if (condition)
                  {
                    scope_item = create_scope_item (for_block, xml_item, item);
                    for_block-> scope_item = scope_item;
                    for_block-> xml_item   = xml_item;
                    if (evaluate_script (condition) != 0)
                      {
                        error = TRUE;
                        break;
                      }
                    number_result (condition);
                    if ((condition-> result_type == TYPE_NUMBER)
                    &&  (condition-> result_n    != 0))
                        n++;
                    gg_clean (condition);
                  }
                else
                    n++;
                
                xml_item = xml_next_sibling (xml_item);
              }
          }
        destroy_scope_block ();
    
        if (error)
            return -1;
      }

    node-> result_type = TYPE_NUMBER;
    node-> result_n    = n;
    return 0;
}


static int
eval_env (SCRIPT_NODE *node)
{
    if (evaluate_script_node (node-> op3) != 0)
        return -1;
            
    if (node-> op3-> result_type != TYPE_UNDEFINED)
      {
        node-> result_type = TYPE_STRING;
        node-> result_s    = mem_strdup (getenv (string_result (node-> op3)));
      }
    else
      {
        node-> result_type = TYPE_UNDEFINED;
        node-> culprit     = node-> op3;
      }
    return 0;
}


static int
eval_index (SCRIPT_NODE *node)
{
    SCOPE_BLOCK
        *scope_block;

    scope_block = lookup_data_scope_from_parameter (node);
    if (! scope_block)
        return -1;

    if (scope_block-> index)
      {
        node-> result_type = TYPE_NUMBER;
        node-> result_n    = scope_block-> index;
      }
    else
        node-> result_type = TYPE_UNDEFINED;

    return 0;
}


SCOPE_BLOCK *
lookup_data_scope_from_parameter (SCRIPT_NODE *node)
{
    SCOPE_BLOCK
        *scope_block;

    if (node-> op3)
        if ((node-> op3-> type == GG_OPERAND)
        ||  (node-> op3-> type == GG_SYMBOL))
          {
            if (evaluate_script_node (node-> op3-> op2) != 0)
                return NULL;

            if (node-> op3-> op1)
              {
                gg_report_error ('E', "Invalid scope.");
                return NULL;
              }
            
            scope_block = lookup_scope_block (string_result (node-> op3-> op2));
            if (! scope_block)
              {
                gg_report_error ('E', "Unknown data scope: %s",
                                      node-> op3-> op2-> result_s);
                return NULL;
              }
          }
        else
        if (node-> op3-> type == GG_NUMBER)
          {
            if (evaluate_script_node (node-> op3) != 0)
                return NULL;

            scope_block = lookup_scope_block (string_result (node-> op3));
            if (! scope_block)
              {
                gg_report_error ('E', "Unknown data scope: %s",
                                      node-> op3-> result_s);
                return NULL;
              }
          }
        else
          {
            gg_report_error ('E', "Argument must be a legal identifier.");
            return NULL;
          }
    else
      {
        FORLIST (scope_block, scope_stack)
            if (scope_block-> children)
                break;

        if ((void *) scope_block == & scope_stack)
          {
            gg_report_error ('E', "No data scope.");
            return NULL;
          }
      }
    return scope_block;
}


static int
eval_item (SCRIPT_NODE *node)
{
    SCOPE_BLOCK
        *scope_block;

    scope_block = lookup_data_scope_from_parameter (node);
    if (! scope_block)
        return -1;

    node-> result_type = TYPE_NUMBER;
    node-> result_n    = scope_block-> scope_item-> item;

    return 0;
}


static int
eval_name (SCRIPT_NODE *node)
{
    SCOPE_BLOCK
        *scope_block;

    scope_block = lookup_data_scope_from_parameter (node);
    if (! scope_block)
        return -1;

    if ((scope_block-> xml_item)
    &&  (xml_item_name (scope_block-> xml_item)))
      {
        node-> result_type = TYPE_STRING;
        node-> result_s    = mem_strdup (
                             xml_item_name (scope_block-> xml_item));
      }
    else
        node-> result_type = TYPE_UNDEFINED;

    return 0;
}


static int
eval_exists (SCRIPT_NODE *node)
{
    if (evaluate_script_node (node-> op3) != 0)
        return -1;
        

    node-> result_type = TYPE_NUMBER;
    node-> result_n    = file_exists (string_result (node-> op3)) ? 1 : 0;
    return 0;
}


static int
eval_timestamp (SCRIPT_NODE *node)
{
    char
        buffer [LINE_MAX + 1];
    time_t
        timer;

    if (evaluate_script_node (node-> op3) != 0)
        return -1;
        
    timer = get_file_time (string_result (node-> op3));
    if (timer)
      {
        node-> result_type = TYPE_STRING;
        sprintf (buffer, "%ld%ld", timer_to_date (timer),
                 timer_to_time (timer));
        node-> result_s = mem_strdup (buffer);
        ASSERT (node-> result_s);
      }
    return 0;
}


static int
eval_length (SCRIPT_NODE *node)
{
    if (evaluate_script_node (node-> op3) != 0)
        return -1;

    node-> constant = node-> op2-> constant
                   && node-> op3-> constant;

    if (node-> op3)
        if (node-> op3-> result_type != TYPE_UNDEFINED)
          {
            node-> result_type = TYPE_NUMBER;
            node-> result_n    = strlen (string_result (node-> op3));
          }
        else
          {
            node-> result_type = TYPE_UNDEFINED;
            node-> culprit     = node-> op3-> culprit;
          }
    else
        node-> result_type = TYPE_UNDEFINED;

    return 0;
}


static int
eval_defined (SCRIPT_NODE *node)
{

    if (evaluate_script_node (node-> op3) != 0)
        return -1;

    node-> result_n = (node-> op3-> result_type != TYPE_UNDEFINED) ? 1 : 0;
    node-> result_type = TYPE_NUMBER;
    return 0;
}


static int
eval_deleted (SCRIPT_NODE *node)
{
    SCOPE_BLOCK
        *scope_block;

    scope_block = lookup_data_scope_from_parameter (node);
    if (! scope_block)
        return -1;

    node-> result_n = (scope_block-> xml_item == NULL) ? 1 : 0;
    node-> result_type = TYPE_NUMBER;
    return 0;
}


static int
eval_substr (SCRIPT_NODE *node)
{
    SCRIPT_NODE
        *string,
        *start,
        *end,
        *len;
    unsigned long
        start_n = 0,
        end_n   = 0,
        len_n   = 0;

    string = function_parameter (node-> op3, 1);
    start  = function_parameter (node-> op3, 2);
    end    = function_parameter (node-> op3, 3);
    len    = function_parameter (node-> op3, 4);
    if (evaluate_script_node (string) != 0)
        return -1;
    if (string-> result_type == TYPE_UNDEFINED)
      {
        node-> culprit = string-> culprit;
        return 0;
      }
    else
        string_result (string);
  
    if (start && end && len)
      {
        gg_report_error ('E', "Too many parameters for function 'substr'.");
        return -1;
      }
    if (!(start || end || len))
      {
        gg_report_error ('E', "Not enough parameters for function 'substr'.");
        return -1;
      }
    
    if (start)
      {
        if (evaluate_script_node (start) != 0)
            return -1;
        if (node_is_countable (start))
            start_n = (unsigned long) start-> result_n;
        else
          {
            gg_report_error ('E', "Illegal 'Start' value: %s", 
                                  string_result (start));
            return -1;
          }
      }
    if (end)
      {
        if (evaluate_script_node (end) != 0)
            return -1;
        if (node_is_countable (end))
            end_n = (unsigned long) end-> result_n;
        else
          {
            gg_report_error ('E', "Illegal 'End' value: %s", 
                                  string_result (end));
            return -1;
          }
      }
    if (len)
      {
        if (evaluate_script_node (len) != 0)
            return -1;
        if (node_is_countable (len))
            len_n = (unsigned long) len-> result_n;
        else
          {
            gg_report_error ('E', "Illegal 'Len' value: %s", 
                                  string_result (len));
            return -1;
          }
      }

    if (start &&  end
    && (end_n < start_n))
      {
        gg_report_error ('E', "'End' must be at least 'Start' in 'substr'");
        return -1;
      }
    if (len && !start)
      {
        if (!end)
            end_n = strlen (string-> result_s) - 1;

        start_n = end_n - len_n + 1;
      }
    else
      {
        if (!start)
            start_n = 0;

        if (!len)
          {
            if (end)
                len_n = end_n - start_n + 1;
            else
                len_n = strlen (string-> result_s);
          }
      }
    if (start_n >= strlen (string-> result_s))
        node-> result_s = mem_strdup ("");
    else
      {
        node-> result_s = mem_alloc (len_n + 1);
        if (start_n >= 0)
            strncpy (node-> result_s, & string-> result_s [start_n], len_n);
        else
            strncpy (node-> result_s, string-> result_s, len_n);
        
        (node-> result_s) [len_n] = '\0';
      }
    node-> result_type = TYPE_STRING;

    node-> constant =  node-> op2-> constant
                    && (string ? string-> constant : TRUE)
                    && (start  ? start -> constant : TRUE)
                    && (end    ? end   -> constant : TRUE)
                    && (len    ? len   -> constant : TRUE);

    return 0;
}


/*  Return n-th function parameter  */
static SCRIPT_NODE *
function_parameter (SCRIPT_NODE *node, int n)
{
    int
       count = 0;

    if ((node)
    &&  (node-> type           == GG_OPERATOR)
    &&  (node-> operator       == OP_NEXT_ARG))
      {
        count++;

        while ((node-> op1)
           &&  (node-> op1-> type     == GG_OPERATOR)
           &&  (node-> op1-> operator == OP_NEXT_ARG))
          {
            node = node-> op1;
            count++;
          }
      }

    if (n > count + 1)
        return NULL;

    while (n > 2)
      {
        node = node-> parent;
        n--;
      }
    if (n == 1)
        if ((node-> type     == GG_OPERATOR)
        &&  (node-> operator == OP_NEXT_ARG))
            return node-> op1;
        else
            return node;
    else
        return node-> op2;
}


static Bool
node_is_countable (SCRIPT_NODE *node)
{
    if (node)
      {
        if (node-> result_type != TYPE_UNDEFINED)
          {
            number_result (node);
            if ((node-> result_type == TYPE_NUMBER)
            &&  (node-> result_n == floor (node-> result_n))
            &&  (node-> result_n >= 0))
                return TRUE;
          }
        else
            undefined_expression_error (node);
      }
    
    return FALSE;
}


static int
eval_trim (SCRIPT_NODE *node)
{
    if (evaluate_script_node (node-> op3) != 0)
        return -1;

    node-> constant = node-> op2-> constant
                   && node-> op3-> constant;

    node-> result_type = node-> op3-> result_type;
    if (node-> op3-> result_s)
      {
        node-> result_s = mem_strdup (strcrop (node-> op3-> result_s));
        ASSERT (node-> result_s);
      }
    node-> result_n = node-> op3-> result_n;
    node-> culprit  = node-> op3-> culprit;
    return 0;
}


static int
eval_justify (SCRIPT_NODE *node)
{
    SCRIPT_NODE
        *string,
        *width,
        *prefix;
    unsigned long
        width_n;

    string = function_parameter (node-> op3, 1);
    width  = function_parameter (node-> op3, 2);
    prefix = function_parameter (node-> op3, 3);
    if (evaluate_script_node (string) != 0)
        return -1;
    if (evaluate_script_node (prefix) != 0)
        return -1;
    if (evaluate_script_node (width) != 0)
        return -1;

    node-> constant =  (node-> op2-> constant
                    &&  (string ? string-> constant : TRUE)
                    &&  (width  ? width -> constant : TRUE)
                    &&  (prefix ? prefix-> constant : TRUE));

    if (node_is_countable (width))
        width_n = (unsigned long) width-> result_n;
    else
      {
        gg_report_error ('E', "Illegal 'Width' value: %s", 
                              string_result (width));
        return -1;
      }
    node-> result_s = 
                          strreformat (string_result (string),
                                       width_n,
                                       string_result (prefix));
    node-> result_type = TYPE_STRING;
    return 0;
}

static int
eval_macro (SCRIPT_NODE *node)
{
    if (evaluate_script_node (node-> op3) != 0)
        return -1;

    node-> result_type = TYPE_NUMBER;
    if (sym_lookup_symbol (macros, string_result (node-> op3)))
        node-> result_n = 1;
    else
        node-> result_n = 0;

    return 0;
}


#if defined (JAVASCRIPT)

static int
evaluate_javascript_function (SCRIPT_NODE *node)
{
    SCRIPT_NODE
        *param_node = node-> op3;
    int
        count  = 0,
        length = 0;
    char
        *js_text;
    JSScript
        *script;
    jsval
        result;

    if (param_node)
      {
        while ((param_node-> type     == GG_OPERATOR)
           &&  (param_node-> operator == OP_NEXT_ARG))
          {
            if (evaluate_script_node (param_node-> op3) != 0)
                return -1;
            count ++;
            if (param_node-> op3)
              {
                number_result (param_node-> op3);
                if (param_node-> op3-> result_type != TYPE_UNDEFINED)
                    length += strlen (string_result (param_node-> op3));
                if (param_node-> op3-> result_type == TYPE_STRING)
                    length += 2;
              }
            if (param_node-> op1)
                param_node = param_node-> op1;
            else
                break;
          }

        count ++;
        if ((param_node-> type     != GG_OPERATOR)
        ||  (param_node-> operator != OP_NEXT_ARG))
          {
            if (evaluate_script_node (param_node) != 0)
                return -1;
            if (param_node)
              {
                number_result (param_node);
                if (param_node-> result_type != TYPE_UNDEFINED)
                    length += strlen (string_result (param_node));
                if (param_node-> result_type == TYPE_STRING)
                    length += 2;
              }
          }
      }
    length += strlen (node-> op1-> result_s) + count + 1;
    if (count == 0)
        length ++;
    js_text = mem_alloc (length + 1);
    ASSERT (js_text);
    strcpy (js_text, node-> op1-> result_s);
    strcat (js_text, "(");
    if (count > 0)
      {
        if ((param_node-> type     != GG_OPERATOR)
        ||  (param_node-> operator != OP_NEXT_ARG))
          {
            if (param_node-> result_type == TYPE_STRING)
                strcat (js_text, "\"");
            if (param_node-> result_type != TYPE_UNDEFINED)
                strcat (js_text, param_node-> result_s);
            if (param_node-> result_type == TYPE_STRING)
                strcat (js_text, "\"");

            param_node = param_node-> parent;
          }

        while (param_node != node)
          {
            if (param_node-> op3)
              {
                if (param_node-> op3-> result_type == TYPE_STRING)
                    strcat (js_text, ",\"");
                if (param_node-> op3-> result_type != TYPE_UNDEFINED)
                    strcat (js_text, param_node-> op3-> result_s);
                if (param_node-> op3-> result_type == TYPE_STRING)
                    strcat (js_text, "\"");
              }
            else
                strcat (js_text, ",");

            param_node = param_node-> parent;
          }
      }
    strcat (js_text, ")");

    script = JS_CompileScript (cx, obj,
                               js_text, length,
                               NULL, 0);
    mem_free (js_text);
    if (!script)
        return -1;

    if (JS_ExecuteScript(cx, obj, script, &result))
      {
        node-> result_s = mem_strdup (
                              JS_GetStringBytes (
                                  JS_ValueToString(cx, result)));
        node-> result_type = TYPE_STRING;
      }
    else
        node-> result_type = TYPE_UNDEFINED;

    JS_DestroyScript(cx, script);
    return 0;
}


#endif

static int
evaluate_operator (SCRIPT_NODE *node)
{
    OPERATOR
        the_operator = node-> operator;
    double
        n = 0;
    long
        i;
    Bool
        safe;
    int
        offset,
        length,
        cmp;

    node-> line_break  = node-> op2 ? node-> op2-> line_break : FALSE;
    node-> extend      = node-> op2 ? node-> op2-> extend     : FALSE;
    node-> result_type = TYPE_UNDEFINED;
    node-> result_s    = NULL;
    node-> culprit     = NULL;

    if (the_operator == OP_NEXT_ARG)
      {
        if (evaluate_script_node (node-> op1) != 0)
            return -1;
        if (evaluate_script_node (node-> op2) != 0)
            return -1;
        return 0;
      }

    safe = ((the_operator == OP_SAFE_EQUALS)
         || (the_operator == OP_SAFE_NOT_EQUALS)
         || (the_operator == OP_SAFE_GREATER_THAN)
         || (the_operator == OP_SAFE_LESS_THAN)
         || (the_operator == OP_SAFE_GREATER_EQUAL)
         || (the_operator == OP_SAFE_LESS_EQUAL));

    /*  Evaluate first operand  */
    if (the_operator == OP_UNDEFINED)
        node-> op1-> shuffle_in = node-> shuffle_in;

    if (evaluate_script_node (node-> op1) != 0)
        return -1;

    node-> constant =  (node-> op1 ? node-> op1-> constant : TRUE);

    if (node-> op1)
      {
        /*  Now try some optimisation.  This also allows for             */
        /*  constructs such as '.if defined (N) & N = 1'                 */
        if ((the_operator == OP_OR) || (the_operator == OP_AND))
          {
            number_result (node-> op1);
            if (node-> op1-> result_type == TYPE_NUMBER)
              {
                if ((the_operator == OP_OR) 
                &&  (node-> op1-> result_n != 0))
                  {
                    node-> result_type = TYPE_NUMBER;
                    node-> result_n    = 1;
                    return 0;
                  }
                else
                if ((the_operator == OP_AND)
                &&  (node-> op1-> result_n == 0))
                  {
                    node-> result_type = TYPE_NUMBER;
                    node-> result_n    = 0;
                    return 0;
                  }
              }
          }
        if (node-> op1-> result_type == TYPE_UNDEFINED)
          {
            if (the_operator == OP_DEFAULT)
              {
                if (evaluate_script_node (node-> op2) != 0)
                    return -1;

                node-> constant &= (node-> op2 ? node-> op2-> constant : TRUE);

                copy_result (node, node-> op2);
                return 0;
              }
            else
            if (! safe)
              {
                node-> culprit = node-> op1-> culprit;
                return 0;
              }
            else                        /*  Return FALSE if op is undefined  */
              {
                node-> result_type = TYPE_NUMBER;
                node-> result_n    = 0;
                return 0;
              }
          }
        else
            if (the_operator == OP_DEFAULT)
              {
                copy_result (node, node-> op1);
                return 0;
              }
      }

    if (the_operator == OP_UNDEFINED)
        node-> op2-> shuffle_in = node-> op1-> shuffle_out;

    if (evaluate_script_node (node-> op2) != 0)
        return -1;

    node-> constant &= (node-> op2 ? node-> op2-> constant : TRUE);

    if (node-> op2)
      {
        if (node-> op2-> result_type == TYPE_UNDEFINED)
          {
            if (! safe)
              {
                node-> culprit = node-> op2-> culprit;
                return 0;
              }
            else              /*  Return FALSE if operand is undefined       */
              {
                node-> result_type = TYPE_NUMBER;
                node-> result_n    = 0;
                return 0;
              }
          }
      }
    else                      /* This should never happen, but might save us */
        return 0;      

    /*  Change safe operator to non-safe equevalent */
    if (the_operator == OP_SAFE_EQUALS)
        the_operator = OP_EQUALS;
    else if (the_operator == OP_SAFE_NOT_EQUALS)
        the_operator = OP_NOT_EQUALS;
    else if (the_operator == OP_SAFE_GREATER_THAN)
        the_operator = OP_GREATER_THAN;
    else if (the_operator == OP_SAFE_LESS_THAN)
        the_operator = OP_LESS_THAN;
    else if (the_operator == OP_SAFE_GREATER_EQUAL)
        the_operator = OP_GREATER_EQUAL;
    else if (the_operator == OP_SAFE_LESS_EQUAL)
        the_operator = OP_LESS_EQUAL;

    if (node-> op1)                     /*  Not unary operator               */
        number_result (node-> op1);
    number_result (node-> op2);
 
    /*  If operator wants numeric operands, do conversion  */
    if ((the_operator == OP_TIMES)
    ||  (the_operator == OP_DIVIDE)
    ||  (the_operator == OP_PLUS)
    ||  (the_operator == OP_MINUS)
    ||  (the_operator == OP_OR)
    ||  (the_operator == OP_AND)
    ||  (the_operator == OP_NOT))
      {
        /*  Binary PLUS between string operands or TIMES with string first   */
        /*  operand are allowed.  Otherwise both operands must be numeric.   */
        if ( (! (   ( (the_operator == OP_PLUS)
                  &&  (node-> op1)           )
        ||          ( (the_operator == OP_TIMES)
                  &&  (node-> op2-> result_type == TYPE_NUMBER) ) ))
        &&   ((node-> op1 ? node-> op1-> result_type == TYPE_STRING : FALSE)
        ||    (node-> op2-> result_type == TYPE_STRING)) )
          {
            gg_report_error ('E', "Non-numeric operand to numeric operator.");
            return -1;
          }
      }

    /*  If the operator is UNDEFINED then we are concatenating strings,      */
    /*  including the spaces, and shuffling if enabled.  If the operator is  */
    /*  PLUS and at least one of the operands is non-numeric then we also    */
    /*  concatenate but do not include the spaces before the 2nd string.     */
    if (( the_operator == OP_UNDEFINED)
    ||  ((the_operator == OP_PLUS) && (node-> op1)
    &&   ((node-> op1-> result_type == TYPE_STRING)
    ||    (node-> op2-> result_type == TYPE_STRING)) ) )
      {
        node-> constant = FALSE;    /*  Need to re-evaluate for shuffle  */
        offset = strlen (string_result (node-> op1));
        if (node-> op1-> line_break)
            offset++;

        length = offset
               + node-> op2-> indent
               + strlen (string_result (node-> op2));

        node-> result_s = mem_alloc (length + 1);
        ASSERT (node-> result_s);
        if (node-> op1)
          {
            strcpy (node-> result_s, node-> op1-> result_s);
            if (node-> op1-> line_break)
                strcat (node-> result_s, "\n");
          }
        
        if (the_operator == OP_UNDEFINED)
          {
            memset (node-> result_s + offset, ' ', node-> op2-> indent);
            offset += node-> op2-> indent;
          }
        strcpy (node-> result_s + offset, node-> op2-> result_s);
        node-> result_type = TYPE_STRING;
        return 0;
      }

    /*  "string" * 3 = "stringstringstring"  */
    if ((the_operator == OP_TIMES)
    &&  (node-> op1-> result_type == TYPE_STRING)
    &&  (node-> op2-> result_type == TYPE_NUMBER))
      {
        length = strlen (node-> op1-> result_s);
        i = (long) node-> op2-> result_n;      /*  Integer value  */
        if (i < 0)
          {
            gg_report_error ('E', "String repeat count is negative.");
            return -1;
          }

        node-> result_s = mem_alloc (length * i + 1);
        ASSERT (node-> result_s);
        offset = 0;
        while (i-- > 0)
          {
            memcpy (node-> result_s + offset, node-> op1-> result_s, length);
            offset += length;
          }
        node-> result_s [offset] = '\0';
        node-> result_type = TYPE_STRING;
        return 0;
      }

    /*  If at least one operand is non-numeric, treat as a string operator   */
    if (node-> op1)
        if ((node-> op1-> result_type == TYPE_STRING)
        ||  (node-> op2-> result_type == TYPE_STRING))
      {
        cmp = strcmp (string_result (node-> op1),
                      string_result (node-> op2));
        switch (the_operator)
          {
            case OP_EQUALS        : n = (cmp == 0);  break;
            case OP_NOT_EQUALS    : n = (cmp != 0);  break;
            case OP_GREATER_THAN  : n = (cmp >  0);  break;
            case OP_LESS_THAN     : n = (cmp <  0);  break;
            case OP_GREATER_EQUAL : n = (cmp >= 0);  break;
            case OP_LESS_EQUAL    : n = (cmp <= 0);  break;
            default               : gg_report_error ('E', "Invalid operator.");
                                    return -1;
          }
        node-> result_type = TYPE_NUMBER;
        node-> result_n    = n;
        return 0;
      }

    /*  If we got here then we have two numeric operands.  The operator may  */
    /*  be unary, in which case the first operand is zero.                   */
    switch (the_operator)
      {
        case OP_NOT           : n = (double) (! (Bool) node-> op2-> result_n);
                                                                        break;
        case OP_PLUS          : n = (node-> op1 ? node-> op1-> result_n : 0)
                                  +  node-> op2-> result_n;             break;
        case OP_MINUS         : n = (node-> op1 ? node-> op1-> result_n : 0)
                                  -  node-> op2-> result_n;             break;
        case OP_TIMES         : n = node-> op1-> result_n
                                  * node-> op2-> result_n;              break;
        case OP_DIVIDE        : if (node-> op2-> result_n != 0)
                               {
                                 n = node-> op1-> result_n
                                   / node-> op2-> result_n;            
                                 break;
                               }
                             else
                               {
                                 gg_report_error ('E', "Division by zero.");
                                 return -1;
                               }
        case OP_EQUALS        : n = (double) (node-> op1-> result_n
                                           == node-> op2-> result_n);    break;
        case OP_NOT_EQUALS    : n = (double) (node-> op1-> result_n
                                           != node-> op2-> result_n);    break;
        case OP_GREATER_THAN  : n = (double) (node-> op1-> result_n
                                            > node-> op2-> result_n);    break;
        case OP_LESS_THAN     : n = (double) (node-> op1-> result_n
                                            < node-> op2-> result_n);    break;
        case OP_GREATER_EQUAL : n = (double) (node-> op1-> result_n
                                           >= node-> op2-> result_n);    break;
        case OP_LESS_EQUAL    : n = (double) (node-> op1-> result_n
                                           <= node-> op2-> result_n);    break;
        case OP_OR            : n = (double) ((Bool) node-> op1-> result_n
                                           || (Bool) node-> op2-> result_n);
                                                                         break;
        case OP_AND           : n = (double) ((Bool) node-> op1-> result_n
                                           && (Bool) node-> op2-> result_n);
                                                                         break;
        default               : gg_report_error ('E', "Invalid operator.");
                                return -1;
      }
    node-> result_type = TYPE_NUMBER;
    node-> result_n    = n;
    return 0;
}


static int
evaluate_text_node (SCRIPT_NODE *node)
{
    char
        *out,
        *in,
        *sign;

    sign = operator_text (node-> operator);

    /*  Replace \x with x */
    node-> result_type = TYPE_STRING;
    if (node-> text)
      {
        node-> result_s = mem_alloc (strlen (node-> text)
                                   + strlen (sign) + 1);
        strcpy (node-> result_s, sign);
        out = node-> result_s + strlen (sign);
        in  = node-> text;
        while (*in)
            if (*in == '\\')
              {
                *out = *++in;
                if (*out == 'n')
                    *out = '\n';
		else
		if (*out == 'r')
		    *out = '\r';
		else
		if (*out == 't')
		    *out = '\t';
                out++;
                in++;
              }
            else
                *out++ = *in++;
        *out = '\0';
      }
    else
        node-> result_s = mem_strdup (sign);

    node-> constant = TRUE;

    return 0;
}
