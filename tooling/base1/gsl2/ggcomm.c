/*===========================================================================*
 *                                                                           *
 *  ggcomm.c - Common functions                                              *
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

#include "sfl.h"                        /*  Universal include file           */
#if defined (JAVASCRIPT)
#include "ggjs.h"                       /*  Support for Javascript           */
#endif
#include "ggpars.h"                     /*  Script parser header file        */
#include "ggcomm.h"                     /*  Header file                      */

/*- Global variables --------------------------------------------------------*/

int
    shuffle,
    feedback,
    line_length;
SCRIPT_FILE
    *cur_script_file = NULL;
SCRIPT_BLOCK
    cur_script_block = {NULL, {& cur_script_block. control_stack, 
                               & cur_script_block. control_stack}, 1};
LIST
    scope_stack,
    script_stack,
    script_list = {& script_list, & script_list};  /*  Empty list  */
Bool
    ignorecase = TRUE;
Bool
    stdout_echo = TRUE;                 /*  Copy to stdout                   */
CONSOLE_FCT
    *stdout_fct = NULL;                 /*  Redirector function              */
SYMTAB
    *macros;

#if defined (JAVASCRIPT)
JSRuntime
    *rt;
JSContext
    *cx;
JSObject
    *obj;
#endif

/*- Prototypes --------------------------------------------------------------*/


/*- Script Functions --------------------------------------------------------*/

Bool
open_script_file (char *name)
{
    SCRIPT_FILE
        *script_file;

    cur_script_file = NULL;
    FORLIST (script_file, script_list)
        if (streq (script_file-> name, name))
          {
            cur_script_file = script_file;
            break;
          }

    if (! cur_script_file)
      {
        /*  Open unopened script file  */
        list_create (script_file, sizeof (SCRIPT_FILE));
        ASSERT (script_file);
        script_file-> file = file_locate (PATH, name, "gsl");
        if (!script_file-> file)
          {
            mem_free (script_file);
            return FALSE;
          }
        
        script_file-> name      = mem_strdup (name);
        script_file-> text      = NULL;
        script_file-> ptr       = NULL;
        script_file-> next_line = 1;
        script_file-> eof       = FALSE;
        list_reset (& script_file-> line_head);

        list_relink_after (script_file, & script_list);
        cur_script_file = script_file;
      }

    cur_script_file-> cur_line = (SCRIPT_LINE *) & cur_script_file-> line_head;
    return TRUE;
}


void
open_script_text (char *name, char *text)
{
    SCRIPT_FILE
        *script_file;

    list_create (script_file, sizeof (SCRIPT_FILE));
    ASSERT (script_file);

    script_file-> file      = NULL;
    script_file-> name      = mem_strdup (name);
    script_file-> text      = mem_strdup (text);
    script_file-> ptr       = script_file-> text;
    script_file-> next_line = 1;
    script_file-> eof       = FALSE;
    list_reset (& script_file-> line_head);

    list_relink_after (script_file, & script_list);

    cur_script_file = script_file;
    cur_script_file-> cur_line = (SCRIPT_LINE *) & cur_script_file-> line_head;
}


void
close_script (void)
{
    if (cur_script_file-> text)
        mem_strfree (& cur_script_file-> text);

    if (cur_script_file-> file)
      {
        file_close (cur_script_file-> file);
        cur_script_file-> file = NULL;
      }
}


