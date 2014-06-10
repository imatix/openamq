/*===========================================================================*
 *                                                                           *
 *  ggcomm.h - Common functions                                              *
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

#ifndef GGCOMM_INCLUDED                 /*  Allow multiple inclusions        */
#define GGCOMM_INCLUDED

/*---------------------------------------------------------------------------*/

#include "sfl.h"                        /*  Universal include file           */

/*- Macros ------------------------------------------------------------------*/

#define LONG_WIDTH 10
#define PATH       "PATH"


/*- Type definitions --------------------------------------------------------*/

typedef struct _SCRIPT_LINE SCRIPT_LINE;
typedef struct _SCRIPT_FILE SCRIPT_FILE;

struct _SCRIPT_FILE {
    SCRIPT_FILE  *next,
                 *prev;
    char         *name;
    FILE         *file;
    char         *text,
                 *ptr;
    LIST          line_head;
    SCRIPT_LINE  *cur_line;
    int           next_line;
    Bool          eof;
};

struct _SCRIPT_LINE {
    SCRIPT_LINE *next,
                *prev;
    SCRIPT_FILE *parent;
    SCRIPT_NODE *node;                  /*  GSL parse tree for GSL lines     */
    char        *text;                  /*  Text for non-GSL lines           */
    int          line;
};

typedef struct {
    void
        *next,
        *prev;
    XML_ITEM
        *xml_item;                      /*  Data for this item               */
    long
        item;                           /*  XML item number                  */
    DATA_TYPE
        result_type;                    /*  Result of evaluating sort key    */
    char
        *result_s;                      /*  String result when evaluated     */
    double
        result_n;                       /*  Numeric result when evaluated    */
} SCOPE_ITEM;

typedef struct _SCOPE_BLOCK {
    struct _SCOPE_BLOCK
        *next,
        *prev;
    char *name;                         /*  Scope name                       */
    LIST
        item_list;                      /*  List of items to iterate through */
    SCOPE_ITEM
        *scope_item;                    /*  Current item in for list         */
    XML_ITEM
        *xml_item;                      /*  Data for this item               */
    long
        index;                          /*  Loop index                       */
    Bool
        children;                       /*  Can structure have children?     */
} SCOPE_BLOCK;

typedef enum {
    CTL_UNDEFINED,
    CTL_IF,
    CTL_ELSE,
    CTL_FOR,
    CTL_WHILE,
    CTL_MACRO,
    CTL_FUNCTION,
    CTL_NEW,
    CTL_TEMPLATE
} CONTROL_TYPE;

typedef struct {
    CONTROL_TYPE  type;
    void         *extra;
    SCRIPT_LINE  *position;
} CONTROL_BLOCK;

typedef struct {
    SCRIPT_LINE *position;
    LIST         control_stack;
    int          template;
} SCRIPT_BLOCK;
    
typedef struct {                        /*  Store reference for macros       */
    char        *name;
    SCRIPT_NODE *args;
    SCRIPT_LINE *position;
    int          template;
} MACRO;


/*- Global variables --------------------------------------------------------*/

extern int
    shuffle,
    feedback,
    line_length;
extern SCRIPT_FILE
    *cur_script_file;
extern SCRIPT_BLOCK
    cur_script_block;
/* extern char */
/*     *me; */
extern LIST
    scope_stack,
    script_stack,
    script_list;
extern Bool
    ignorecase;
extern Bool
    stdout_echo;                        /*  Copy to stdout                   */
extern CONSOLE_FCT
    *stdout_fct;                        /*  Redirector function              */
extern SYMTAB
    *macros;


#if defined (JAVASCRIPT)
extern JSRuntime
    *rt;
extern JSContext
    *cx;
extern JSObject
    *obj;
#endif


/*- Functions ---------------------------------------------------------------*/

Bool         open_script_file           (char *name);
void         open_script_text           (char *name, char *text);
void         close_script               (void);
Bool         parse_next_script_line     (void);
Bool         script_read                (char *text);
Bool         read_next_script_line      (void);
SCRIPT_LINE *script_position            (void);
void         restore_position           (SCRIPT_LINE *position);
void         destroy_script_data        (void);

XML_ITEM    *extended_scope_xml         (SCRIPT_NODE *scope);
XML_ITEM    *lookup_from_xml            (SCRIPT_NODE *scope);
char        *extended_scope_string      (SCRIPT_NODE *scope);
char        *symbol_value               (SCRIPT_NODE *symbol);
char        *valueof                    (char *scope, char *name);
char        *compound_item_value        (XML_ITEM *item);
void         put_numeric_attr           (XML_ITEM *item,
                                         const char *name, const long value);
void         copy_result                (SCRIPT_NODE *dest,
                                         SCRIPT_NODE *source);

void         reset_scope_stack          (void);
SCOPE_BLOCK *create_scope_block         (const char *alias);
void         destroy_scope_block        (void);
SCOPE_BLOCK *lookup_scope_block         (char *name);
SCOPE_BLOCK *first_scope_block          (void);
SCOPE_BLOCK *last_scope_block           (void);

SCOPE_ITEM  *create_scope_item          (SCOPE_BLOCK *scope_block,
                                         XML_ITEM *xml_item,
                                         long item);
void         destroy_scope_item         (SCOPE_ITEM *scope_item);

Bool         first_scope_item           (SCOPE_BLOCK *scope_block);
Bool         next_scope_item            (SCOPE_BLOCK *scope_block);

void         reset_script_stack         (void);
void         push_script_block          (void);
void         pop_script_block           (void);

void         push_control               (CONTROL_TYPE type,
                                         void *extra);
void         pop_control                (CONTROL_BLOCK *control);

void         gg_report_error            (char type, char *format, ...);
char        *string_result              (SCRIPT_NODE *node);
double       number_result              (SCRIPT_NODE *node);
int          format_output              (char *buffer, size_t max,
                                         SCRIPT_NODE *node);
void         undefined_expression_error (SCRIPT_NODE *node);
int          collect_source_text        (char **buffer, char *terminator);

#if defined (JAVASCRIPT)
JSBool js_valueof (JSContext *cx, JSObject *obj,
                   uintN argc, jsval *argv, jsval *rval);
JSBool js_write   (JSContext *cx, JSObject *obj,
                   uintN argc, jsval *argv, jsval *rval);
void   js_error   (JSContext *cx, const char *message, JSErrorReport *report);
#endif

/*---------------------------------------------------------------------------*/

#endif
