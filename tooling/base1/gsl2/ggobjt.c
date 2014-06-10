/*===========================================================================*
 *                                                                           *
 *  ggobjt.c - Object module                                                 *
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
#include "ggpars.h"                     /*  Script parser header file        */
#include "ggobjt.h"                     /*  Object Module Header             */
#include "ggeval.h"                     /*  Expression Evaluator Header      */

/*- Types -------------------------------------------------------------------*/

typedef struct _GSL_OBJECT GSL_OBJECT;

struct _GSL_OBJECT {
    GSL_OBJECT   *next,
                 *prev;
    char         *name;
    GSL_FUNCTION *function;
    int           size;
};

/*- Global variables --------------------------------------------------------*/

LIST
    object_list;

/*- Function prototypes -----------------------------------------------------*/

static int  build_method_arguments   (SCRIPT_NODE *fn_node, 
                                      SCRIPT_NODE ***arg);

/*- Functions ---------------------------------------------------------------*/

void
initialise_objects (void)
{
    list_reset (& object_list);
}

void
destroy_objects (void)
{
    GSL_OBJECT
        *object = object_list. next;

    while ((void *) object != & object_list)
      {
        mem_free (object-> name);
        list_unlink (object);
        mem_free (object);
        object = object_list. next;
      }
}

int
object_register (char *name, GSL_FUNCTION *gsl_functions, int size)
{
    GSL_OBJECT
        *object;
    int
        i;

    list_create (object, sizeof (GSL_OBJECT));
    ASSERT (object);
    list_relink_after (object, & object_list);

    object-> name     = mem_strdup (name);
    object-> function = gsl_functions;
    object-> size     = size;

    for (i = 1; i < size; i++)
        if (strcmp (gsl_functions [i-1]. name, gsl_functions [i]. name) >= 0)
          {
            mem_free (object-> name);
            mem_free (object);
            return -1;
          }
    return 0;
}

int
method_evaluate (SCRIPT_NODE *node)
{
    int
        min = 0,
        max,
        chop = 0,
        cmp = -1,
        argc,
        rc;
    char
        *name = NULL;
    GSL_OBJECT
        *object;
    GSL_FUNCTION
        *function;
    SCRIPT_NODE
        **argv = NULL;

    if (node-> op1)
      {
        if (evaluate_script_node (node-> op1) != 0)
            return -1;
        name = string_result (node-> op1);

        FORLIST (object, object_list)
            if (object-> name ? streq (object-> name, name) : FALSE)
                break;
      }
    else
        FORLIST (object, object_list)
            if (! object-> name)
                break;

    if ((void *) object == & object_list)
      {
        gg_report_error ('E', "Unknown object %s.", name);
        return -1;
      }

    if (evaluate_script_node (node-> op2) != 0)
        return -1;
    name = string_result (node-> op2);

    max = object-> size;

    /*  Check for case where object has one function to handle all methods  */
    if ((max == 1)
    &&  (! object-> function [chop]. name))
      {
        chop = 0;
        cmp  = 0;
      }
    else
      {
        chop = (max + min) / 2;
        cmp = strcmp (object-> function [chop]. name, name);
        while (max > min)
          {
            if (cmp < 0)
                min = chop + 1;
            else if (cmp > 0)
                max = chop;
            else
                break;

            chop = (max + min) / 2;
            cmp = strcmp (object-> function [chop]. name, name);
          }
      }
    if (cmp == 0)
      {
        function = & object-> function [chop];
        argc = build_method_arguments (node-> op3, &argv);

        if ((argc >= function-> min_params)
        &&  (argc <= function-> max_params))
            rc = function-> evaluate (argc, argv, node);
        else
          {
            gg_report_error ('E', "Incorrect number of arguments.");
            rc = -1;
          }
        if (argc > 0)
            mem_free (argv);
        return rc;
      }
    else
      {
        gg_report_error ('E', "Unknown function: %s", name);
        return -1;
      }
}

static int
build_method_arguments (SCRIPT_NODE *fn_node, SCRIPT_NODE ***arg)
{
    int
        count,
        n;
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

    if (count)
      {
        *arg = mem_alloc (sizeof (SCRIPT_NODE *) * count);
        (*arg) [0] = node;
        node = node-> parent;
        for (n = 1; n < count; n++)
          {
            (*arg) [n] = node-> op2;
            node = node-> parent;
          }
      }
    return count;
}


int
evaluate_arguments (int argc, SCRIPT_NODE **argv)
{
    int
        i;

    for (i = 0; i < argc; i++)
        if (evaluate_script_node (argv [i]) != 0)
            return -1;
    return 0;
}


Bool
arguments_are_defined (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    int
        i;

    for (i = 0; i < argc; i++)
        if (argv [i]-> result_type == TYPE_UNDEFINED)
          {
            result-> culprit = argv [i]-> culprit;
            return FALSE;
          }
    return TRUE;
}