Bool
parse_next_script_line (void)
{
    SCRIPT_LINE
        *script_line;

    /*  Clean up result of evaluation last parse tree  */
    if ((void *) cur_script_file-> cur_line != & cur_script_file-> line_head)
        gg_clean (cur_script_file-> cur_line-> node);

    cur_script_file-> cur_line = cur_script_file-> cur_line-> next;

    /*  Skip lines with no parse trees.  */
    while (((void *) cur_script_file-> cur_line != & cur_script_file-> line_head)
       && (! cur_script_file-> cur_line-> node))
        cur_script_file-> cur_line = cur_script_file-> cur_line-> next;

    if ((void *) cur_script_file-> cur_line == & cur_script_file-> line_head)
      {
        list_create (script_line, sizeof (SCRIPT_LINE));
        ASSERT (script_line);
        
        script_line-> parent = cur_script_file;
        script_line-> text   = NULL;
        script_line-> line   = cur_script_file-> next_line;

        if (cur_script_block. template)
            script_line-> node = gg_parse_template (script_read);
        else
            script_line-> node = gg_parse_gsl      (script_read);

        /*  This tells us if any lines were read from the script file  */
        if (cur_script_file-> next_line == script_line-> line)
          {
            mem_free (script_line);
            return FALSE;
          }
        list_relink_before (script_line, & cur_script_file-> line_head);
        cur_script_file-> cur_line = script_line;
      }
    return TRUE;
}


Bool
script_read (char *text)
{
    char
        line [LINE_MAX + 1],
       *ptr;
    int
        len;

    if (cur_script_file-> eof)
        return FALSE;

    if (cur_script_file-> file)
      {
        if (!file_read (cur_script_file-> file, line))
          {
            cur_script_file-> eof  = TRUE;
            return FALSE;
          }
        strncpy (text, line, LINE_MAX);
        text [LINE_MAX] = '\0';             /*  Just in case  */
      }
    else
      {
        text [0] = '\0';

        /* Handle blank line separately - strtok doesn't like empty tokens */
        if (! *cur_script_file-> ptr)
          {
            cur_script_file-> eof = TRUE;
            return FALSE;
          }

        ptr = strchr (cur_script_file-> ptr, '\n');
        if (ptr)
            len = ptr - cur_script_file-> ptr;
        else
            len = strlen (cur_script_file-> ptr);

        if (len > LINE_MAX)
            len = LINE_MAX;

        strncpy (text, cur_script_file-> ptr, len);
        text [len] = '\0';

        if (ptr)
            cur_script_file-> ptr = ptr + 1;
        else
            cur_script_file-> ptr += len;
      }

    cur_script_file-> next_line++;
    return TRUE;
}


Bool
read_next_script_line (void)
{
    SCRIPT_LINE
        *script_line;
    static char
        line [LINE_MAX + 1];

    cur_script_file-> cur_line = cur_script_file-> cur_line-> next;

    if ((void *) cur_script_file-> cur_line == & cur_script_file-> line_head)
      {
        list_create (script_line, sizeof (SCRIPT_LINE));
        ASSERT (script_line);
        
        script_line-> parent = cur_script_file;
        script_line-> node   = NULL;
        script_line-> line   = cur_script_file-> next_line;

        if (script_read (line))
            script_line-> text = mem_strdup (line);
        else
          {
            mem_free (script_line);
            return FALSE;
          }
        list_relink_before (script_line, & cur_script_file-> line_head);
        cur_script_file-> cur_line = script_line;
      }
    return TRUE;
}


SCRIPT_LINE *
script_position (void)
{
    if (cur_script_file)
        return cur_script_file-> cur_line;
    else
        return NULL;
}


void
restore_position (SCRIPT_LINE *position)
{
    cur_script_file            = position-> parent;
    cur_script_file-> cur_line = position;
}


void
destroy_script_data (void)
{
    SCRIPT_FILE
        *script_file;
    SCRIPT_LINE
        *script_line;

    while (! list_empty (& script_list))
      {
        script_file = (SCRIPT_FILE *) script_list. prev;
        if (script_file-> file)
            file_close (script_file-> file);
        mem_free (script_file-> name);
        mem_free (script_file-> text);
        while (! list_empty (& script_file-> line_head))
          {
            script_line = (SCRIPT_LINE *) script_file-> line_head. prev;
            gg_free  (script_line-> node);
            mem_free (script_line-> text);
            list_unlink (script_line);
            mem_free (script_line);
          }
        list_unlink (script_file);
        mem_free (script_file);
      }
}          



