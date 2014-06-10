/*===========================================================================*
 *                                                                           *
 *  ggobjt.c - Object functions                                              *
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


/*- Types -------------------------------------------------------------------*/

typedef struct _GSL_OBJECT GSL_OBJECT;

struct _GSL_OBJECT {
    GSL_OBJECT   *next,
                 *prev;
    char         *name;
    GSL_FUNCTION *function;
    function     destructor;
    int           size;
};

/*- Global variables --------------------------------------------------------*/

char
    object_error [LINE_MAX + 1];

/*- Global variables used in this source file only --------------------------*/

static LIST
    object_list = {&object_list, &object_list};

/*- Function prototypes -----------------------------------------------------*/

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
        if (object-> destructor)
            (void) (object-> destructor) ();
        mem_free (object-> name);
        list_unlink (object);
        mem_free (object);
        object = object_list. next;
      }
}

int
object_register (char *name, GSL_FUNCTION *gsl_functions, int size, function destructor)
{
    GSL_OBJECT
        *object;
    int
        i;

    /*  First unsure function names are sorted correctly.  */
    for (i = 1; i < size; i++)
        if (strcmp (gsl_functions [i-1]. name, gsl_functions [i]. name) >= 0)
            return -1;

    /*  If object is already register, remove old registration.  */
    FORLIST (object, object_list)
        if (object-> name ? streq (object-> name, name) : FALSE)
            break;

    if ((void *) object == & object_list)        /*  ie wasn't found.  */
      {
        list_create (object, sizeof (GSL_OBJECT));
        ASSERT (object);
        list_relink_after (object, & object_list);

        object-> name = mem_strdup (name);
      }

    object-> function   = gsl_functions;
    object-> size       = size;
    object-> destructor = destructor;

    return 0;
}

GSL_FUNCTION *
locate_method (RESULT_NODE *node)
{
    int
        min = 0,
        max,
        chop = 0,
        cmp = -1;
    char
        *name = NULL;
    GSL_OBJECT
        *object;

    if (node-> scope)
      {
        name = string_value (& node-> scope-> value);

        FORLIST (object, object_list)
            if (object-> name ? streq (object-> name, name) : FALSE)
                break;
      }
    else
        FORLIST (object, object_list)
            if (! object-> name)
                break;

    if ((void *) object == & object_list)
        return NULL;

    name = string_value (& node-> name-> value);

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
        while (max > min)
          {
            chop = (max + min) / 2;
            cmp = strcmp (object-> function [chop]. name, name);

            if (cmp < 0)
                min = chop + 1;
            else if (cmp > 0)
                max = chop;
            else
                break;
          }
      }

    if ((cmp != 0) && (min < object-> size))
      {
        chop = (max + min) / 2;
        cmp = strcmp (object-> function [chop]. name, name);
      }
    if (cmp == 0)
        return & object-> function [chop];
    else
        return NULL;
}


int
build_method_arguments (SCRIPT_NODE *fn_node,
                        RESULT_NODE ***arg)
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
	*arg = mem_alloc (sizeof (RESULT_NODE) * count);
        for (n = 0; n < count; n++)
            (*arg) [n] = NULL;
      }
    return count;
}


Bool
arguments_are_defined (int argc, RESULT_NODE **argv, RESULT_NODE *result)
{
    int
        i;

    for (i = 0; i < argc; i++)
        if (argv [i]-> value. type == TYPE_UNDEFINED)
          {
            result-> culprit = argv [i]-> culprit;
            return FALSE;
          }
    return TRUE;
}
