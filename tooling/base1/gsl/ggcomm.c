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

#include "ggpriv.h"                     /*  Header file                      */


/*- Type definitions --------------------------------------------------------*/

typedef struct {
    SCRIPT_NODE_TYPE type;
    int              template;
    SCRIPT_LINE     *script_line;
} CONTROL_BLOCK;

/*- Global variables --------------------------------------------------------*/

LIST
    all_scopes;                         /*  Every scope block ie all threads */

/*- Global variables local to this module -----------------------------------*/

static char
    error_buffer [LINE_MAX + 1];

static RESULT_NODE
    *result_node_cache = NULL;          /*  To cache unused blocks           */


/*- Prototypes --------------------------------------------------------------*/

static void strneat                      (char *text);
static void replace_referred_xml_by_null (XML_ITEM **xml_referred, 
                                          XML_ITEM *xml_delete);
static void construct_block              (VALUE *result);
static RESULT_NODE *first_operand        (RESULT_NODE *r, int *indent);
static RESULT_NODE *next_operand         (RESULT_NODE *r, int *indent);

/*- Functions ---------------------------------------------------------------*/


SCOPE_BLOCK *
lookup_simple_scope (LIST *scope_stack,  VALUE *scope_value,
                     Bool ignorecase,    char **error_text)
{
    long
        scope_number;
    char
        *name_ptr;
    SCOPE_BLOCK
        *scope_block;

    if (error_text)
        *error_text = NULL;             /*  No errors yet  */

    scope_number = (long) number_value (scope_value);
    if (scope_value-> type == TYPE_NUMBER)
      {
        if (scope_number <= 0)  /*  Count from inner to outer   */
          {
            FORLIST (scope_block, *scope_stack)
                if (scope_block-> stacked)
                    if (scope_number++ == 0)
                        break;
          }
        else  /*  Count from outer to inner  */
            for (scope_block = scope_stack-> prev;
                 (void *) scope_block != scope_stack;
                 scope_block = scope_block-> prev)
                if (scope_block-> stacked)
                    if (--scope_number == 0)
                        break;
      }
    else
    /*  A null string as a starting scope currently means use the    */
    /*  topmost stacked scope.                                       */
    if ((! scope_value-> s) || (! scope_value-> s [0]))
      {
        FORLIST (scope_block, *scope_stack)
            if (scope_block-> stacked)
                break;
      }
    else
      {
        name_ptr = scope_value-> s;
        if (isalpha (*name_ptr))
            while ((isalnum (*name_ptr)) || (*name_ptr == '_'))
                name_ptr++;

        if (*name_ptr)
          {
            if (error_text)
              {
                sprintf (error_buffer, "Illegal scope: %s", scope_value-> s);
                *error_text = error_buffer;
              }
            return NULL;
          }
        
        if (ignorecase)
          {
            FORLIST (scope_block, *scope_stack)
                if (scope_block-> name
                && (lexcmp (scope_block-> name, scope_value-> s) == 0))
                    break;
          }
        else
          {
            FORLIST (scope_block, *scope_stack)
                if (scope_block-> name
                && (streq (scope_block-> name, scope_value-> s)))
                    break;
          }
      }

    if ((void *) scope_block == scope_stack)
      {
        /*  PH 2002/10/10 - removed this code, which was pissing me off
        if (error_text)
          {
            sprintf (error_buffer, "Unknown scope: %s", scope_value-> s);
            *error_text = error_buffer;
          }
        End of removed/reinstated code  */
        return NULL;
      }

    return scope_block;
}


char *
name_the_symbol (RESULT_NODE *node)
{
    char
        *scope_string = "",
        *point_string = "";

    if (node-> scope)
        scope_string = extended_scope_string (node-> scope);

    /*  Print point except if there is identifier and no scope  */
    if (node-> scope || ! node-> name)
        point_string = ".";

    sprintf (error_buffer, "%s%s%s",
                           scope_string,
                           point_string,
                           node-> name
                             ? string_value (& node-> name-> value)
                             : "");

    return error_buffer;
}


void
undefined_expression_error (RESULT_NODE *node,
                            char **error_text)
{
    if (node-> culprit)
        sprintf (error_buffer, "Undefined identifier: %s", node-> culprit);
    else
        strcpy (error_buffer, "Undefined expression.");

    *error_text = error_buffer;
}


