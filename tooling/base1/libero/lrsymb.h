/*===========================================================================*
 *                                                                           *
 *  lrsymb.h - Symbol table management functions                             *
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

#ifndef _LRSYMB_INCLUDED                /*  Allow multiple inclusions        */
#define _LRSYMB_INCLUDED

#define SYM_TYPE_INT        1           /*  Indicates integer symbol         */
#define SYM_TYPE_STRING     2           /*  Indicates string symbol          */
#define SYM_TYPE_EITHER     3           /*  Indicates flexible type          */

#define SYM_FLAG_RDONLY     1           /*  Indicates read-only variable     */
#define SYM_FLAG_FOUND      2           /*  Indicates found in source        */

#define SYM_SIGNATURE       12345       /*  Indicates valid symbol           */
#define SYM_HASH_SIZE       256         /*  If you change this, also change  */
                                        /*  sym_hash ()                      */
#define ASSERT_SYM(s)       ASSERT((s)-> signature == SYM_SIGNATURE)

typedef struct symbol_tag {
    struct  symbol_tag
           *next, *prev,                /*  Next/prev symbol in scope        */
           *h_next, *h_prev;            /*  Next/prev symb in bucket         */
    char   *name;                       /*  Copy of name                     */
    long    ivalue;                     /*  Long integer value               */
    char   *svalue;                     /*  String value, or null            */
    byte    type;                       /*  SYM_TYPE_...                     */
    byte    flags;                      /*  SYM_FLAG_...                     */
    byte    hash_value;                 /*  Hash bucket #                    */
    int     signature;                  /*  Indicates valid symbol           */
} lr_symbol;

typedef struct {
    lr_symbol *syms;
    lr_symbol *hash [SYM_HASH_SIZE];
    int      size;
} lr_sym_tab;

typedef int  (*lr_sym_func)(lr_symbol *,...);

lr_sym_tab *lr_sym_create_table (void);
void        lr_sym_delete_table (lr_sym_tab *);
lr_symbol  *lr_sym_lookup       (const lr_sym_tab *, const char *);
lr_symbol  *lr_sym_intern       (lr_sym_tab *, const char *, const int);
lr_symbol  *lr_sym_create       (lr_sym_tab *, const char *, const int);
lr_symbol  *lr_sym_delete       (lr_sym_tab *, lr_symbol *);
void        lr_sym_values       (lr_symbol *, const long ivalue, const char *svalue);
void        lr_sym_exec_all     (const lr_sym_tab *, const lr_sym_func,...);

#endif                                  /*  Include LRSYMB.H                 */
