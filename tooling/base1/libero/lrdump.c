/*===========================================================================*
 *                                                                           *
 *  lrdump.c - Debugging functions                                           *
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
 |  lr_dump_dialog             |
 |-----------------------------`---------------------------------------------.
 |  int lr_dump_dialog (lrnode *head, lrstat *stats);                        |
 |                                                                           |
 |  Description: dumps various internal tables.  Each of the various dump    |
 |  routines can be individually switched on or off by changing the #if's.   |
 `---------------------------------------------------------------------------*/

void lr_dump_dialog (lrnode *listhead, lrstat *stats)
{
    lrnode *state, *event, *module;     /*  Pointers into dialog list        */
    int    item;                        /*  Number of item to display        */
    char   *typeptr, *nameptr;          /*  Pointers into symbol table       */
    byte   *numbptr;

    ASSERT (listhead != NULL);
    ASSERT (stats != NULL);

    /*  Dump symbol table                                                    */
    Trace ("\nSYMBOL TABLE:");
    Trace ("----------------------");
    for (typeptr = listhead-> name; *typeptr; )
      {
        numbptr = (byte *) typeptr + sizeof (char);
        nameptr = (char *) numbptr + sizeof (dbyte);
        Trace ("%c %3d %s", *typeptr, (*numbptr << 8) + *(numbptr + 1),
                                                              nameptr);
        typeptr = strchr (nameptr, 0) + 1;
      }

    /*  Dump state/event/module names and numbers                            */
    Trace ("\nSTATES:");
    Trace ("----------------------");
    for (item = 0; item < stats-> states; item++)
        Trace ("%3d  %s", item, stats-> snames [item]);

    Trace ("\nEVENTS:");
    Trace ("----------------------");
    for (item = 0; item < stats-> events; item++)
        Trace ("%3d  %s", item, stats-> enames [item]);

    Trace ("\nMODULES:");
    Trace ("----------------------");
    for (item = 0; item < stats-> modules; item++)
        Trace ("%3d  %s", item, stats-> mnames [item]);

    /*  Display state diagram                                                */
    Trace ("\nSTATE DIAGRAM:");
    Trace ("----------------------");
    for (state = listhead-> child; state; state = state-> next)
      {
        Trace ("%3d %s:", state-> number, state-> name);
        for (event = state-> child; event; event = event-> next)
          {
            module = event-> child;
            Trace ("    (%02d) %-40s -> %3d %s",
                   event-> number,  event-> name,
                   module-> number, module-> name);

            for (module = module-> next; module; module = module-> next)
                Trace ("        %3d + %s", module-> number, module-> name);
          }
        Trace ("");
      }
}