XML_ITEM *
extended_scope_xml (SCRIPT_NODE *scope)
{
    SCRIPT_NODE
        *member = NULL;
    XML_ITEM
        *xml_item = NULL,
        *xml_child;
    SCOPE_BLOCK
        *scope_block;

    if (scope)
      {
        member = scope;
        while ((member-> type == GG_MEMBER) && (member-> op1))
            member = member-> op1;
      }

    if ((! scope)                                  /*  Empty starting scope  */
    ||  (member-> type == GG_MEMBER))
        scope_block = lookup_scope_block ("");
    else
      {
	string_result (member);
        scope_block = lookup_scope_block (member-> result_s);

        member = member-> parent;
      }

    if (! scope_block)
        return NULL;

    xml_item = scope_block-> xml_item;
    if (! xml_item)
      {
        gg_report_error ('W', "Scope has been deleted.");
        return NULL;
      }

    if (scope)
        while (xml_item && (member != scope-> parent))
          {
            ASSERT (member-> op2);
            
            string_result (member-> op2);
            if (ignorecase)
              {
                FORCHILDREN (xml_child, xml_item)
                    if (lexcmp (member-> op2-> result_s,
                                xml_item_name (xml_child)) == 0)
                        break;
              }
            else
              {
                FORCHILDREN (xml_child, xml_item)
                    if (streq (member-> op2-> result_s,
                               xml_item_name (xml_child)) == 0)
                        break;
              }
            xml_item = xml_child;
            member = member-> parent;
          }

    return xml_item;
}


XML_ITEM *
lookup_from_xml (SCRIPT_NODE *scope)
{
    XML_ITEM
        *xml_item;

    xml_item = extended_scope_xml (scope);
    if (! xml_item)
        gg_report_error ('E', "Unknown data scope: %s",
                              extended_scope_string (scope));
    return xml_item;
}


char *
extended_scope_string (SCRIPT_NODE *scope)
{
    SCRIPT_NODE
        *member;
    static char
        line [LINE_MAX + 1];
    int
        length = 0;

    line [0] = '\0';
    if (scope)
      {
        member = scope;
        while ((member-> type == GG_MEMBER) && (member-> op1))
            member = member-> op1;
            
        if (member-> type != GG_MEMBER)
          {
            strncat (line, string_result (member),
                           LINE_MAX - length);
            length += strlen (member-> result_s);
            member = member-> parent;
          }
        while (member != scope-> parent && length < LINE_MAX)
          {
            strncat (line, "->", LINE_MAX - length);
            length += 2;
            strncat (line, string_result (member-> op2),
                           LINE_MAX - length);
            length += strlen (member-> op2-> result_s);
            member = member-> parent;
          }
      }
    return line;
}
    

char *
symbol_value (SCRIPT_NODE *symbol)
{
    XML_ITEM
        *xml_item;
    SCOPE_BLOCK
        *scope_block;
    char
        *value = NULL,
        *result;

    if (symbol-> op1)                   /*  Scope specification  */
      {
	xml_item = extended_scope_xml (symbol-> op1);

        if (xml_item == NULL)           /*  Didn't find scope */
            return NULL;

        if (symbol-> op2)
          {
            result = xml_get_attr_ic (xml_item,
                                      symbol-> op2-> result_s,
                                      NULL,
                                      ignorecase);
            return result;
          }
        else
            return compound_item_value (xml_item);

      }
    else
        if (symbol-> op2)
          {
            FORLIST (scope_block, scope_stack)
                if ((xml_item = scope_block-> xml_item) != NULL)
                    if ((value = xml_get_attr_ic (xml_item,
                                                  symbol-> op2-> result_s,
                                                  NULL,
                                                  ignorecase)) != NULL)
                        return value;
          }
        else
            FORLIST (scope_block, scope_stack)
                if ((xml_item = scope_block-> xml_item) != NULL)
                    return compound_item_value (xml_item);

    return NULL;
}


