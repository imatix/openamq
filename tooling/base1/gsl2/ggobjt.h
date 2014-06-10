/*===========================================================================*
 *                                                                           *
 *  ggobjt.h - Object module                                                 *
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

#ifndef GGOBJT_INCLUDED                 /*  Allow multiple inclusions        */
#define GGOBJT_INCLUDED

/*---------------------------------------------------------------------------*/

#include "sfl.h"                        /*  Universal include file           */
#include "ggcomm.h"                     /*  Common declarations              */
#include "ggobjt.h"                     /*  Evaluation functions             */
#include "ggpars.h"                     /*  Script parser header file        */

/*- Types -------------------------------------------------------------------*/

typedef int (EVAL_FUNCTION (int argc,
                            SCRIPT_NODE **argv, 
                            SCRIPT_NODE *result));

typedef struct {
    char
        *name;
    int
        min_params,
        max_params;
    EVAL_FUNCTION
        *evaluate;
} GSL_FUNCTION;


/*- Functions ---------------------------------------------------------------*/

void initialise_objects    (void);
void destroy_objects       (void);
int  object_register       (char *name, GSL_FUNCTION *gsl_functions, int size);
int  method_evaluate       (SCRIPT_NODE *node);
int  evaluate_arguments    (int argc, SCRIPT_NODE **argv);
Bool arguments_are_defined (int argc, SCRIPT_NODE **argv, SCRIPT_NODE *result);

/*---------------------------------------------------------------------------*/

#endif



