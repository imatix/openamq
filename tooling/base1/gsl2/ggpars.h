/*===========================================================================*
 *                                                                           *
 *  ggpars.h - Script parser                                                 *
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

#ifndef GGPARS_INCLUDED                 /*  Allow multiple inclusions        */
#define GGPARS_INCLUDED

/*---------------------------------------------------------------------------*/

#include "sfl.h"                        /*  Universal include file           */


/*- Type definitions --------------------------------------------------------*/

typedef struct _SCRIPT_NODE        SCRIPT_NODE;
typedef struct _SUBSTITUTE_OBJECT  SUBSTITUTE_OBJECT;

typedef enum { GG_COMMENT,
               GG_TEXT,
               GG_SUBSTITUTE,
               GG_OPERATOR,
               GG_LITERAL,
               GG_NUMBER,
               GG_SYMBOL,
               GG_MEMBER,
               GG_CALL,
               GG_OPERAND,
               GG_OUTPUT,
               GG_APPEND,
               GG_CLOSE,
               GG_IF,
               GG_ELSIF,
               GG_ELSE,
               GG_END_IF,
               GG_FOR,
               GG_END_FOR,
               GG_NEW,
               GG_END_NEW,
               GG_DELETE,
               GG_MOVE,
               GG_COPY,
               GG_WHILE,
               GG_END_WHILE,
               GG_NEXT,
               GG_LAST,
               GG_MACRO,
               GG_END_MACRO,
               GG_FUNCTION,
               GG_END_FUNCTION,
               GG_RETURN,
               GG_GSL,
               GG_DIRECT,
               GG_XML,
               GG_JAVASCRIPT,
               GG_SOURCE,
               GG_TEMPLATE,
               GG_END_TEMPLATE,
               GG_ECHO,
               GG_ABORT,
               GG_DEFINE,
               GG_SAVE,
               GG_SORT,
               GG_UNDEFINED }
    SCRIPT_NODE_TYPE;


typedef enum { OP_UNDEFINED,
               OP_TIMES,
               OP_DIVIDE,
               OP_PLUS,
               OP_MINUS,
               OP_DEFAULT,
               OP_EQUALS,
               OP_NOT_EQUALS,
               OP_GREATER_THAN,
               OP_LESS_THAN,
               OP_GREATER_EQUAL,
               OP_LESS_EQUAL,
               OP_SAFE_EQUALS,
               OP_SAFE_NOT_EQUALS,
               OP_SAFE_GREATER_THAN,
               OP_SAFE_LESS_THAN,
               OP_SAFE_GREATER_EQUAL,
               OP_SAFE_LESS_EQUAL,
               OP_NOT,
               OP_OR,
               OP_AND,
               OP_NEXT_ARG }
    OPERATOR;


typedef enum { TYPE_UNDEFINED,
               TYPE_STRING,
               TYPE_NUMBER,
               TYPE_OK }
    DATA_TYPE;

struct _SCRIPT_NODE {
    SCRIPT_NODE_TYPE
        type;
    SCRIPT_NODE
        *parent;
    int
        brackets;
    int  
        length,                         /*  Length of script construct       */
        spaces;                         /*  Leading spaces in script         */
    int
        indent,                         /*  Leading spaces after shuffling   */
        shuffle_in,                     /*  Shuffle count before this node   */
        shuffle_out;                    /*  Shuffle count after this node    */
    DATA_TYPE
        result_type;
    char
        *result_s;                      /*  String result when evaluated     */
    double
        result_n;                       /*  Numeric result when evaluated    */
    char 
        *text;                          /*  Text-only node                   */
    SCRIPT_NODE
        *op1,                           /*  Also name, literal value         */
        *op2,                           /*  Also scope of identifier         */
        *op3,                           /*  Also 'as' expr                   */
        *pretty,                        /*  Also 'where' expression          */
        *format,                        /*  also 'by' expression             */
        *culprit;                       /*  for undefined expression         */
    OPERATOR
        operator;
    Bool
        extend,
        line_break,
        constant;
};

/*  Function type for function to read next line of script                   */

typedef Bool (SCRIPT_READ) (char *text);


/*- Macros ------------------------------------------------------------------*/
            
#define init_script_node(n)                                                   \
    (n). type        = GG_UNDEFINED;                                          \
    (n). parent      = NULL;                                                  \
    (n). brackets    = 0;                                                     \
    (n). length      = 0;                                                     \
    (n). spaces      = 0;                                                     \
    (n). indent      = 0;                                                     \
    (n). shuffle_in  = 0;                                                     \
    (n). shuffle_out = 0;                                                     \
    (n). result_type = TYPE_UNDEFINED;                                        \
    (n). result_s    = NULL;                                                  \
    (n). result_n    = 0;                                                     \
    (n). text        = NULL;                                                  \
    (n). op1         = NULL;                                                  \
    (n). op2         = NULL;                                                  \
    (n). op3         = NULL;                                                  \
    (n). pretty      = NULL;                                                  \
    (n). format      = NULL;                                                  \
    (n). culprit     = NULL;                                                  \
    (n). operator    = OP_UNDEFINED;                                          \
    (n). extend      = FALSE;                                                 \
    (n). line_break  = FALSE;                                                 \
    (n). constant    = FALSE;

/*  Prototypes  */
void         gg_free             (SCRIPT_NODE *node);
void         gg_clean            (SCRIPT_NODE *node);
void         print_node_type     (FILE *stream, 
                                  int level, 
                                  SCRIPT_NODE_TYPE type);
void         gg_print            (FILE *stream, SCRIPT_NODE *node);
char        *operator_text       (OPERATOR op);
char        *gg_error            (void);
SCRIPT_NODE *gg_parse_template   (SCRIPT_READ *read);
SCRIPT_NODE *gg_parse_gsl        (SCRIPT_READ *read);
SCRIPT_NODE *gg_parse_expression (char *line);

/*---------------------------------------------------------------------------*/

#endif

