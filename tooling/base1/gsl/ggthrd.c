/*===========================================================================*
 *                                                                           *
 *  ggthrd.c - Thread object functions                                       *
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
#include "ggthrd.h"                     /*  Include header file              */


/*- Definitions -------------------------------------------------------------*/

#define AGENT_NAME      "GGTHRD"        /*  Our public name                  */
#define NAME "thread"

/*- Function prototypes -----------------------------------------------------*/

static EVAL_FUNCTION
    eval_create;

/*- Type definitions --------------------------------------------------------*/

/*- Global variables used in this source file only --------------------------*/

static PARM_LIST
    one_value_parm_list = {PARM_VALUE};

static GSL_FUNCTION thread_functions [] = 
{
    /*  Don't forget to keep these in alphaetical order!                     */
    {"create",  1, 1, 1, (void *) &one_value_parm_list, TRUE, eval_create}
};

#define ERROR_MSG_LENGTH 80

static char
    error_msg [ERROR_MSG_LENGTH + 1];

/*- Functions ---------------------------------------------------------------*/

int register_thread_functions (void)
{
    error_msg [0] = '\0';

    return object_register (NAME, thread_functions, tblsize (thread_functions), NULL);
}


static int
eval_create (int argc,
             RESULT_NODE **argv,
             RESULT_NODE *result,
             THREAD *dummy)
{
    RESULT_NODE
        *gsl = argc > 0 ? argv [0] : NULL;
    THREAD
        *thread;

    if (! arguments_are_defined (argc, argv, result))
        return 0;
    thread = gsl_spawn (NULL,   /*  Spawn as child of current GSL thread.  */
                        string_value (& gsl-> value));
    if (! thread)
      {
        strcpy (object_error, "Error starting thread.");
        return -1;
      }
    result-> value. type = TYPE_NUMBER;
    result-> value. n    = (long) thread;
    
    return 0;
}