XML_ITEM *
extended_scope_xml (LIST *scope_stack, RESULT_NODE *scope, 
                    Bool ignorecase,   char **error_text)
{
    RESULT_NODE
        *member = NULL;
    XML_ITEM
        *xml_item = NULL,
        *xml_child;
    SCOPE_BLOCK
        *scope_block;

    if (error_text)
        *error_text = NULL;             /*  No errors yet  */

    if (scope)
      {
        member = scope;
        while ((member-> script_node-> type == GG_MEMBER) && (member-> op1))
            member = member-> op1;
      }

    /*  No scope at all means suse the topmost stacked scope.                */
    if ((! scope)
    ||  (member-> script_node-> type == GG_MEMBER))
      {
        FORLIST (scope_block, *scope_stack)
            if (scope_block-> stacked)
                break;

        if ((void *) scope_block == scope_stack)
            return NULL;
      }
    else
      {
        scope_block = lookup_simple_scope (scope_stack, & member-> value,
                                           ignorecase,  error_text);
        if (! scope_block)
          {
            return NULL;
          }
        member = member-> parent;
      }

    xml_item = scope_block-> xml_item;
    if (! xml_item)
      {
        if (error_text)
          {
            strcpy (error_buffer, "Scope has been deleted.");
            *error_text = error_buffer;
          }
        return NULL;
      }

    /*  Now chase down those members.                                        */
    if (scope)
        while (xml_item && (member != scope-> parent))
          {
            ASSERT (member-> op2);
            
            string_value (& member-> op2-> value);
            if (ignorecase)
              {
                FORCHILDREN (xml_child, xml_item)
                    if (lexcmp (member-> op2-> value. s,
                                xml_item_name (xml_child)) == 0)
                        break;
              }
            else
              {
                FORCHILDREN (xml_child, xml_item)
                    if (streq (member-> op2-> value. s,
                               xml_item_name (xml_child)))
                        break;
              }
            xml_item = xml_child;
            if (! xml_item)
              {
                sprintf (error_buffer, "Child does not exist: %s",
                                       member-> op2-> value. s);
                *error_text = error_buffer;
                return NULL;
              }
            member = member-> parent;
          }

    return xml_item;
}


char *
extended_scope_string (RESULT_NODE *scope)
{
    RESULT_NODE
        *member;
    static char
        line [LINE_MAX + 1];
    int
        length = 0;

    line [0] = '\0';
    if (scope)
      {
        member = scope;
        while ((member-> script_node-> type == GG_MEMBER) && (member-> op1))
            member = member-> op1;
            
        if (member-> script_node-> type != GG_MEMBER)
          {
            strncat (line, string_value (& member-> value),
                           LINE_MAX - length);
            length += strlen (member-> value. s);
            member = member-> parent;
          }
        while (member != scope-> parent && length < LINE_MAX)
          {
            strncat (line, "->", LINE_MAX - length);
            length += 2;
            strncat (line, string_value (& member-> op2-> value),
                           LINE_MAX - length);
            length += strlen (member-> op2-> value. s);
            member = member-> parent;
          }
      }
    return line;
}
    

XML_ITEM *
symbol_xml_parent (LIST *scope_stack, RESULT_NODE *symbol, 
                   Bool ignorecase,   char **error_text)
{
    XML_ITEM
        *xml_parent;
    RESULT_NODE
        *name;
    SCOPE_BLOCK
        *scope_block;

    if (error_text)
        *error_text = NULL;             /*  No errors yet  */

    if (symbol-> scope)
    xml_parent = extended_scope_xml (scope_stack,
                                         symbol-> scope, 
                                         ignorecase,
                                         error_text);
    else
      {
        xml_parent = NULL;
        
        /*  Name might come from symbol if symbol appeared as an operand.    */
        name = symbol-> name ? symbol-> name : symbol-> op1;
        if (name)
          {
            /*  Look for a stacked scope that defines this identifier.       */
            FORLIST (scope_block, *scope_stack)
                if (scope_block-> stacked && scope_block-> xml_item)
                    if (xml_attr_ic (scope_block-> xml_item,
                                     string_value (& name-> value),
                                     ignorecase) != NULL)
                        {
                          xml_parent = scope_block-> xml_item;
                          break;
                        }
          }
        if (xml_parent == NULL)
          {
            /*  Use outermost stacked scope block.  */
            scope_block = first_scope_block (scope_stack);
            while (((void *) scope_block != scope_stack)
               && (! scope_block-> stacked))
                scope_block = scope_block-> prev;

            /*  ASSERT that there is at least one stacked scope.  */
            ASSERT ((void *) scope_block != scope_stack);

            xml_parent = scope_block-> xml_item;
            if (xml_parent == NULL)
              {
                strcpy (error_buffer, "Scope has been deleted.");
                *error_text = error_buffer;
              }
          }
      }
    
    return xml_parent;
}


