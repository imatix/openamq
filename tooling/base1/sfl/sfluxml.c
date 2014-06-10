/*===========================================================================*
 *                                                                           *
 *  $(filename) - $(description)                                             *
 *                                                                           *
 *  $(project) $(version)                                                    *
 *  $(copyright)                                                             *
 *                                                                           *
 *  $(license)                                                               *
 *===========================================================================*/

#include "prelude.h"                    /*  Universal header file            */
#include "sflstr.h"                     /*  String functions                 */
#include "sfllist.h"                    /*  List access functions            */
#include "sflmem.h"                     /*  Memory access functions          */
#include "sflfile.h"                    /*  File access functions            */
#include "sfldate.h"                    /*  Date and time functions          */
#include "sflunic.h"                    /*  Unicode function                 */
#include "sfluxml.h"                    /*  Prototypes for functions         */
#include "sfluxmll.h"                   /*  Loading & saving functions       */


/*  Implementation-dependent type definitions                                */

struct _UXML_ITEM {                     /*  Item node definition             */
    struct _UXML_ITEM
        *next,                          /*  Next item in list                */
        *prev,                          /*  Previous item in list            */
        *parent;                        /*  Parent if this is a child        */
    UCODE
        *name,                          /*  Item name, allocated string      */
        *value;                         /*  Value node, allocated string     */
    LIST
        attrs,                          /*  List of attributes, 0 or more    */
        children;                       /*  List of children, 0 or more      */
};

struct _UXML_ATTR {                     /*  Attribute node definition        */
    struct _UXML_ATTR
        *next,                          /*  Next attr in list                */
        *prev;                          /*  Previous attr in list            */
    struct _UXML_ITEM
        *parent;                        /*  Parent item if this attribute    */
    UCODE
        *name,                          /*  Attribute name                   */
        *value;                         /*  Attribute value, may be null     */
};


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_create

    Synopsis: Creates and initialises a new UXML_ITEM item.  Returns
    the address of the created UXML_ITEM item or NULL if there was not
    enough memory.  Sets the new item's name and value as specified; only
    one of these should contain a value, although sflxml will not complain
    if both do.  If the name is non-NULL this is a child node; if the
    value is non-NULL then this is a value node.
    ---------------------------------------------------------------------[>]-*/

