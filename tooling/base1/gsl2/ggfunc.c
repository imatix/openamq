/*===========================================================================*
 *                                                                           *
 *  ggfunc.c - Built-in GSL functions                                        *
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
#include "ggobjt.h"                     /*  Object Module Header             */
#include "ggeval.h"                     /*  Expression Evaluator Header      */
#include "ggfunc.h"                     /*  Include header file              */


/*- Types -------------------------------------------------------------------*/


/*- Function prototypes -----------------------------------------------------*/

static SCOPE_BLOCK *lookup_data_scope_from_parameter (SCRIPT_NODE *node);

static EVAL_FUNCTION
    eval_count,
    eval_index,
    eval_item,
    eval_name,
    eval_defined,
    eval_deleted,
    eval_macro;

/*- Global variables --------------------------------------------------------*/

static GSL_FUNCTION gsl_functions [] = 
{
    {"count",     1, 2, eval_count},
    {"defined",   1, 1, eval_defined},
    {"deleted",   0, 1, eval_deleted},
    {"index",     0, 1, eval_index},
    {"item",      0, 1, eval_item},
    {"macro",     1, 1, eval_macro},
    {"name",      0, 1, eval_name}
};

/*- Functions ---------------------------------------------------------------*/

int register_inbuilt_functions (void)
{
    return object_register (NULL, gsl_functions, tblsize (gsl_functions));
}


static int
eval_count (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    long
        item,
        n;
    SCRIPT_NODE
        *identifier = argc > 0 ? argv [0] : NULL,
        *condition  = argc > 1 ? argv [1] : NULL;
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

    result-> result_type = TYPE_NUMBER;
    result-> result_n    = n;
    return 0;
}


static int
eval_index (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    SCOPE_BLOCK
        *scope_block;
    SCRIPT_NODE
        *scope = argc > 0 ? argv [0] : NULL;

    scope_block = lookup_data_scope_from_parameter (scope);
    if (! scope_block)
        return -1;

    if (scope_block-> index)
      {
        result-> result_type = TYPE_NUMBER;
        result-> result_n    = scope_block-> index;
      }
    else
        result-> result_type = TYPE_UNDEFINED;

    return 0;
}


SCOPE_BLOCK *
lookup_data_scope_from_parameter (SCRIPT_NODE *node)
{
    SCOPE_BLOCK
        *scope_block;

    if (node)
        if ((node-> type == GG_OPERAND)
        ||  (node-> type == GG_SYMBOL))
          {
            if (evaluate_script_node (node-> op2) != 0)
                return NULL;

            if (node-> op1)
              {
                gg_report_error ('E', "Invalid scope.");
                return NULL;
              }
            
            scope_block = lookup_scope_block (string_result (node-> op2));
            if (! scope_block)
              {
                gg_report_error ('E', "Unknown data scope: %s",
                                      node-> op2-> result_s);
                return NULL;
              }
          }
        else
        if (node-> type == GG_NUMBER)
          {
            if (evaluate_script_node (node) != 0)
                return NULL;

            scope_block = lookup_scope_block (string_result (node));
            if (! scope_block)
              {
                gg_report_error ('E', "Unknown data scope: %s",
                                      node-> result_s);
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
eval_item (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    SCOPE_BLOCK
        *scope_block;
    SCRIPT_NODE
        *scope = argc > 0 ? argv [0] : NULL;

    scope_block = lookup_data_scope_from_parameter (scope);
    if (! scope_block)
        return -1;

    result-> result_type = TYPE_NUMBER;
    result-> result_n    = scope_block-> scope_item-> item;

    return 0;
}


static int
eval_name (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    SCOPE_BLOCK
        *scope_block;
    SCRIPT_NODE
        *scope = argc > 0 ? argv [0] : NULL;

    scope_block = lookup_data_scope_from_parameter (scope);
    if (! scope_block)
        return -1;

    if ((scope_block-> xml_item)
    &&  (xml_item_name (scope_block-> xml_item)))
      {
        result-> result_type = TYPE_STRING;
        result-> result_s    = mem_strdup (
                                   xml_item_name (scope_block-> xml_item));
      }
    else
        result-> result_type = TYPE_UNDEFINED;

    return 0;
}


static int
eval_defined (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    SCRIPT_NODE
        *expr = argc > 0 ? argv [0] : NULL;

    if (evaluate_script_node (expr) != 0)
        return -1;

    result-> result_n = (expr-> result_type != TYPE_UNDEFINED) ? 1 : 0;
    result-> result_type = TYPE_NUMBER;
    return 0;
}


static int
eval_deleted (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    SCOPE_BLOCK
        *scope_block;
    SCRIPT_NODE
        *scope = argc > 0 ? argv [0] : NULL;

    scope_block = lookup_data_scope_from_parameter (scope);
    if (! scope_block)
        return -1;

    result-> result_n = (scope_block-> xml_item == NULL) ? 1 : 0;
    result-> result_type = TYPE_NUMBER;
    return 0;
}


static int
eval_macro (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    SCRIPT_NODE
        *name = argc > 0 ? argv [0] : NULL;

    if (evaluate_script_node (name) != 0)
        return -1;

    result-> result_type = TYPE_NUMBER;
    if (sym_lookup_symbol (macros, string_result (name)))
        result-> result_n = 1;
    else
        result-> result_n = 0;

    return 0;
}