char *
symbol_value (LIST *scope_stack, RESULT_NODE *symbol, 
              Bool ignorecase,   char **error_text)
{
    XML_ITEM
        *xml_item;
    SCOPE_BLOCK
        *scope_block;
    char
        *value = NULL;
    RESULT_NODE
        *name;

    if (error_text)
        *error_text = NULL;             /*  No errors yet  */

    if (symbol-> scope)
      {
    xml_item = extended_scope_xml (scope_stack,
                                       symbol-> scope,
                                       ignorecase,
                                       error_text);

        if (xml_item == NULL)           /*  Didn't find scope */
            return NULL;

        if (symbol-> name)
            return xml_get_attr_ic (xml_item,
                                    symbol-> name-> value. s,
                                    NULL,
                                    ignorecase);
        else
            return compound_item_value (xml_item);

      }
    else
      {
        name = symbol-> name ? symbol-> name : symbol-> op1;
        if (name)
          {
            string_value (& name-> value);
            FORLIST (scope_block, *scope_stack)
                if ((scope_block-> stacked)
                && ((xml_item = scope_block-> xml_item) != NULL))
                    if ((value = xml_get_attr_ic (xml_item,
                                                  name-> value. s,
                                                  NULL,
                                                  ignorecase)) != NULL)
                        return value;
          }
        else
            FORLIST (scope_block, *scope_stack)
                if ((scope_block-> stacked)
                && ((xml_item = scope_block-> xml_item) != NULL))
                    return compound_item_value (xml_item);
      }
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


Bool
store_symbol_definition (LIST  *scope_stack,  Bool  ignorecase,
                         RESULT_NODE *symbol, VALUE *value,
                         char **error_text)
{
    RESULT_NODE
        *name;
    XML_ITEM
        *xml_value,
        *xml_child,
        *xml_father;
    char
        *item_name;

    if (error_text)
        *error_text = NULL;             /*  No errors yet  */

    if (symbol-> scope)
      {
        if (symbol-> scope-> value. type == TYPE_UNDEFINED)
          {
            
            undefined_expression_error (symbol-> scope, error_text);
            return FALSE;
          }
        name = symbol-> name;
      }
    else
        name = symbol-> name ? symbol-> name : symbol-> op1;

    if (name && name-> value. type == TYPE_UNDEFINED)
      {
        undefined_expression_error (name, error_text);
        return FALSE;
      }

    xml_father = symbol_xml_parent (scope_stack,
                                    symbol,
                                    ignorecase,
                                    error_text);
    if (! xml_father)
      {
        if (error_text
        && (! *error_text))
          {
            sprintf (error_buffer, "Unknown scope: %s",
                     extended_scope_string (symbol-> scope));
            *error_text = error_buffer;
          }
        return FALSE;
      }
    if (name)
        xml_put_attr_ic (xml_father,
                         string_value (& name-> value),
                         value && value-> type != TYPE_UNDEFINED
                             ? string_value (value) : NULL,
                         ignorecase,
                         TRUE);
    else
      {
        /*  Modifying an item's value means getting rid of any existing  */
        /*  value nodes then adding one value node with the new value.   */
        xml_child = xml_first_child (xml_father);
        while (xml_child)
          {
            item_name = xml_item_name (xml_child);
            xml_value = xml_child;
            xml_child = xml_next_sibling (xml_child);
            if (! item_name)           /*  This is a value node  */
                xml_free (xml_value);
          }
        if (value && value-> type != TYPE_UNDEFINED)
            xml_new (xml_father,
                     NULL,       /*  Value node  */
                     string_value (value));
      }
    return TRUE;
}


void
put_numeric_attr (XML_ITEM *item, const char *name, long value)
{
    char
        buffer [LONG_WIDTH + 1];

    sprintf (buffer, "%ld", value);
    xml_put_attr (item, name, buffer);
}


void
put_char_attr (XML_ITEM *item, const char *name, char value)
{
    char
        buffer [2] = " ";

    buffer [0] = value;
    xml_put_attr (item, name, buffer);
}


void
copy_value_ (MEMTRN *memtrn, VALUE *dest, VALUE *source)
{
    char
      **blkptr,
       *txtptr;

    dest-> type = source-> type;
    dest-> s    = memtrn
                ? memt_strdup (memtrn, source-> s)
                : mem_strdup  (source-> s);
    dest-> n    = source-> n;

    if (source-> b)
      {
        dest-> b = memtrn
                 ? memt_copy (memtrn, source-> b)
                 : mem_copy  (source-> b);

        blkptr = dest-> b;
        while (*blkptr++) ;
        txtptr = (char *) blkptr;
        blkptr = dest-> b;
        while (*blkptr)
          {
            *blkptr++ = txtptr;
            txtptr += strlen (txtptr) + 1;
          }
      }
}


void
copy_result (RESULT_NODE *dest, RESULT_NODE *source)
{
    ASSERT (dest);

    if (source)
      {
    copy_value_ (NULL, & dest-> value, & source-> value);
    dest-> culprit  = source-> culprit;
      }
}


RESULT_NODE *
new_result_node (void)
{
    RESULT_NODE
        *node;
        
    if (result_node_cache)
      {
        node = result_node_cache;
        result_node_cache = node-> next;
        node-> next = NULL;
      }
    else
        node = memt_alloc (NULL, sizeof (RESULT_NODE));

    return node;
}


void
destroy_result (RESULT_NODE *node)
{
    int
        argc;

    if (node)
      {
        if (node-> scope)
            destroy_result (node-> scope);
        if (node-> name)
            destroy_result (node-> name);
        if (node-> op1)
            destroy_result (node-> op1);
        if (node-> op2)
            destroy_result (node-> op2);
        if (node-> as)
            destroy_result (node-> as);
        if (node-> to)
            destroy_result (node-> to);
        if (node-> before)
            destroy_result (node-> before);
        if (node-> after)
            destroy_result (node-> after);
        if (node-> operand)
            destroy_result (node-> operand);

        argc = node-> argc;
        if (argc)
          {
            while (argc--)
                if (node-> argv [argc])
                     destroy_result (node-> argv [argc]);

            mem_free (node-> argv);
          }

        if (node-> value. s)
            mem_free (node-> value. s);
        if (node-> value. b)
            mem_free (node-> value. b);

        /*  Clean up link from parent to avoid multiple frees  */
        if (node-> parent)
          {
            if (node == node-> parent-> scope)
                node-> parent-> scope = NULL;
            if (node == node-> parent-> name)
                node-> parent-> name = NULL;
            if (node == node-> parent-> op1)
                node-> parent-> op1 = NULL;
            if (node == node-> parent-> op2)
                node-> parent-> op2 = NULL;
            if (node == node-> parent-> as)
                node-> parent-> as = NULL;
            if (node == node-> parent-> to)
                node-> parent-> to = NULL;
            if (node == node-> parent-> before)
                node-> parent-> before = NULL;
            if (node == node-> parent-> after)
                node-> parent-> after = NULL;
            if (node == node-> parent-> operand)
                node-> parent-> operand = NULL;

            argc = node-> parent-> argc;
            while (argc--)
                if (node == node-> parent-> argv [argc])
                    node-> parent-> argv [argc] = NULL;
          }

        node-> next = result_node_cache;
        result_node_cache = node;
      }
}


void
remove_scope_references_to_xml (XML_ITEM *xml_delete)
{
    SCOPE_STACK_REF
        *scope_stack_ref;
    SCOPE_BLOCK
        *scope_block;
    SCOPE_ITEM
        *scope_item;

    FORLIST (scope_stack_ref, all_scopes)
        FORLIST (scope_block, *scope_stack_ref-> scope_stack)
          {
            if (! list_empty (& scope_block-> item_list))
              {
                FORLIST (scope_item, scope_block-> item_list)
                  {
                    replace_referred_xml_by_null (& scope_item-> xml_item,
                                                  xml_delete);
                  }
                scope_block-> xml_item = scope_block-> scope_item-> xml_item;
              }
            else
                replace_referred_xml_by_null (& scope_block-> xml_item,
                                              xml_delete);
          }
}


static void
replace_referred_xml_by_null (XML_ITEM **xml_referred, XML_ITEM *xml_delete)
{
    XML_ITEM
        *xml_scan;

    xml_scan = *xml_referred;
    while (xml_scan)
        if (xml_scan == xml_delete)
          {
            *xml_referred = NULL;
            break;
          }
        else
            xml_scan = xml_parent (xml_scan);
}


/*- For Stack Functions -----------------------------------------------------*/

SCOPE_BLOCK *
create_scope_block (LIST *scope_stack, 
                    SCRIPT_NODE_TYPE scope_type, 
                    const char *alias)
{
    SCOPE_BLOCK
        *scope_block;

    list_create (scope_block, sizeof (SCOPE_BLOCK));
    ASSERT (scope_block);
    list_relink_after (scope_block, scope_stack);

    scope_block-> scope_type = scope_type;
    scope_block-> name       = mem_strdup (alias);
    scope_block-> scope_item = NULL;
    scope_block-> xml_item   = NULL;
    scope_block-> total      = 0;
    scope_block-> index      = 0;
    scope_block-> stacked    = TRUE;
    list_reset (& scope_block-> item_list);

    return scope_block;
}


void 
destroy_scope_block (LIST *scope_stack)
{
    SCOPE_BLOCK
        *scope_block = last_scope_block (scope_stack);

    list_unlink (scope_block);

    if (! list_empty (& scope_block-> item_list))
        while (! list_empty (& scope_block-> item_list))
            destroy_scope_item (scope_block-> item_list.prev);

    mem_free (scope_block-> name);
    mem_free (scope_block);
}


SCOPE_BLOCK *
first_scope_block (LIST *scope_stack)
{
    if (!list_empty (scope_stack))
        return scope_stack-> prev;
    else
        return NULL;
}


SCOPE_BLOCK *
last_scope_block (LIST *scope_stack)
{
    if (!list_empty (scope_stack))
        return scope_stack-> next;
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
    init_value_node (scope_item-> sort_key);

    return scope_item;
}


void
destroy_scope_item (SCOPE_ITEM *scope_item)
{
    list_unlink (scope_item);
    mem_free (scope_item-> sort_key. s);
    mem_free (scope_item-> sort_key. b);
    mem_free (scope_item);
}


Bool
first_scope_item (SCOPE_BLOCK *scope_block)
{
    if (! list_empty (& scope_block-> item_list))
      {
        scope_block-> scope_item = scope_block-> item_list. next;
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
    if ((void *) scope_block-> scope_item-> next != & scope_block-> item_list)
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


void         
copy_scope_stack (LIST *to,
                  LIST *from)
{
    SCOPE_BLOCK
        *new_block,
        *old_block;

    for (old_block = from-> prev;
         (void *) old_block != from;
         old_block = old_block-> prev)
      {
        new_block = create_scope_block (to, 
                                        old_block-> scope_type,
                                        old_block-> name);
        new_block-> xml_item = old_block-> xml_item;
        new_block-> total    = old_block-> total;
        new_block-> index    = old_block-> index;
        new_block-> stacked  = old_block-> stacked;
      }
}


/*- Type Conversion Functions -----------------------------------------------*/

char *
string_value (VALUE *value)
{
    char
        buffer [LINE_MAX + 1];
    int
        width,
        lines,
        length,
        i;
    char
      **blkptr,
       *txtptr,
       *strptr;
        
    if (! value-> s)
      {
        if (value-> type == TYPE_NUMBER)
          {
            sprintf (buffer, "%.9f", value-> n);
            i = strlen (buffer) - 1;
            /*  Strip trailing zeroes  */
            while (buffer [i] == '0')
                buffer [i--] = '\0';
            if (buffer [i] == '.')
                buffer [i] = '\0';

            value-> s = mem_strdup (buffer);
          }
        else
        if (value-> type == TYPE_BLOCK)
          {
            blkptr = value-> b;
            width = 0;
            lines = 0;
            while (*blkptr)
              {
                lines++;
                txtptr = *blkptr++;
                length = strlen (txtptr);
                if (length > width)
                    width = length;
              }
            value-> s = mem_alloc (lines * (width + 1));

            blkptr = value-> b;
            strptr = value-> s;
            while (*blkptr)
              {
                strcpy (strptr, *blkptr++);
                length = strlen (strptr);
                memset (strptr + length, ' ', width - length);
                strptr [width] = '\n';
                strptr += width + 1;
              }
            *(strptr - 1) = 0;
          }
        else
            value-> s = mem_strdup ("");
      }
    return value-> s;
}


double
number_value (VALUE *value)
{
    double
        place = 0.1,
        n = 0;
    int
        sign = 1;
    char
        *ch;

    if (value-> type == TYPE_NUMBER)
        return value-> n;
    else
      {
        if ((value-> s) && (*value-> s))
          {
            ch = value-> s;
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
                if (*ch == '.')
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
                value-> type = TYPE_NUMBER;
                value-> n    = sign * n;
                return value-> n;
              }
            else
              {
                value-> type = TYPE_STRING;
                return 0;
              }
          }
      }

    return 0;
}


/*- Error Functions ---------------------------------------------------------*/

/*- Print format string -----------------------------------------------------*/


int
format_output (char *buffer, size_t max, RESULT_NODE *node, char **error_text)
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

    if (error_text)
        *error_text = NULL;             /*  No errors yet  */

    *out++ = '%';
    if (node-> as)
      {
        if (strlen (string_value (& node-> as-> value))
                                 + node-> as-> script_node-> spaces > 30)
          {
            if (error_text)
              {
                sprintf (error_buffer, "Format string too long: %s",
                                       node-> as-> value. s);
                *error_text = error_buffer;
              }

            return -1;
          }

        in = node-> as-> value. s;

        if (node-> as-> script_node-> spaces)     /*  Skip spaces  */
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
                               (long int) number_value (& node-> value));
              }
            else
            if (strchr ("eEfg", conversion))
              {
                *out++ = conversion;
                *out = '\0';
                rc = snprintf (buffer, max, format,
                               number_value (& node-> value));
              }
            else
            if (conversion == 'c')
              {
                *out++ = conversion;
                *out = '\0';
                rc = snprintf (buffer, max, format,
                               (int) number_value (& node-> value));
              }
            else
            if (conversion == 's')
              {
                *out++ = conversion;
                *out = '\0';
                rc = snprintf (buffer, max, format,
                               string_value (& node-> value));
              }
            else
                error = TRUE;
          }
      }
    else
      {
        error = FALSE;
        rc = snprintf (buffer, max, "%s",
                       string_value (& node-> value));
      }

    if (error)
      {
        if (error_text)
          {
            sprintf (error_buffer, "Invalid format string: %s",
                                    node-> as-> value. s);
            *error_text = error_buffer;
          }
        return -1;
      }
    else
      {
        if (rc == -1)
          {
            if (error_text)
              {
                sprintf (error_buffer, "Substitution too long.");
                *error_text = error_buffer;
              }
          }

        return rc;
      }
}


