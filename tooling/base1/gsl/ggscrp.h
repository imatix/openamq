/*===========================================================================*
 *                                                                           *
 *  ggscrp.h - Script execution functions                                    *
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

#ifndef GGSCRP_INCLUDED                 /*  Allow multiple inclusions        */
#define GGSCRP_INCLUDED


/*- Macros ------------------------------------------------------------------*/

#define PATH       "PATH"

/*- Type definitions --------------------------------------------------------*/

typedef struct _SCRIPT_LINE   SCRIPT_LINE;
typedef struct _SCRIPT_SOURCE SCRIPT_SOURCE;

struct _SCRIPT_SOURCE {
    SCRIPT_SOURCE *next,
                  *prev;
    MEMTRN        *memtrn;              /*  Transaction holding this script  */
    char          *name;
    Bool           keep;
    long           size;
    time_t         last_closed;
    time_t         timestamp;
    LIST           line_head;
    int            links:16,
                   macros:16;
};

struct _SCRIPT_LINE {
    SCRIPT_LINE   *next,
                  *prev,
                  *loop_start,          /*  Start of loop block              */
                  *block_end;           /*  End of control block             */
    SCRIPT_SOURCE *parent;
    SCRIPT_NODE   *node;                /*  GSL parse tree for GSL lines     */
    char          *text;                /*  Text for non-GSL lines           */
    word           line;
    int            template:1;
    int            macros:16;
};

typedef struct {                        /*  Store reference for macros       */
    char        *name;
    SCRIPT_NODE *args;
    SCRIPT_LINE *position;
} SCRIPT_MACRO;


/*- Functions ---------------------------------------------------------------*/

int            ggscrp_init                (long size);
int            ggscrp_term                (void);
void           ggscrp_set_size            (long size);
int            load_script_file           (char *name,
                                           Bool template,
                                           Bool debug,
                                           Bool keep,
                                           void *job,
                                           QUEUE *replyqueue);
int            load_script_text           (char *name,
                                           char *text,
                                           Bool template,
                                           Bool debug,
                                           void *job,
                                           QUEUE *replyqueue);
void           close_script               (SCRIPT_SOURCE *script_source);

SCRIPT_LINE   *read_next_script_line      (SCRIPT_LINE   *script_line);

/*---------------------------------------------------------------------------*/

#endif