/*  Valueof handles extracting values from the XML tree.  Four cases are     */
/*  covered: attribute value with or without scope, and object value with    */
/*  or without scope.                                                        */
char *
valueof (char *scope, char *name)
{
    SCOPE_BLOCK
        *scope_block;
    char
        *value = NULL,
        *result;

    if (scope)
      {
        if (ignorecase)
          {
            FORLIST (scope_block, scope_stack)
                if ((scope_block-> xml_item)
                &&  (lexcmp (scope, scope_block-> name) == 0))
                  {
                    if (name)
                      {
                        result = xml_get_attr_ic (scope_block-> xml_item,
                                                  name,
                                                  NULL,
                                                  ignorecase);
                        return result;
                      }
                    else
                        return compound_item_value (scope_block-> xml_item);
                  }
          }
        else
          {
            FORLIST (scope_block, scope_stack)
                if ((scope_block-> xml_item)
                &&  (streq (scope, scope_block-> name)))
                  {
                    if (name)
                        return xml_get_attr_ic (scope_block-> xml_item,
                                                name,
                                                NULL,
                                                ignorecase);
                    else
                        return compound_item_value (scope_block-> xml_item);
                  }
          }

      }
    else
    if (name)
      {
        if (ignorecase)
          {
            FORLIST (scope_block, scope_stack)
                if (scope_block-> xml_item)
                  {
                    value = xml_get_attr_ic (scope_block-> xml_item,
                                             name,
                                             NULL,
                                             ignorecase);
                    if (value != NULL)
                        return value;
                  }
          }
        else
          {
            FORLIST (scope_block, scope_stack)
                if (scope_block-> xml_item)
                    if ((value = xml_get_attr_ic (scope_block-> xml_item,
                                                  name,
                                                  NULL,
                                                  ignorecase)) != NULL)
                        return value;
          }
      }
    else
        FORLIST (scope_block, scope_stack)
            if (scope_block-> xml_item)
                return compound_item_value (scope_block-> xml_item);

    return NULL;
}


/*  compound_item_value:  Concatentates all the value nodes of an item into  */
/*  a single string and assigns that string to the value of the given node.  */
/*  This is not really XML, since non-value items do not have values, so     */
/*  this value will not appear if the XML is saved.  We need to do this so   */
/*  that the memory allocated to hold the compound string will get           */
/*  deallocated.                                                             */

char *
compound_item_value (XML_ITEM *item)
{
    XML_ITEM
        *child;
    char
        *name,
        *value,
        *compound = NULL;
    size_t
        length = 0;

    if (xml_item_name (item))
      {
        child = xml_first_child (item);
        while (child)
          {
            name = xml_item_name (child);
            if (! name)           /*  This is a value node  */
              {
                value = xml_item_value (child);
                if (value)
                  {
                    if (compound)
                        compound = mem_realloc (compound,
                                                length + strlen (value) + 1);
                    else
                        compound = mem_alloc (strlen (value) + 1);
                    
                    ASSERT (compound);
                    strcpy (compound + length, value);
                    
                    length += strlen (value);
                  }
              }
            child = xml_next_sibling (child);
          }
        xml_modify_value (item, compound);
        mem_free (compound);
      }
    return xml_item_value (item);
}


void
put_numeric_attr (XML_ITEM *item, const char *name, const long value)
{
    char
        buffer [LONG_WIDTH + 1];

    sprintf (buffer, "%ld", value);
    xml_put_attr (item, name, buffer);
}


void
copy_result (SCRIPT_NODE *dest, SCRIPT_NODE *source)
{
    ASSERT (dest);

    if (source)
      {
        dest-> result_type = source-> result_type;
        dest-> result_s    = mem_strdup (source-> result_s);
        dest-> result_n    = source-> result_n;
        dest-> culprit     = source-> culprit;
      }
    else
        dest-> result_type = TYPE_UNDEFINED;
}


