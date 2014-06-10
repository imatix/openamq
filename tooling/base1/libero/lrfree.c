/*===========================================================================*
 *                                                                           *
 *  lrfree.c - Memory clean-up functions                                     *
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

#include "prelude.h"                    /*  Public definitions               */
#include "lrpriv.h"                     /*  Private definitions              */


/*-----------------------------.
 |  lr_free_memory             |
 |-----------------------------`---------------------------------------------.
 |  int lr_free_memory (lrnode *head, lrstat *stats);                        |
 |                                                                           |
 |  Description:                                                             |
 |                                                                           |
 |                                                                           |
 |  Returns: 0 if okay, -1 if errors were found.                             |
 |                                                                           |
 |  Comments:                                                                |
 |                                                                           |
 `---------------------------------------------------------------------------*/

int lr_free_memory (lrnode *listhead, lrstat *stats)
{
    lrnode *state;                      /*  Pointer to state in dialog list  */

    int feedback = 0;

    ASSERT (stats-> mnames != NULL);
    ASSERT (stats-> enames != NULL);
    ASSERT (stats-> snames != NULL);
    ASSERT (stats-> source != NULL);

    free (stats-> mnames);              /*  Drop list of modules             */
    free (stats-> enames);              /*    and list of events             */
    free (stats-> snames);              /*    and list of states             */
    free (stats-> source);              /*    and source file name           */

    state = listhead-> child;           /*  Free main states list            */
    while (state)
        state = DeleteState (listhead, state);

    free (listhead-> name);             /*  Free symbol table                */
    return (feedback);
}
