/*===========================================================================*
 *                                                                           *
 *  lroptn.h - Option parsing functions                                      *
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

#ifndef _OPTN_INCLUDED                  /*  Allow multiple inclusions        */
#define _OPTN_INCLUDED

/*  Type definitions                                                         */

typedef struct {                        /*  Command-line option              */
    char    *name;                      /*    name of option                 */
    char    type;                       /*    ?  Boolean on/off value        */
                                        /*    =  Single value                */
                                        /*    +  List of values              */
    byte    flags;                      /*    bit 0 = T/F                    */
    char   *value;                      /*    actual value if any            */
    char   *check;                      /*    check list of valid values     */
} option;

#define OPT_OFF                 0       /*  Bit masks for 'flags' field      */
#define OPT_ON                  1

/*  Function prototypes                                                      */

int    lr_parse_option          (char *option);
void   lr_parse_option_line     (char *line);
int    lr_parse_option_file     (char *filename);
void   lr_reset_options         (void);
void   lr_show_options          (char *filename);
void   lr_push_options          (void);
void   lr_pop_options           (void);

#endif                                  /*  Included                         */