/*- For Stack Functions -----------------------------------------------------*/

void
reset_scope_stack (void)
{
    list_reset (& scope_stack);
}


SCOPE_BLOCK *
create_scope_block (const char *alias)
{
    SCOPE_BLOCK
        *scope_block;

    list_create (scope_block, sizeof (SCOPE_BLOCK));
    ASSERT (scope_block);
    list_relink_after (scope_block, &scope_stack);

    scope_block-> name     = mem_strdup (alias);
    scope_block-> scope_item = NULL;
    scope_block-> xml_item   = NULL;
    scope_block-> index      = 0;
    scope_block-> children   = FALSE;
    list_reset (& scope_block-> item_list);

    return scope_block;
}


void 
destroy_scope_block (void)
{
    SCOPE_BLOCK
        *scope_block = last_scope_block ();

    list_unlink (scope_block);

    if (! list_empty (& scope_block-> item_list))
        while (! list_empty (& scope_block-> item_list))
            destroy_scope_item ((SCOPE_ITEM *) (scope_block-> item_list.prev));
    else
        if (scope_block-> xml_item)
            xml_free (scope_block-> xml_item);

    mem_free (scope_block-> name);
    mem_free (scope_block);
}
        

SCOPE_BLOCK *
lookup_scope_block (char *name)
{
    SCOPE_BLOCK
        *scope_block = NULL;
    long
        scope_number;
    int
        rc;
    char
        c,
        *p;

    ASSERT (name);

    if (*name)
      {
        rc = sscanf (name, "%ld%c", &scope_number, &c);
        if (rc == 1)
          {
            if (scope_number <= 0)  /*  Count from inner to outer   */
              {
                FORLIST (scope_block, scope_stack)
                    if (scope_block-> children)
                        if (scope_number++ == 0)
                            break;
              }
            else  /*  Count from outer to inner  */
                for (scope_block = scope_stack.prev;
                     (void *) scope_block != & scope_stack;
                     scope_block = scope_block-> prev)
                    if (scope_block-> children)
                        if (--scope_number == 0)
                            break;
          }
        else
          {
            p = name;
            if (isalpha (*p))
                while ((isalnum (*p)) || (*p == '_'))
                    p++;
            if (*p)
              {
                gg_report_error ('W', "Illegal scope: %s", name);
                return NULL;
              }

            if (ignorecase)
              {
                FORLIST (scope_block, scope_stack)
                    if (lexcmp (scope_block-> name, name) == 0)
                        break;
              }
            else
              {
                FORLIST (scope_block, scope_stack)
                    if (streq (scope_block-> name, name))
                        break;
              }
          }
      }
    else
        FORLIST (scope_block, scope_stack)
            if (scope_block-> children)
                break;

    if ((void *) scope_block == & scope_stack)
        return NULL;

    if (! scope_block-> children)
        return NULL;

    return scope_block;
}


SCOPE_BLOCK *
first_scope_block (void)
{
    if (!list_empty (&scope_stack))
        return (SCOPE_BLOCK *) scope_stack.prev;
    else
        return NULL;
}


SCOPE_BLOCK *
last_scope_block (void)
{
    if (!list_empty (&scope_stack))
        return (SCOPE_BLOCK *) scope_stack.next;
    else
        return NULL;
}


SCOPE_ITEM *
create_scope_item (SCOPE_BLOCK *scope_block, XML_ITEM *xml_item, long item)
{
    SCOPE_ITEM
        *scope_item;

    list_create (scope_item, sizeof (SCOPE_ITEM));
    ASSERT (scope_item);
    list_relink_before (scope_item, &scope_block-> item_list);

    scope_item-> xml_item = xml_item;
    scope_item-> item     = item;
    scope_item-> result_s = NULL;

    return scope_item;
}


