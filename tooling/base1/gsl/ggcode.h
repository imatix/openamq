/*===========================================================================*
 *                                                                           *
 *  ggcode.h - Code generator functions                                      *
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

#ifndef GGCODE_INCLUDED                /*  Allow multiple inclusions        */
#define GGCODE_INCLUDED


/*---------------------------------------------------------------------------*/

typedef void (OUTPUT_FCT) (qbyte, const char *);
typedef void (HANDLER_FCT) (void);

typedef struct                          /*  Thread context block:            */
{
    THREAD
        *thread;
    MEMTRN
        *scratch_memtrn;
    void *
        job;
    XML_ITEM
        *xml_gsl,                       /*  GSL internal data                */
        *xml_switches,                  /*  Flat switches from caller        */
        **xml_root,                     /*  Array of initial XML items       */
        *xml_junk;                      /*  Parent for various XML items     */
    int
        xml_count;                      /*  Number of initial XML items      */
    QUEUE
        *replyq;
    int
        output_line,                    /*  Line number in output file       */
        last_line,                      /*  Most recently written line       */
        shuffle,                        /*  How to shuffle blank space       */
        execute_level;
    char
        *script_name,
        *script_text,
        *output_name,
        *me,
        *terminator;                    /*  Line terminator                  */
    FILE
        *output;
    SCRIPT_NODE
        *script_root,
        *script_node,
        *evaluate_node,
        *operand_node,
        *fake_for_node;
    RESULT_NODE
       **result_ptr,
        *result_root,
        *result_node,
       *output_buffer;
    RESULT_NODE
        call_result;
    Bool
        template,
        ignorecase,
        stdout_echo,                    /*  Copy to stdout                   */
        execute_full,
        robust,                         /*  Recover from bad XML input?      */
        stepping,
        error_occurred;
    SCRIPT_SOURCE
        *script_source;
    SCRIPT_LINE
        *script_line;
    LIST
        scope_stack,
        script_stack;
    RESULT_NODE
        *node_stack;
    SCOPE_STACK_REF
        *scope_stack_ref;
    OUTPUT_FCT
        *output_fct;                    /*  Redirector function              */
    SYMTAB
        *macros;
    DATA_TYPE
        sort_type;
} GGCODE_TCB;


/*- Function prototypes -----------------------------------------------------*/

int     gsl_init         (long size);

THREAD *gsl_execute      (QUEUE    *replyqueue,
                          qbyte     job,
                          XML_ITEM *switches,
                          int       count,
                          /* XML_ITEM *xml */ ...);
THREAD *gsl_start        (QUEUE    *replyqueue,
                          qbyte     job,
                          XML_ITEM *switches,
                          int       count,
                          /* XML_ITEM *xml */ ...);
void    gsl_continue     (THREAD   *gsl_thread,
                          QUEUE    *replyqueue);
void    gsl_next         (THREAD   *gsl_thread,
                          QUEUE    *replyqueue);
void    gsl_command      (THREAD   *gsl_thread,
                          char     *command,
                          Bool      terminate,
                          QUEUE    *replyqueue);
int     gsl_function     (THREAD   *gsl_thread,
                          QUEUE    *replyqueue,
                          char     *function,
                          int       parm_count,
                          char     *parm_value[]);
THREAD *gsl_spawn        (THREAD   *gsl_thread,
                          char     *command);
void    gsl_finish       (THREAD   *gsl_thread);

char   *gsl_cur_script   (THREAD *gsl_thread);
int     gsl_cur_line     (THREAD *gsl_thread);
char   *gsl_cur_text     (THREAD *gsl_thread);

int  gsl_term         (void);

void gg_send_output   (THREAD *gsl_thread, OUTPUT_FCT *output_fct, Bool echo);
void gg_set_handler   (HANDLER_FCT *handler_fct, int event);

/*  These are the events that we can provide handlers for                    */

typedef enum {
    EVENT_ABORT
} GG_EVENT;


/*---------------------------------------------------------------------------*/

#endif
