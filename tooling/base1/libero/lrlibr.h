/*===========================================================================*
 *                                                                           *
 *  lrlibr.h - Private library functions                                     *
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

#ifndef _LIBR_INCLUDED                  /*  Allow multiple inclusions        */
#define _LIBR_INCLUDED

/*  Data types and definitions used by LIBERO programs                       */

struct _lrnode {                        /*  Node in state diagram            */
    char *name,                         /*  Item name in symbol table        */
         type;                          /*  Item type: s, e, m, or n         */
    dbyte number;                       /*  Item number                      */
    struct _lrnode
         *next,                         /*  Pointer to next sibling          */
         *child;                        /*  Pointer to first child           */
};

struct _lrstat {                        /*  Statistics about dialog          */
    int  states,                        /*    Number of states               */
         events,                        /*    Number of events               */
         modules,                       /*    Number of modules              */
         defaults,                      /*    Defaults state, if any         */
         maxaction;                     /*    Max actions for one event      */
    char **snames,                      /*    Table of state names           */
         **enames,                      /*    Table of event names           */
         **mnames,                      /*    Table of module names          */
         *source;                       /*    Name of source file            */
    Bool crlf;                          /*    File is in MS-DOS CRLF format  */
};

typedef struct _lrnode lrnode;          /*  Define data types for structs    */
typedef struct _lrstat lrstat;


/*  Function prototypes                                                      */

#ifdef __cplusplus
extern "C" {
#endif

Bool    ValidName           (char *name);
Bool    ReservedName        (char *name);
char   *TrueName            (char *name);
char   *CleanName           (char *name);
char   *ExternalName        (char *name);
void   *Check               (void *ptr);
dbyte   GetSymNumber        (char *name);
void    PutSymNumber        (char *name, dbyte number);
lrnode *NamedState          (lrnode *head, char *name);
lrnode *DeleteState         (lrnode *head, lrnode *state);

#ifdef __cplusplus
}
#endif


#endif                                  /*  Included                         */