UXML_ITEM *
_uxml_create (
    const UCODE *name,
    const UCODE *value,
    Bool        duplicate)
{
    UXML_ITEM
        *item;

    list_create (item, sizeof (UXML_ITEM));
    if (item)
      {
        list_reset (&item-> attrs);
        list_reset (&item-> children);
        item-> parent = NULL;
        if ( duplicate ) {
            item-> name   = name?  mem_ustrdup (name): NULL;
            item-> value  = value? mem_ustrdup (value): NULL;
        }
        else {
            item-> name   = (UCODE *)name;
            item-> value  = (UCODE *)value;
        }

        return (item);
      }
    else
      {
        if ( !duplicate ) {
            if (name)
                mem_free ((UCODE *)name);
            if (value)
                mem_free ((UCODE *)value);
        }
        return (NULL);
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_new

    Synopsis: Creates and initialises a new UXML_ITEM item with a specified
    parent item.  Returns the address of the created UXML_ITEM item or NULL
    if there was not enough memory.  Sets the new item's name and value as
    specified; only one of these should contain a value, although sflxml
    will not complain if both do.  If the name is non-NULL this is a child
    node; if the value is non-NULL then this is a value node.  If the
    parent argument is non-NULL, attaches the new item to the end of the
    parent item list.
    ---------------------------------------------------------------------[>]-*/

UXML_ITEM *
_uxml_new (
    UXML_ITEM   *parent,
    const UCODE *name,
    const UCODE *value,
    Bool        duplicate)
{
    UXML_ITEM
        *item;

    if ( duplicate ) {
        item = uxml_create (name, value);
    }
    else {
        item = uxml_create_no_dup (name, value);
    }

    if (item && parent)
        uxml_attach_child (parent, item);

    return item;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_modify_value

    Synopsis: Modifies an existing XML item's value.
    ---------------------------------------------------------------------[>]-*/

void
uxml_modify_value  (UXML_ITEM *item, const UCODE *value)
{
    ASSERT (item);

    if (!item-> value)
        item-> value = mem_ustrdup (value);
    else
    if (!value     
    || (item-> value && wcscmp (value, item-> value) != 0))
      {
        if (item-> value) mem_free (item-> value);
        item-> value = mem_ustrdup (value);
      }
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_rename

    Synopsis: Modifies an existing XML item's name.
    ---------------------------------------------------------------------[>]-*/


void
uxml_rename (UXML_ITEM *item, const UCODE *name)
{
    ASSERT (item);

    if (item-> name) mem_free (item-> name);
    item-> name = mem_ustrdup (name);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_item_name

    Synopsis: Extracts the name of a specified XML item.  The returned string
    should NOT be modified.  To manipulate it, first make a copy first.
    ---------------------------------------------------------------------[>]-*/

UCODE *
uxml_item_name (UXML_ITEM *item)
{
    ASSERT (item);

    return item-> name;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_item_value

    Synopsis: Extracts the value from a value node.  These are recognised
    by their name being NULL.  The returned string should NOT be modified.
    To manipulate it, first make a copy first.
    ---------------------------------------------------------------------[>]-*/

UCODE *
uxml_item_value (UXML_ITEM *item)
{
    ASSERT (item);

    return item-> value;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_item_child_value

    Synopsis: Extracts all child value.  These are recognised
    by their name being NULL.  The returned string is allocated by mem_alloc,
    must be free by mem_free.
    ---------------------------------------------------------------------[>]-*/

UCODE *
uxml_item_child_value (UXML_ITEM *item)
{
    UXML_ITEM
        *child;
    long
        length;
    UCODE
        *buffer = NULL,
        *target,
        *source;

    length = 0;
    FORUVALUES (child, item)
      {
        if (uxml_item_value (child))
            length += wcslen (uxml_item_value (child));
       }

    if (length > 0)
      {
        buffer = unicode_alloc (length + 1);
        if (buffer != NULL)
          {
            target = buffer;
            FORUVALUES (child, item)
              {
                source = uxml_item_value (child);
                if (source)
                  {
                    while (*source)
                       *target++ = *source++;
                  }
              }
            *target  = 0;
          }
      }

    return (buffer);
}

/*  ---------------------------------------------------------------------[<]-
    Function: uxml_free

    Synopsis: Frees all memory used by an UXML_ITEM item and its children.
    ---------------------------------------------------------------------[>]-*/

void
uxml_free (
    UXML_ITEM *item)
{
    ASSERT (item);

    /*  Free attribute nodes for the item                                    */
    while (!list_empty (&item-> attrs))
        uxml_free_attr (item-> attrs.next);

    /*  Free child nodes for the item                                        */
    while (!list_empty (&item-> children))
        uxml_free (item-> children.next);

    /*  Now free this item itself                                            */
    list_unlink (item);                 /*  Unlink from its parent list      */
    if (item-> name)  mem_free (item-> name); 
    if (item-> value) mem_free (item-> value);
    mem_free (item);                    /*  And free the item itself         */
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_attach_child

    Synopsis: Attaches an XML item as the last child of a given parent.
    If the item is already attached to a parent, it is first removed.
    ---------------------------------------------------------------------[>]-*/

void
uxml_attach_child (
    UXML_ITEM *parent,
    UXML_ITEM *item)
{
    if (item == NULL)
        return;

    if (item-> parent)
        uxml_detach (item);

    item-> parent = parent;
    if (parent)
        list_relink_before (item, &parent-> children);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_attach_sibling

    Synopsis: Attaches an XML item as the sibling preceeding a given item.
    If the item is already attached to a parent, it is first removed.
    ---------------------------------------------------------------------[>]-*/

void
uxml_attach_sibling (
    UXML_ITEM *sibling,
    UXML_ITEM *item)
{
    if (item-> parent)
        uxml_detach (item);

    item-> parent = uxml_parent (sibling);
    list_relink_before (item, sibling);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_detach

    Synopsis: Removes an XML item from the tree.
    ---------------------------------------------------------------------[>]-*/

void
uxml_detach (
    UXML_ITEM *item)
{
    item-> parent = NULL;
    list_unlink (item);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_copy

    Synopsis: Recursively makes a copy of all the attributes and children
              of node 'from', attaching them to node 'to'.
    ---------------------------------------------------------------------[>]-*/

int
uxml_copy (UXML_ITEM *to, UXML_ITEM *from)
{
    UXML_ATTR
        *attr;
    UXML_ITEM
        *to_child,
        *from_child;

    FORUATTRIBUTES (attr, from)
        uxml_put_attr (to,
                      uxml_attr_name  (attr),
                      uxml_attr_value (attr));

    for (from_child  = uxml_first_child (from);
         from_child != NULL;
         from_child  = uxml_next_sibling (from_child))
      {
        to_child = uxml_new (to,
                            uxml_item_name  (from_child),
                            uxml_item_value (from_child));
        if (! to_child)
            return -1;

        uxml_copy (to_child, from_child);
      }
    return 0;
}




/*  ---------------------------------------------------------------------[<]-
    Function: uxml_first_child

    Synopsis: Returns the first child node of the specified item, or NULL
    if there are none.
    ---------------------------------------------------------------------[>]-*/

UXML_ITEM *
uxml_first_child (UXML_ITEM *item)
{
    ASSERT (item);

    if (!list_empty (&item-> children))
        return item-> children. next;
    else
        return NULL;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_last_child

    Synopsis: Returns the last child node of the specified item, or NULL
    if there are none.
    ---------------------------------------------------------------------[>]-*/

UXML_ITEM *
uxml_last_child (UXML_ITEM *item)
{
    ASSERT (item);

    if (!list_empty (&item-> children))
        return item-> children. prev;
    else
        return NULL;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_next_sibling

    Synopsis: Returns the next sibling of the specified item, or NULL if there
    if are none.
    ---------------------------------------------------------------------[>]-*/

UXML_ITEM *
uxml_next_sibling (UXML_ITEM *item)
{
    ASSERT (item);

    if ((LIST *) item-> next != & item-> parent-> children)
        return item-> next;
    else
        return NULL;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_prev_sibling

    Synopsis: Returns the previous sibling of the specified item, or NULL if
    there if are none.
    ---------------------------------------------------------------------[>]-*/

UXML_ITEM *
uxml_prev_sibling (UXML_ITEM *item)
{
    ASSERT (item);

    if ((LIST *) item-> prev != & item-> parent-> children)
        return item-> prev;
    else
        return NULL;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_parent

    Synopsis: Returns the parent of the specified item, or NULL if this is
    the root item.
    ---------------------------------------------------------------------[>]-*/

UXML_ITEM *
uxml_parent (UXML_ITEM *item)
{
    ASSERT (item);

    return (item-> parent);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_put_attr_ic

    Synopsis: Sets, modifies, or deletes an attribute for the
    specified item.  The attribute name must be supplied.  If the value is
    NULL,  the first attribute with the specified name is deleted.  Otherwise
    it is either created or modified accordingly.  If the paramater
    'ignore_case' is TRUE, the case of the attribute name is insignificant.
    Returns the number  of attribute nodes created (-1, 0, or 1).
    ---------------------------------------------------------------------[>]-*/

int
uxml_put_attr_ic (
    UXML_ITEM   *item,
    const UCODE *name,
    const UCODE *value,
    Bool        ignore_case,
    Bool        duplicate)
{
    int
        feedback = 0;
    UXML_ATTR
        *attr;

    ASSERT (item);
    ASSERT (name);

    attr = uxml_attr_ic (item, name, ignore_case);
    if (attr)
        if (value)                      /*  Value specified - update attr    */
          {
            if (attr-> value) mem_free (attr-> value);
            if ( duplicate )
                attr-> value = mem_ustrdup (value);
            else
              {
                attr-> value = (UCODE *)value;
                if (name) mem_free ((UCODE *)name);
              }
          }
        else
          {
            uxml_free_attr (attr);       /*  No value - delete attribute      */
            feedback = -1;
            if ( ! duplicate )
              {
                if (name)
                    mem_free ((UCODE *)name);
                if (value)
                    mem_free ((UCODE *)value);
              }
          }
    else
        if (value)                      /*  Value specified - update attr    */
          {
            list_create (attr, sizeof (UXML_ATTR));
            if (attr)
              {
                if ( duplicate )
                  {
                    attr-> name   = name?  mem_ustrdup (name):  NULL;
                    attr-> value  = value? mem_ustrdup (value): NULL;
                  }
                else
                  {
                    attr-> name   = (UCODE *)name;
                    attr-> value  = (UCODE *)value;
                  }
                attr-> parent = item;
                list_relink_before (attr, &item-> attrs);
                feedback = 1;
              }
            else
            if (!duplicate)
              {
                if (name)
                    mem_free ((UCODE *)name);
                if (value)
                    mem_free ((UCODE *)value);
              }
          }
        else
        if (!duplicate)
          {
                if (name)
                    mem_free ((UCODE *)name);
                if (value)
                    mem_free ((UCODE *)value);
          }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_attr_ic

    Synopsis: Searches for the attribute with the specified name; if
    found, returns the address of the attribute node, otherwise  returns
    NULL.  If the paramater 'ignore_case' is TRUE, the case of the
    attribute name is insignificant.
    ---------------------------------------------------------------------[>]-*/

UXML_ATTR *
uxml_attr_ic (
    UXML_ITEM   *item,
    const UCODE *name,
    Bool        ignore_case)
{
    UXML_ATTR
        *attr;

    ASSERT (item);
    ASSERT (name);

    if (ignore_case)
      {
        FORLIST (attr, item-> attrs)
            if (attr-> name ? lexucmp (attr-> name, name) == 0 : FALSE)
                return (attr);
      }
    else
      {
        FORLIST (attr, item-> attrs)
            if (attr-> name ? wcscmp (attr-> name, name) == 0: FALSE)
                return (attr);
      }
    return (NULL);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_attr_name

    Synopsis: Extracts the name of a specified XML attr.  The returned string
    should NOT be modified.  To manipulate it, first make a copy first.
    ---------------------------------------------------------------------[>]-*/

UCODE *
uxml_attr_name (UXML_ATTR *attr)
{
    ASSERT (attr);

    return attr-> name;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_attr_value

    Synopsis: Extracts the value of a specified XML attr.  The returned string
    should NOT be modified.  To manipulate it, first make a copy first.
    ---------------------------------------------------------------------[>]-*/

UCODE *
uxml_attr_value (UXML_ATTR *attr)
{
    ASSERT (attr);

    return attr-> value;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_get_attr_ic

    Synopsis: Returns the value for the specified attribute, if it exists.
    Otherwise returns the default value.  If the paramater 'ignore_case'
    is TRUE, the case of the attribute name is insignificant.
    ---------------------------------------------------------------------[>]-*/

UCODE *
uxml_get_attr_ic (
    UXML_ITEM   *item,
    const UCODE *name,
    const UCODE *deflt,
    Bool        ignore_case)
{
    UXML_ATTR
        *attr;

    ASSERT (item);
    ASSERT (name);

    attr = uxml_attr_ic (item, name, ignore_case);
    if (attr)
        return (attr-> value);
    else
        return ((UCODE *) deflt);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_free_attr

    Synopsis: Frees all memory used by an UXML_ATTR node.
    ---------------------------------------------------------------------[>]-*/

void
uxml_free_attr (
    UXML_ATTR *attr)
{
    ASSERT (attr);

    list_unlink (attr);
    if (attr-> name)  mem_free (attr-> name);
    if (attr-> value) mem_free (attr-> value);
    mem_free (attr);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_first_attr

    Synopsis: Returns the first attribute of a specified XML item, or NULL
    if there are none.
    ---------------------------------------------------------------------[>]-*/

UXML_ATTR *
uxml_first_attr (UXML_ITEM *item)
{
    ASSERT (item);

    if (!list_empty (&item-> attrs))
        return item-> attrs. next;
    else
        return NULL;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_last_attr

    Synopsis: Returns the last attribute of a specified XML item, or NULL
    if there are none.
    ---------------------------------------------------------------------[>]-*/

UXML_ATTR *
uxml_last_attr (UXML_ITEM *item)
{
    ASSERT (item);

    if (!list_empty (&item-> attrs))
        return item-> attrs. prev;
    else
        return NULL;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_next_attr

    Synopsis: Returns the next attribute following the specified attribute,
    or NULL if there are none.
    ---------------------------------------------------------------------[>]-*/

UXML_ATTR *
uxml_next_attr (UXML_ATTR *attr)
{
    ASSERT (attr);

    if ((LIST *) attr-> next != & attr-> parent-> attrs)
        return attr-> next;
    else
        return NULL;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_prev_attr

    Synopsis: Returns the previous attribute following the specified
    attribute, or NULL if there are none.
    ---------------------------------------------------------------------[>]-*/

UXML_ATTR *
uxml_prev_attr (UXML_ATTR *attr)
{
    ASSERT (attr);

    if ((LIST *) attr-> prev != & attr-> parent-> attrs)
        return attr-> prev;
    else
        return NULL;
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_changed

    Synopsis: Returns TRUE if the XML file loaded into the specified list
    has in the meantime been changed.  Returns FALSE if not.
    ---------------------------------------------------------------------[>]-*/

Bool
uxml_changed (
    UXML_ITEM *item)
{
    char
        *filename,
        *filedate,
        *filetime;
    UCODE
        *fname;
    Bool
        feedback = FALSE;
    
    ASSERT (item);

    /*  Date, time, and name of original XML file are in the list            */
    fname = uxml_get_attr (item, L"filename", NULL);
    if (fname)
      {
        filename = ucode2ascii (fname);
        filedate = ucode2ascii (uxml_get_attr (item, L"filedate", L"0"));
        filetime = ucode2ascii (uxml_get_attr (item, L"filetime", L"0"));
        if (file_has_changed (filename, atol (filedate), atol (filetime)))
            feedback = TRUE;
        if (filename) mem_free (filename);
        if (filedate) mem_free (filedate);
        if (filetime) mem_free (filetime);
      }
    return (feedback);
}


/*  ---------------------------------------------------------------------[<]-
    Function: uxml_refresh

    Synopsis: Refreshes an XML tree created by uxml_load ().  If the original
    file (as specified by the 'filename' attribute of the root item) has
    been modified, reloads the whole XML file.  Returns TRUE if the XML file
    was actually reloaded, or FALSE if the file had not changed or could not
    be accessed, or if the XML tree was incorrectly created.
    ---------------------------------------------------------------------[>]-*/

Bool
uxml_refresh (
    UXML_ITEM **item)
{
    char
        *filename,
        *pathsym;
    int
        rc;

    ASSERT (item);
    ASSERT (*item);
    if (uxml_changed (*item))
      {
        pathsym  = ucode2ascii (uxml_get_attr (*item, L"pathsym",  NULL));
        filename = ucode2ascii (uxml_get_attr (*item, L"filename", NULL));
        uxml_free (*item);               /*  Delete previous XML tree         */
        rc = uxml_load (item, pathsym, filename);
        if (pathsym)  mem_free (pathsym);
        if (filename) mem_free (filename);
        return (rc == UXML_NOERROR);
      }
    return (FALSE);
}



/* Functions for generated XML parser.                                       */
/* These functions should not be used by module other than sflxmll.c         */
/* 'uxml_' prefix has been discarded to distinguish them from the 'regular'   */
/* interface                                                                 */


UXML_ITEM *alloc_uxml_item (void)
{
    UXML_ITEM
            *item;

    list_create (item, sizeof (UXML_ITEM));
    if (item)
      {
        list_reset (&item-> attrs);
        list_reset (&item-> children);
        item-> parent = NULL;
        item-> name   = NULL;
        item-> value  = NULL;
      }

    return (item);
}

Bool set_uxml_item_name (UXML_ITEM *item, UCODE *name)
{
    ASSERT (item);
    ASSERT (!item-> name);
    item-> name = name;

    return TRUE;
}

Bool set_uxml_item_value (UXML_ITEM *item, UCODE *value)
{
    UXML_ITEM
        *child;
    UCODE
        *cur;
    ASSERT (item);

    for (cur = value; *cur; cur++)
      {
        if ( !wcschr (L" \t\r\n", *cur) )
          break;
      }

    if (!*cur)                          /* value is only made of blanks */
      {
        if (value) mem_free (value);
        return TRUE;
      }

    child = alloc_uxml_item ();
    ASSERT (child);
    child-> value = value;
    uxml_attach_child (item, child);

    return TRUE;
}


Bool link_uxml_child (UXML_ITEM *parent, UXML_ITEM *item)
{
    if (item-> parent)
        uxml_detach (item);

    item-> parent = parent;
    if (parent)
        list_relink_before (item, &parent-> children);

    return TRUE;
}


Bool check_uxml_closing_name (UXML_ITEM *item, UCODE *name)
{
    Bool
        res;

    ASSERT (item);
    ASSERT (name);

    res = wcscmp (uxml_item_name(item), name) == 0 ? TRUE : FALSE;
    if (name) mem_free (name);

    return res;
}

 /* ------------------------------------------------------------------------- */


UXML_ATTR *alloc_uxml_attr (void)
{
    UXML_ATTR
            *attr;

    list_create (attr, sizeof (UXML_ATTR));
    if (attr)
      {
        attr-> parent = NULL;
        attr-> name   = NULL;
        attr-> value  = NULL;
      }

    return (attr);
}

Bool set_uxml_attr_name (UXML_ATTR *attr, UCODE *name)
{
    ASSERT (attr);
    ASSERT (!attr-> name);
    attr-> name = name;

    return TRUE;
}

Bool set_uxml_attr_value (UXML_ATTR *attr, UCODE *value)
{
    ASSERT (attr);
    ASSERT (!attr-> value);

    attr-> value = value;
    return TRUE;
}

Bool link_uxml_attr (UXML_ITEM *item, UXML_ATTR *attr)
{
    ASSERT (item);
    ASSERT (attr);

    if (uxml_get_attr(item, attr-> name, NULL) != NULL)
        return FALSE;

    attr-> parent = item;
    list_relink_before (attr, &item-> attrs);
    return TRUE;
}


