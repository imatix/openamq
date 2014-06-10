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

/*- Macros ------------------------------------------------------------------*/

#define LONG_WIDTH 10
#define PATH       "PATH"

#define BLOCK_START_MARKER 1
#define BLOCK_END_MARKER   2
#define BLOCK_DONE_MARKER  3

#define init_result_node(n)                                                   \
    (n)-> next         = NULL;                                                \
    (n)-> parent       = NULL;                                                \
    (n)-> scope        = NULL;                                                \
    (n)-> name         = NULL;                                                \
    (n)-> op1          = NULL;                                                \
    (n)-> op2          = NULL;                                                \
    (n)-> as           = NULL;                                                \
    (n)-> to           = NULL;                                                \
    (n)-> before       = NULL;                                                \
    (n)-> after        = NULL;                                                \
    (n)-> script_node  = NULL;                                                \
    (n)-> operand      = NULL;                                                \
    (n)-> scope_block  = NULL;                                                \
    (n)-> macro        = NULL;                                                \
    (n)-> gsl_function = NULL;                                                \
    (n)-> argc         = 0;                                                   \
    (n)-> argv         = NULL;                                                \
    (n)-> culprit      = NULL;                                                \
    (n)-> indent       = 0;                                                   \
    (n)-> width        = 0;                                                   \
    (n)-> constant     = FALSE;                                               \
    init_value_node ((n)-> value);


#define copy_value(d,s) copy_value_ (NULL, d, s)

/*- Type definitions --------------------------------------------------------*/

typedef struct _SCOPE_BLOCK     SCOPE_BLOCK;
typedef struct _SCOPE_ITEM      SCOPE_ITEM;
typedef struct _SCOPE_STACK_REF SCOPE_STACK_REF;

struct _SCOPE_ITEM {
    SCOPE_ITEM
        *next,
        *prev;
    XML_ITEM
        *xml_item;                      /*  Data for this item               */
    long
        item;                           /*  XML item number                  */
    VALUE
        sort_key;
};

struct _SCOPE_BLOCK {
    SCOPE_BLOCK
        *next,
        *prev;
    SCRIPT_NODE_TYPE
        scope_type;
    char *name;                         /*  Scope name                       */
    LIST
        item_list;                      /*  List of items to iterate through */
    SCOPE_ITEM
        *scope_item;                    /*  Current item in for list         */
    XML_ITEM
        *xml_item;                      /*  Data for this item               */
    long
        total,                          /*  Total number of items in loop    */
        index;                          /*  Loop index                       */
    Bool
        stacked;                        /*  Is scope 'stacked'?              */
};

struct _SCOPE_STACK_REF {
    SCOPE_STACK_REF
        *next,
        *prev;
    LIST
        *scope_stack;
};

typedef enum {
    PARM_VALUE,
    PARM_REFERENCE,
    PARM_SIMPLE_SCOPE,
    PARM_EXTENDED_SCOPE,
    PARM_EXPRESSION
} PARM_TYPE;

typedef
    PARM_TYPE PARM_LIST [];

typedef struct _RESULT_NODE RESULT_NODE;

typedef int EVAL_FUNCTION (int argc,
                           RESULT_NODE **argv, 
                           RESULT_NODE *result,
                           THREAD *gsl_thread);

typedef struct {
    char
       *name;
    int
        min_parmc,
        max_parmc,
        cnt_parmt;
    PARM_LIST
       *parmt;
    Bool
        immediate;
    EVAL_FUNCTION
       *evaluate;
} GSL_FUNCTION;

struct _RESULT_NODE {
    RESULT_NODE
        *next,                          /*  In stack or cache                */
        *parent,
        *scope,
        *name,
        *op1,
        *op2,
        *as,
        *to,
        *before,
        *after,
        *operand;                       /*  Result of evaluating operand     */
    SCRIPT_NODE
        *script_node;                   /*  Corresponding parse node         */
    SCOPE_BLOCK
        *scope_block;
    SCRIPT_MACRO
        *macro;
    GSL_FUNCTION
        *gsl_function;
    int
        argc;
    RESULT_NODE
      **argv;
    char
        *culprit;
    int
        indent,                         /*  Leading spaces after shuffling   */
        width,                          /*  Width of last line of result     */
        item_nbr;
    VALUE          
        value;                          /*  Result                           */
    Bool
        constant;                       /*  Is result constant?              */
};


/*- Global variables --------------------------------------------------------*/

extern LIST
    all_scopes;

/*- Functions ---------------------------------------------------------------*/

SCOPE_BLOCK *lookup_simple_scope        (LIST  *scope_stack,
                                         VALUE *scope_value,
                                         Bool  ignorecase,
                                         char  **error_text);
char *       name_the_symbol            (RESULT_NODE *node);
void         undefined_expression_error (RESULT_NODE *node,
                                         char **error_text);
XML_ITEM    *extended_scope_xml         (LIST *scope_stack,
                                         RESULT_NODE *scope,
                                         Bool ignorecase,
                                         char **error_text);
char        *extended_scope_string      (RESULT_NODE *scope);
XML_ITEM *  symbol_xml_parent           (LIST *scope_stack, 
                                         RESULT_NODE *symbol, 
                                         Bool ignorecase,   
                                         char **error_text);
char        *symbol_value               (LIST *scope_stack,
                                         RESULT_NODE *symbol,
                                         Bool ignorecase,
                                         char **error_text);
char        *compound_item_value        (XML_ITEM *item);
Bool         store_symbol_definition    (LIST  *scope_stack,
                                         Bool  ignorecase,
                                         RESULT_NODE *symbol,
                                         VALUE *value,
                                         char **error_text);
void         put_numeric_attr           (XML_ITEM *item,
                                         const char *name, long value);
void         put_char_attr              (XML_ITEM *item,
                                         const char *name, char value);
void         copy_value_                (MEMTRN *memtrn,
                                         VALUE  *dest,
                                         VALUE  *source);
void         copy_result                (RESULT_NODE *dest,
                                         RESULT_NODE *source);
RESULT_NODE *new_result_node            (void);
void         destroy_result             (RESULT_NODE *node);

void         remove_scope_references_to_xml
                                        (XML_ITEM *xml_delete);

SCOPE_BLOCK *create_scope_block         (LIST *scope_stack,
                                         SCRIPT_NODE_TYPE scope_type,
                                         const char *alias);
void         destroy_scope_block        (LIST *scope_stack);
SCOPE_BLOCK *first_scope_block          (LIST *scope_stack);
SCOPE_BLOCK *last_scope_block           (LIST *scope_stack);

SCOPE_ITEM  *create_scope_item          (SCOPE_BLOCK *scope_block,
                                         XML_ITEM *xml_item,
                                         long item);
void         destroy_scope_item         (SCOPE_ITEM *scope_item);

Bool         first_scope_item           (SCOPE_BLOCK *scope_block);
Bool         next_scope_item            (SCOPE_BLOCK *scope_block);

void         copy_scope_stack           (LIST *to,
                                         LIST *from);

char        *string_value               (VALUE *value);
double       number_value               (VALUE *value);
int          format_output              (char *buffer, size_t max,
                                         RESULT_NODE *node,
                                         char **error_text);
int          pretty_print               (VALUE *result,
                                         RESULT_NODE *pretty,
                                         char *example,
                                         int  space,
                                         char **error_text);
size_t       strllen                    (const char *s);
char        *concatenate_results        (RESULT_NODE *r,
                                         int shuffle,
                                         Bool convert_indent);
void         destroy_caches             (void);

/*---------------------------------------------------------------------------*/

#endif