/*  pretty_print: Reformats a string according to a format string consisting */
/*  of a comma-separated series of specifiers.  Currently accepted formats:  */
/*         lower - lower case                                                */
/*         UPPER - UPPER CASE                                                */
/*         Neat  - Neat Case                                                 */
/*         c     - Valid_c_identifier                                        */
/*         COBOL - VALID-COBOL-IDENTIFIER                                    */
/*         left  - Left justify                                              */
/*         block - Turn multi-line into a block                              */
/*                                                                           */
/*  If no case modifier is provided and pretty is not empty and the expr-    */
/*  is a single identifier, its case is used as an example to match for      */
/*  lower, upper or neat case.                                               */

int
pretty_print (VALUE *result,
              RESULT_NODE *pretty, 
              char *example, 
              int space,
              char **error_text)
{
    char
        *tokens,
        *token,
        *c,
        *newstring;
    Bool
        use_example;

    if (error_text)
        *error_text = NULL;             /*  No errors yet  */

    string_value (result);

    use_example = (example != NULL);

    if (pretty)
      {
        if (strlen (string_value (& pretty-> value)) == 0)
            use_example = FALSE;

        tokens = mem_strdup (pretty-> value. s);
        token = strtok (tokens, ", ");
        while (token)
          {
            strlwc (token);
            if (streq (token, "lower"))
              {
                use_example = FALSE;
                strlwc (result-> s);
              }
            else
            if (streq (token, "upper"))
              {
                use_example = FALSE;
                strupc (result-> s);
              }
            else
            if (streq (token, "neat"))
              {
                use_example = FALSE;
                strneat (result-> s);
              }
            else
            if (streq (token, "c"))
              {
                c = result-> s;
                if (!isalpha (*c))
                    *c = '_';
                
                while (*c)
                  {
                    if (!(isalpha (*c) || isdigit (*c)))
                        *c = '_';

                    c++;
                  }
              }
            else
            if (streq (token, "cobol"))
              {
                c = result-> s;
                if (!isalpha (*c))
                    *c = '-';
                
                while (*c)
                  {
                    if (!(isalpha (*c) || isdigit (*c)))
                        *c = '-';

                    c++;
                  }
              }
            else
            if (streq (token, "left"))
              {
                newstring = strjustify (result-> s, space);
                mem_free (result-> s);
                result-> s = newstring;
              }
            else
            if (streq (token, "block"))
                construct_block (result);
            else
            if (error_text)
              {
                sprintf (error_buffer, "Unknown pretty-print modifier: %s",
                                       token);
                *error_text = error_buffer;
                mem_free (token);
                return -1;
              }

            token = strtok (NULL, ", ");
          }
        mem_free (tokens);
      }

    if ((use_example)
    &&  (strlen (example) > 1))
      {
        c = example;

        if (isupper (*c))
            while ((isupper (*c) || !isalpha (*c)) && (*c))
                c++;

        if (*c == 0)
            strupc (result-> s);
        else
        if (c == example + 1)
          {
            if (islower (*c))
              {
                while ((islower (*c) || !isalpha (*c)) && (*c))
                    c++;
                if (!isupper (*c))
                    strneat (result-> s);
              }
          }
        else
            if (c == example)
              {
                if (islower (*c))
                    while ((islower (*c) || !isalpha (*c)) && (*c))
                        c++;

                if (*c == 0)
                    strlwc (result-> s);
              }
      }
    return 0;
}


