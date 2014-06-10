/*===========================================================================
 *                                                                           
 *  ggfunc.inc  GSL Built-In Functions
 *                                                                           
 *  Written:    2000/02/16    iMatix <tools@imatix.com>
 *  Revised:    2002/10/17
 *                                                                            
 *  Copyright (c) 1996-2009 iMatix Corporation
 *                                                                            
 *  This program is free software; you can redistribute it and/or modify      
 *  it under the terms of the GNU General Public License as published by      
 *  the Free Software Foundation; either version 2 of the License, or         
 *  (at your option) any later version.                                       
 *                                                                            
 *  This program is distributed in the hope that it will be useful,           
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             
 *  GNU General Public License for more details.                              
 *                                                                            
 *  You should have received a copy of the GNU General Public License         
 *  along with this program; if not, write to the Free Software               
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
 *===========================================================================*/

/*- Function prototypes -----------------------------------------------------*/

static int register_inbuilt_functions (void);

static SCOPE_BLOCK *lookup_data_scope_from_parameter (RESULT_NODE *node);

static EVAL_FUNCTION
    eval_count,
    eval_index,
    eval_item,
    eval_total,
    eval_name,
    eval_alias,
    eval_defined,
    eval_deleted,
    eval_macro,
    eval_scope;

/*- Global variables --------------------------------------------------------*/

static PARM_LIST
    one_value_parm_list = {PARM_VALUE},
    one_scope_parm_list = {PARM_SIMPLE_SCOPE};

static PARM_LIST
    count_parm_list     = {PARM_EXPRESSION, PARM_EXPRESSION, PARM_SIMPLE_SCOPE};

static GSL_FUNCTION gsl_functions [] = 
{
    {"alias",   0, 1, 1, (void *) &one_scope_parm_list, TRUE, eval_alias},
    {"count",   0, 3, 3, (void *) &count_parm_list,     TRUE, eval_count},
    {"defined", 1, 1, 1, (void *) &one_value_parm_list, TRUE, eval_defined},
    {"deleted", 0, 1, 1, (void *) &one_scope_parm_list, TRUE, eval_deleted},
    {"index",   0, 1, 1, (void *) &one_scope_parm_list, TRUE, eval_index},
    {"item",    0, 1, 1, (void *) &one_scope_parm_list, TRUE, eval_item},
    {"macro",   1, 1, 1, (void *) &one_value_parm_list, TRUE, eval_macro},
    {"name",    0, 1, 1, (void *) &one_scope_parm_list, TRUE, eval_name},
    {"scope",   1, 1, 1, (void *) &one_scope_parm_list, TRUE, eval_scope},
    {"total",   0, 1, 1, (void *) &one_scope_parm_list, TRUE, eval_total}
};

/*- Functions ---------------------------------------------------------------*/

static int register_inbuilt_functions (void)
{
    int
        rc;

    rc = object_register (NULL,    gsl_functions,
                          tblsize (gsl_functions), NULL);

    return rc;
}


static int
eval_count (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    raise_exception (evaluate_count_event);

    return 0;
}


static int
eval_index (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    SCOPE_BLOCK
        *scope_block;
    RESULT_NODE
        *scope = argc > 0 ? argv [0] : NULL;

    scope_block = lookup_data_scope_from_parameter (scope);
    if (scope_block
    && (scope_block-> index))
      {
        result-> value. type = TYPE_NUMBER;
        result-> value. n    = scope_block-> index;
      }

    return 0;
}


SCOPE_BLOCK *
lookup_data_scope_from_parameter (RESULT_NODE *node)
{
    SCOPE_BLOCK
        *scope_block;
    char
        *error_text;

    if (node)
      {
        scope_block = lookup_simple_scope (& tcb-> scope_stack,
                                           & node-> value,
                                           tcb-> ignorecase,
                                           &error_text);
        if ((! scope_block)
        &&   ( error_text))
            report_error (error_event, "%s", error_text);
      }
    else
      {
        FORLIST (scope_block, tcb-> scope_stack)
            if (scope_block-> stacked)
                break;

        if ((void *) scope_block == & tcb-> scope_stack)
            scope_block = NULL;
      }
    return scope_block;
}


