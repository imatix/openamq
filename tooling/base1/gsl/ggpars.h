/*===========================================================================*
 *                                                                           *
 *  ggpars.h - Script parser functions                                       *
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


/*- Type definitions --------------------------------------------------------*/

typedef struct _SCRIPT_NODE        SCRIPT_NODE;

typedef enum { GG_COMMENT,
               GG_LINE,
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
               GG_SCOPE,
               GG_END_SCOPE,
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
               TYPE_BLOCK,
               TYPE_UNKNOWN }
    DATA_TYPE;

typedef struct {
    DATA_TYPE
        type;
    char
       *s;                              /*  String value                     */
    double
        n;                              /*  Numeric value                    */
    char
      **b;                              /*  Block value                      */
} VALUE;

struct _SCRIPT_NODE {
    SCRIPT_NODE
        *parent;
    SCRIPT_NODE
        *scope,
        *name,
        *op1,
        *op2,
        *as,
        *to,
        *before,
        *after;
    VALUE          
        result;                         /*  Only used if result is constant  */
    int  
        width:16,                       /*  Width of script construct        */
        spaces:16,                      /*  Leading spaces in script         */
        line_break:16,                  /*  CRs following text               */
        brackets:16;
    OPERATOR
        operator:8;
    SCRIPT_NODE_TYPE
        type:8;
    int
        extend:1,
        constant:1,                     /*  Is the result constant?          */
        stacked:1,                      /*  Is scope to be 'stacked'?        */
        dynamic:1;                      /*  TRUE if job == 0.                */
};


/*  Function type for function to read next line of script                   */

typedef Bool (SCRIPT_READ) (void * job, char *text);


/*- Macros ------------------------------------------------------------------*/
            
#define init_script_node(n)                                                   \
    (n)-> type         = GG_UNDEFINED;                                        \
    (n)-> parent       = NULL;                                                \
    (n)-> brackets     = 0;                                                   \
    (n)-> width        = 0;                                                   \
    (n)-> spaces       = 0;                                                   \
    (n)-> scope        = NULL;                                                \
    (n)-> name         = NULL;                                                \
    (n)-> op1          = NULL;                                                \
    (n)-> op2          = NULL;                                                \
    (n)-> as           = NULL;                                                \
    (n)-> to           = NULL;                                                \
    (n)-> before       = NULL;                                                \
    (n)-> after        = NULL;                                                \
    (n)-> operator     = OP_UNDEFINED;                                        \
    (n)-> extend       = FALSE;                                               \
    (n)-> line_break   = 0;                                                   \
    (n)-> constant     = FALSE;                                               \
    (n)-> stacked      = FALSE;                                               \
    (n)-> dynamic      = FALSE;                                               \
    (n)-> result. type = TYPE_UNDEFINED;                                      \
    (n)-> result. s    = NULL;                                                \
    (n)-> result. b    = NULL;

#define init_value_node(n)                                                    \
    (n). type         = TYPE_UNDEFINED;                                       \
    (n). s            = NULL;                                                 \
    (n). b            = NULL;

/*  Prototypes  */
void         gg_free             (SCRIPT_NODE *node);
XML_ITEM    *gg_xml              (SCRIPT_NODE *node);
char        *operator_text       (OPERATOR op);

int          ggpars_init         (void);
int          gg_parse_template   (SCRIPT_READ *read,
                                  void        *job,
                                  MEMTRN      *memtrn,
                                  QUEUE       *replyqueue);
int          gg_parse_gsl        (SCRIPT_READ *read,
                                  void        *job,
                                  MEMTRN      *memtrn,
                                  QUEUE       *replyqueue);
int          gg_parse_expression (char        *expression,
                                  void        *job,
                                  MEMTRN      *memtrn,
                                  QUEUE       *replyqueue);
int          ggpars_term         (void);

/*---------------------------------------------------------------------------*/

#endif