static void
strneat (char *text)
{
    char
        *c;

    c = text;
    while (*c)
      {
        while ((!isalpha (*c)) && (*c))
            c++;

    if (! *c)
        break;

        *c = toupper (*c);
        c++;

        while (isalpha (*c) && (*c))
          {
            *c = tolower (*c);
            c++;
          }
      }
}


size_t
strllen (const char *s)
{
    const char
        *c;

    c = strrchr (s, '\n');
    if (c)
        c++;
    else
        c = s;
    
    return strlen (c);
}


static void
construct_block (VALUE *result)
{
    char
        *startptr,
        *endptr,
        *txtptr;
    char
        **blkptr;
    int
        length,
        lines,
        blksize,
        txtsize;

    /*  Calculate number of lines and total size of block.  */
    lines   = 0;
    txtsize = 0;
    startptr = result-> s;
    while (startptr)
      {
        lines++;
        endptr = strchr (startptr, '\n');
        if (! endptr)
            endptr = startptr + strlen (startptr);
        txtsize += endptr - startptr + 1;

        if (*endptr)
            startptr = endptr + 1;
        else
            startptr = 0;
      }
    blksize = (lines + 1) * sizeof (char *);
    
    result-> b  = mem_alloc (blksize + txtsize);
    blkptr  = result-> b;
    txtptr  = ((char *) result-> b) + blksize;
    startptr = result-> s;
    while (startptr)
      {
        endptr = strchr (startptr, '\n');
        if (! endptr)
            endptr = startptr + strlen (startptr);
        length = endptr - startptr;
        strncpy (txtptr, startptr, length);
        txtptr [length] = 0;
        *blkptr++ = txtptr;
        txtptr += length + 1;
        if (*endptr)
            startptr = endptr + 1;
        else
            startptr = 0;
      }
    *blkptr = 0;

    mem_free (result-> s);
    result-> s    = NULL;
    result-> type = TYPE_BLOCK;

    /*  JS temporary just to make sure I haven't blundered.  */
    ASSERT (txtptr - (char *) result-> b == blksize + txtsize);
}


