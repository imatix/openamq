/*===========================================================================*
 *                                                                           *
 *  ggstrn.c - String functions                                              *
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
#include "ggstrn.h"                     /*  Include header file              */


/*- Macros ------------------------------------------------------------------*/

#define NAME "string"

/*- Function prototypes -----------------------------------------------------*/

static EVAL_FUNCTION
    eval_length,
    eval_substr,
    eval_trim,
    eval_justify;

static Bool node_is_countable           (int argn, SCRIPT_NODE **argv, 
                                         char *function, SCRIPT_NODE *result);

/*- Global variables --------------------------------------------------------*/

static GSL_FUNCTION gsl_functions [] = 
{
    {"justify",   3, 3, eval_justify},
    {"length",    1, 1, eval_length},
    {"substr",    2, 4, eval_substr},
    {"trim",      1, 1, eval_trim}
};

/*- Functions ---------------------------------------------------------------*/

int register_string_functions (void)
{
    return object_register (NAME, gsl_functions, tblsize (gsl_functions));
}


static int
eval_length (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    if (evaluate_arguments (argc, argv) != 0)
        return -1;

    if (! arguments_are_defined (argc, argv, result))
        return 0;

    result-> result_type = TYPE_NUMBER;
    result-> result_n    = strlen (string_result (argv [0]));
    return 0;
}


static int
eval_substr (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    SCRIPT_NODE
        *string = argc > 0 ? argv [0] : NULL,
        *start  = argc > 1 ? argv [1] : NULL,
        *end    = argc > 2 ? argv [2] : NULL,
        *len    = argc > 3 ? argv [3] : NULL;
    long
        start_n = 0,
        end_n   = 0,
        len_n   = 0;

    if (evaluate_arguments (argc, argv) != 0)
        return -1;

    if (string-> result_type == TYPE_UNDEFINED)
      {
        result-> culprit = string-> culprit;
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
        if (node_is_countable (1, argv, "substr", result))
            start_n = (long) start-> result_n;
        else
            return -1;
      }
    if (end)
      {
        if (node_is_countable (2, argv, "substr", result))
            end_n = (long) end-> result_n;
        else
            return -1;
      }
    if (len)
      {
        if (node_is_countable (3, argv, "substr", result))
            len_n = (long) len-> result_n;
        else
            return -1;
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
        if (start_n < 0)
            start_n = 0;

        len_n = end_n - start_n + 1;
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
    if (start_n >= (long) strlen (string-> result_s))
        result-> result_s = mem_strdup ("");
    else
      {
        result-> result_s = mem_alloc (len_n + 1);
        if (start_n >= 0)
            strncpy (result-> result_s, & string-> result_s [start_n], len_n);
        else
            strncpy (result-> result_s, string-> result_s, len_n);
        
        (result-> result_s) [len_n] = '\0';
      }
    result-> result_type = TYPE_STRING;

    return 0;
}


static Bool
node_is_countable (int argn, SCRIPT_NODE **argv, 
                   char *function, SCRIPT_NODE *result)
{
    if (argv [argn])
      {
        if (argv [argn]-> result_type != TYPE_UNDEFINED)
          {
            number_result (argv [argn]);
            if ((argv [argn]-> result_type == TYPE_NUMBER)
            &&  (argv [argn]-> result_n == floor (argv [argn]-> result_n))
            &&  (argv [argn]-> result_n >= 0))
                return TRUE;
            else
                gg_report_error ('E', "Illegal argument %u to function %s.", 
                                      argn + 1, function);
          }
        else
            result-> culprit = argv [argn]-> culprit;
      }
    
    return FALSE;
}


static int
eval_trim (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    if (evaluate_arguments (argc, argv) != 0)
        return -1;

    if (! arguments_are_defined (argc, argv, result))
        return 0;

    result-> result_type = argv [0]-> result_type;
    if (argv [0]-> result_s)
      {
        result-> result_s = mem_strdup (strcrop (argv [0]-> result_s));
        ASSERT (result-> result_s);
      }
    result-> result_n = argv [0]-> result_n;
    return 0;
}


static int
eval_justify (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result)
{
    SCRIPT_NODE
        *string =            argv [0],
        *width  =            argv [1],
        *prefix = argc > 2 ? argv [2] : NULL;
    unsigned long
        width_n;

    if (evaluate_arguments (argc, argv) != 0)
        return -1;

    if (! arguments_are_defined (argc, argv, result))
        return 0;

    if (node_is_countable (1, argv, "justify", result))
        width_n = (unsigned long) width-> result_n;
    else
        return -1;

    result-> result_s = 
                          strreformat (string_result (string),
                                       width_n,
                                       prefix
                                         ? string_result (prefix) : NULL);
    result-> result_type = TYPE_STRING;
    return 0;
}