void
destroy_scope_item (SCOPE_ITEM *scope_item)
{
    list_unlink (scope_item);
    mem_free (scope_item-> result_s);
    mem_free (scope_item);
}


Bool
first_scope_item (SCOPE_BLOCK *scope_block)
{
    if (! list_empty (& scope_block-> item_list))
      {
        scope_block-> scope_item = (SCOPE_ITEM *) scope_block-> item_list. next;
        scope_block-> xml_item = scope_block-> scope_item-> xml_item;
        scope_block-> index = 1;
        return TRUE;
      }
    else
      {
        scope_block-> scope_item = NULL;
        scope_block-> xml_item   = NULL;
        return FALSE;
      }
}


Bool
next_scope_item (SCOPE_BLOCK *scope_block)
{
    if (scope_block-> scope_item-> next != & scope_block-> item_list)
      {
        scope_block-> scope_item = scope_block-> scope_item-> next;
        scope_block-> xml_item   = scope_block-> scope_item-> xml_item;
        scope_block-> index++;
        return TRUE;
      }
    else
      {
        scope_block-> scope_item = NULL;
        scope_block-> xml_item   = NULL;
        return FALSE;
      }
}


/*- Script Block Functions --------------------------------------------------*/

void
reset_script_stack (void)
{
    list_reset (& script_stack);
    cur_script_block. position = NULL;
    list_reset (& cur_script_block. control_stack);
}


void
push_script_block (void)
{
    list_push (& script_stack, cur_script_block);
    cur_script_block. position = NULL;
    list_reset (& cur_script_block. control_stack);
}


void 
pop_script_block (void)
{
    list_destroy (& cur_script_block. control_stack);
    if (! list_empty (& script_stack))
        list_pop (& script_stack, cur_script_block);
    else
      {
        cur_script_block. position = NULL;
        list_reset (& cur_script_block. control_stack);
      }
}


void
push_control (CONTROL_TYPE type, void *condition)
{
    CONTROL_BLOCK
        control;

    control.type     = type;
    control.extra    = condition;
    control.position = script_position ();
    list_push (& cur_script_block. control_stack, control);
}


void
pop_control (CONTROL_BLOCK *control)
{
    list_pop (& cur_script_block. control_stack, *control);
}


/*- Error Functions ---------------------------------------------------------*/

void
gg_report_error (char type, char *format, ...)
{
    va_list
        argptr;
    char
        buffer [LINE_MAX],
        line_no [10];
    char
        *ptr;
    int
        length;
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;
    char
        *me = xml_item ? xml_get_attr (xml_item, "me", "") : "";

    va_start (argptr, format);          /*  Start variable arguments list    */
#if (defined (DOES_SNPRINTF))
    length = vsnprintf (buffer, LINE_MAX, format, argptr);
    ASSERT (length != -1);
#else
    length = vsprintf (buffer, format, argptr);
    ASSERT (length <= LINE_MAX);
#endif
    va_end (argptr);                    /*  End variable arguments list      */

    ptr = strtok (buffer, "\n");
    while (ptr)
      {
        if (cur_script_file)
          {
            if ((void *) cur_script_file-> cur_line
            ==  &cur_script_file-> line_head)
                strcpy (line_no, "<End>");
            else
                sprintf (line_no, "%u", cur_script_file-> cur_line-> line);

            coprintf ("%s %c: (%s %s) %s", me, type, 
                      cur_script_file-> name ? cur_script_file-> name : "",
                      line_no, ptr);
          }
        else
            coprintf ("%s %c: %s", me, type, ptr);

        ptr = strtok (NULL, "\n");
      }

    feedback = -1;                      /*  So ggcode returns an error       */
}


/*- Type Conversion Functions -----------------------------------------------*/