char *
concatenate_results (RESULT_NODE *r,
                     int shuffle,
                     Bool convert_indent)
{
    RESULT_NODE
       *op;
    int
        indent = 0,
        shuffle_cnt,
        oplines,
        totlines,
        linewidth,
        opwidth,
        totwidth,
        totlinelength,
        runlength,
        line;
    char
      **blk,
       *txt,
       *rc;

    totlines      = 0;
    totwidth      = 0;
    totlinelength = 0;
    shuffle_cnt   = 0;
    op = first_operand (r, & indent);
    if (convert_indent)
        r-> indent = 0;                 /*  Coz indent has been migrated.    */
    else
        indent = 0;                     /*  Forget about initial indentation */
    while (op)
      {
        op-> indent = indent;           /*  Copy indent value to operand.    */
        /*  Perform shuffle  */
        if (shuffle > 0
        &&  op-> indent >= shuffle)
          {
            if (op-> indent - shuffle_cnt >= shuffle)
                op-> indent -= shuffle_cnt;
            else
                op-> indent  = shuffle;
          }

        /*  Calculate length & width of operand for shuffle.  */
        if (op-> value. type == TYPE_BLOCK)
          {
            /*  Catch undefined operand.  */
            if (! op-> value. b)
                return NULL;

            oplines  = 0;
            opwidth  = 0;
            blk      = op-> value. b;
            while (*blk)
              {
                oplines++;
                linewidth = strlen (*blk);
                if (linewidth > opwidth)
                    opwidth = linewidth;
                blk++;
              }
            opwidth       += op-> indent;
            totwidth      += opwidth;
            totlinelength += opwidth;
          }
        else
          {
            /*  Catch undefined operand.  */
            if (op-> value. type == TYPE_UNDEFINED)
                return NULL;

            string_value (& op-> value);
            oplines = 1;
            opwidth = op-> indent + strllen (op-> value. s);
            if (strchr (op-> value. s, '\n'))
                totwidth  = opwidth;
            else
                totwidth += opwidth;
            totlinelength += op-> indent + strlen (op-> value. s);
          }
        if (oplines > totlines)
            totlines = oplines;

        if (op-> script_node-> extend)
            shuffle_cnt  = totwidth;
        else
            shuffle_cnt += opwidth - (op-> script_node-> width
                                   +  indent);

        op = next_operand (op, & indent);
      }

    /*  Now build the result  */
    rc = mem_alloc (totlines * (totlinelength + 1));
    memset (rc, ' ',   totlines * (totlinelength + 1));

    op = first_operand (r, NULL);
    runlength = 0;
    while (op)
      {
        line = 0;
        if (op-> value. type == TYPE_BLOCK)
          {
            opwidth  = 0;
            blk = op-> value. b;
            while (*blk)
              {
                linewidth = strlen (*blk);
                if (linewidth > opwidth)
                    opwidth = linewidth;

                txt = rc
                    + line * (totlinelength + 1) + runlength
                    + op-> indent;
                memcpy (txt, *blk, strlen (*blk));
                blk++;
                line++;
              }
            opwidth   += op-> indent;
            runlength += opwidth;
          }
        else
          {
            for (line = 0; line < totlines; line++)
              {
                txt = rc
                    + line * (totlinelength + 1) + runlength
                    + op-> indent;
                memcpy (txt, op-> value. s, strlen (op-> value. s));
              }
            runlength += op-> indent + strlen (op-> value. s);
          }
        op = next_operand (op, NULL);
      }

    for (line = 1; line < totlines; line++)
        rc [line * (totlinelength + 1) - 1] = '\n';
    
    rc [totlines * (totlinelength + 1) - 1] = 0;

    /*  JS Blunder check  */
    ASSERT (runlength == totlinelength);

    return rc;
}


static RESULT_NODE *
first_operand (RESULT_NODE *r, int *indent)
{
    if (! r)
        return NULL;

    if (indent)
        *indent = r-> indent;

    while ((! r-> script_node)
       ||   r-> script_node-> type == GG_OPERATOR)
        r = r-> op1;
    return r;
}


static RESULT_NODE *
next_operand (RESULT_NODE *r, int *indent)
{
    RESULT_NODE
        *p;

    p = r-> parent;
    while (p-> parent
       && ((! p-> script_node)
       ||  (  p-> script_node-> type     == GG_OPERATOR
       &&     p-> script_node-> operator == OP_UNDEFINED))
       &&  r == p-> op2)
      {
        r = p;
        p = r-> parent;
      }

    if (((! p-> script_node)
    ||  (  p-> script_node-> type     == GG_OPERATOR
    &&     p-> script_node-> operator == OP_UNDEFINED))
    &&  r != p-> op2)
        return first_operand (p-> op2, indent);
    else
        return NULL;
}


void
destroy_caches (void)
{
    RESULT_NODE
        *node;

    while (result_node_cache)
      {
        node = result_node_cache;
        result_node_cache = result_node_cache-> next;
        mem_free (node);
      }
}