static int
eval_item (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    SCOPE_BLOCK
        *scope_block;
    RESULT_NODE
        *scope = argc > 0 ? argv [0] : NULL;

    scope_block = lookup_data_scope_from_parameter (scope);
    if (scope_block
    && (scope_block-> scope_item))
      {
        result-> value. type = TYPE_NUMBER;
        result-> value. n    = scope_block-> scope_item-> item;
      }

    return 0;
}


static int
eval_total (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    SCOPE_BLOCK
        *scope_block;
    RESULT_NODE
        *scope = argc > 0 ? argv [0] : NULL;

    scope_block = lookup_data_scope_from_parameter (scope);
    if (scope_block
    && (scope_block-> total))
      {
        result-> value. type = TYPE_NUMBER;
        result-> value. n    = scope_block-> total;
      }

    return 0;
}


static int
eval_name (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    SCOPE_BLOCK
        *scope_block;
    RESULT_NODE
        *scope = argc > 0 ? argv [0] : NULL;

    scope_block = lookup_data_scope_from_parameter (scope);
    if (scope_block
    && (scope_block-> xml_item)
    && (xml_item_name (scope_block-> xml_item)))
      {
        result-> value. type = TYPE_STRING;
        result-> value. s    = mem_strdup (xml_item_name (scope_block-> xml_item));
      }

    return 0;
}


static int
eval_alias (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    SCOPE_BLOCK
        *scope_block;
    RESULT_NODE
        *scope = argc > 0 ? argv [0] : NULL;

    scope_block = lookup_data_scope_from_parameter (scope);
    if (scope_block
    && (scope_block-> name))
      {
        result-> value. type = TYPE_STRING;
        result-> value. s    = mem_strdup (scope_block-> name);
      }

    return 0;
}


static int
eval_defined (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    RESULT_NODE
        *expr = argc > 0 ? argv [0] : NULL;

    ASSERT (expr);

    result-> value. n    = expr-> value. type != TYPE_UNDEFINED ? 1 : 0;
    result-> value. type = TYPE_NUMBER;
    return 0;
}


static int
eval_deleted (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    SCOPE_BLOCK
        *scope_block;
    RESULT_NODE
        *scope = argc > 0 ? argv [0] : NULL;

    scope_block = lookup_data_scope_from_parameter (scope);
    if (scope_block)
      {
        result-> value. n = (scope_block-> xml_item == NULL) ? 1 : 0;
        result-> value. type = TYPE_NUMBER;
      }

    return 0;
}


static int
eval_macro (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    RESULT_NODE
        *name = argc > 0 ? argv [0] : NULL;

    if (! arguments_are_defined (argc, argv, result))
        return 0;
    ASSERT (name);
     
    result-> value. type = TYPE_NUMBER;
    if (sym_lookup_symbol (tcb-> macros, string_value (& name-> value)))
        result-> value. n = 1;
    else
        result-> value. n = 0;

    return 0;
}


static int
eval_scope (int argc, RESULT_NODE **argv, RESULT_NODE *result, THREAD *dummy)
{
    RESULT_NODE
        *scope = argc > 0 ? argv [0] : NULL;
    SCOPE_BLOCK
        *scope_block;

    if (! arguments_are_defined (argc, argv, result))
        return 0;

    scope_block = lookup_simple_scope (& tcb-> scope_stack,
                                       & scope-> value,
                                       tcb-> ignorecase,
                                       NULL);

    result-> value. type = TYPE_NUMBER;
    if (scope_block)
        result-> value. n = 1;
    else
        result-> value. n = 0;

    return 0;
}

