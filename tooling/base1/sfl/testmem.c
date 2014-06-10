/*===========================================================================*
 *                                                                           *
 *  testmem.c - Test memory access functions                                 *
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

#include "sfl.h"

int main (int argc, char *argv [])
{
    MEMTRN
        *tr1, *tr2;
    void
        *a1, *a2, *a3, *a4;

    tr1 = mem_new_trans();
    tr2 = mem_new_trans();

    a1 = mem_alloc (1);
    a2 = memt_alloc (tr1, 2);
    a3 = memt_alloc (tr1, 3);
    a4 = memt_alloc (tr2, 4);
    mem_display (stdout);

    mem_commit (tr1);
    mem_rollback (tr2);
    mem_display (stdout);

    mem_free (a1);
    mem_free (a2);
    mem_free (a3);

    mem_assert ();
    return (EXIT_SUCCESS);
}