char *
string_result (SCRIPT_NODE *node)
{
    char
        buffer [LINE_MAX + 1];
    int
        i;
        
    if (! node-> result_s)
      {
        if (node-> result_type == TYPE_NUMBER)
          {
            sprintf (buffer, "%.9f", node-> result_n);
            i = strlen (buffer) - 1;
            /*  Strip trailing zeroes  */
            while (buffer [i] == '0')
                buffer [i--] = '\0';
            if (buffer [i] == '.')
                buffer [i] = '\0';

            node-> result_s = mem_strdup (buffer);
          }
        else
            node-> result_s = mem_strdup ("");
      }
    return node-> result_s;
}


double
number_result (SCRIPT_NODE *node)
{
    double
        place = 0.1,
        n = 0;
    int
        sign = 1;
    char
        *ch;

    if ((node-> result_type == TYPE_STRING)
    &&  (node-> type != GG_LITERAL))
      {
        ch = node-> result_s;
        if (*ch == '-')
          {
            sign = -1;
            ch++;
          }
        else
        if (*ch == '+')
            ch++;

        while (isdigit (*ch))
            n = n * 10 + *ch++ - '0';

        if (*ch)
            if  (*ch == '.')
              {
                ch++;
                while (isdigit (*ch))
                  {
                    n += (*ch++ - '0') * place;
                    place /= 10;
                  }
              }

        if (! *ch)
          {
            node-> result_type = TYPE_NUMBER;
            node-> result_n    = sign * n;
            return n;
          }
        else
            return 0;
      }
    else
    if (node-> result_type == TYPE_NUMBER)
        return node-> result_n;

    return 0;
}


void
undefined_expression_error (SCRIPT_NODE *node)
{
    SCRIPT_NODE
        *culprit;
    char
        *scope_string = "",
        *point_string = "";

    culprit = node-> culprit;
    if ((culprit)
            ? (culprit-> type == GG_SYMBOL) || (culprit-> type == GG_DEFINE)
            : FALSE)
      {
        if (culprit-> op1)
	    scope_string = extended_scope_string (culprit-> op1);

        /*  Print point except if there is identifier and no scope  */
        if (culprit-> op1 || ! culprit-> op2)
	    point_string = ".";

        gg_report_error ('W', "Undefined identifier: %s%s%s",
			 scope_string,
			 point_string,
			 (culprit-> op2 ? string_result (culprit-> op2) : ""));
      }
    else
        gg_report_error ('W', "Undefined expression.");
}


/*- Print format string -----------------------------------------------------*/


int
format_output (char *buffer, size_t max, SCRIPT_NODE *node)
{
    char
        format [32],
        *in,
        *out = format,
        conversion;
    int
        rc = 0;
    Bool
        error = TRUE;

    *out++ = '%';
    if (node-> format)
      {
        if (strlen (string_result (node-> format))
                                 + node-> format-> spaces > 30)
          {
            gg_report_error ('W', "Format string too long: %s",
                             node-> format-> result_s);
            return -1;
          }

        in = node-> format-> result_s;

        if (node-> format-> spaces)     /*  Skip spaces  */
            *out++ = ' ';
        while (strchr ("#0- +", *in))   /*  Skip optional flags  */
            *out++ = *in++;
        while (isdigit (*in))           /*  Skip optional width  */
            *out++ = *in++;
        if (*in == '.')                 /*  Skip optional precision  */
          {
            *out++ = *in++;
            while (isdigit (*in))
                *out++ = *in++;
          }
        conversion = *in++;
        if (*in == '\0')
          {
            error = FALSE;
            if (strchr ("diouxX", conversion))
              {
                *out++ = 'l';     /*  All integers are long  */
                *out++ = conversion;
                *out = '\0';
                rc = snprintf (buffer, max, format,
                               (long int) number_result (node));
              }
            else
            if (strchr ("eEfg", conversion))
              {
                *out++ = conversion;
                *out = '\0';
                rc = snprintf (buffer, max, format,
                               number_result (node));
              }
            else
            if (conversion == 'c')
              {
                *out++ = conversion;
                *out = '\0';
                rc = snprintf (buffer, max, format,
                               (int) number_result (node));
              }
            else
            if (conversion == 's')
              {
                *out++ = conversion;
                *out = '\0';
                rc = snprintf (buffer, max, format,
                               string_result (node));
              }
            else
                error = TRUE;
          }
      }
    else
      {
        error = FALSE;
        rc = snprintf (buffer, max, "%s",
                       string_result (node));
      }

    if (error)
      {
        gg_report_error ('W', "Invalid format string: %s",
                              node-> format-> result_s);
        return -1;
      }
    else
      {
        if (rc == -1)
            gg_report_error ('W', "Substitution too long.");

        return rc;
      }
}


/*- Collect source text -----------------------------------------------------*/

int
collect_source_text (char **buffer, char *terminator)
{
    int
        offset,
        size,
        space,
        chunk;

    *buffer = mem_alloc (LINE_MAX);
    offset  = 0;
    size    = LINE_MAX;
    space   = LINE_MAX;
    while (read_next_script_line ())
        if (strncmp (cur_script_file-> cur_line-> text,
                     terminator,
                     strlen (terminator)) == 0)
          {
            (*buffer) [offset] = '\0';
            return 0;
          }
        else
          {
            chunk = strlen (cur_script_file-> cur_line-> text);
            if (space < chunk + 2)
              {
                if (*buffer == NULL)
                    *buffer = mem_alloc (LINE_MAX);
                else
                    *buffer = mem_realloc (*buffer, size + LINE_MAX);
                ASSERT (*buffer);

                space += LINE_MAX;
                size  += LINE_MAX;
              }
            strcpy ((*buffer) + offset, cur_script_file-> cur_line-> text);
            offset += chunk;
            (*buffer) [offset] = '\n';
            offset += 1;
            space -= chunk + 1;
          }

    gg_report_error ('E', "End of text not found.");
    return -1;
}


/*- Javascript native C functions -------------------------------------------*/

#if defined (JAVASCRIPT)

JSBool
js_valueof (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    char
        *scope = NULL,
        *name,
        *value;

    scope = JS_GetStringBytes (JS_ValueToString (cx, argv[0]));
    name  = JS_GetStringBytes (JS_ValueToString (cx, argv[1]));
    value = valueof (scope, name);
    if (value)
        *rval = STRING_TO_JSVAL
                    (JS_NewStringCopyN (cx, value, strlen (value)));
    else
        *rval = JSVAL_NULL;
    return JS_TRUE;
}


JSBool
js_write (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    char
        *text;

    text = JS_GetStringBytes (JS_ValueToString (cx, argv[0]));
    if (text)
      {
        if (stdout_fct)
            (stdout_fct) (text);
        if (stdout_echo)
            fprintf (stdout, "%s", text);

        return JS_TRUE;
      }
    else
        return JS_FALSE;
}


void
js_error (JSContext *cx, const char *message, JSErrorReport *report)
{
    char
        *start,
        *end,
        buffer [LINE_MAX];
    int
        intro;
    XML_ITEM
        *xml_item = first_scope_block ()-> xml_item;
    char
        *me = xml_item ? xml_get_attr (xml_item, "me", "") : "";

    intro = snprintf (buffer, 32, "%s E: (%s %u) ", me,
                      report-> filename ? report-> filename : "",
                      report-> lineno);
    if (intro == -1)  /*  Over-run  */
        intro = 32;

    coprintf ("%sJavascript error:", buffer);

    start = (char *) message;
    while (*start)
      {
        end = strchr (start, '\n');
        if (end)
          {
            strncpy (buffer + intro, start, end - start);
            buffer [intro + end - start] = '\0';
          }
        else
            strcpy  (buffer + intro, start);

        coprintf ("%s", buffer);

        if (end)
            start = end + 1;
        else
            break;
      }
}

#endif
