<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 -->
<class
    name      = "icl_tree_head"
    comment   = "Abstract base class for Red-Black balanced tree container"
    version   = "1.0"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
    Abstract base class for balanced tree head container
</doc>

<inherit name = "icl_base"/>
<inherit name = "icl_init"/>

<import class = "icl_system" />

<public name = "header">
//  Red-Black tree description

typedef enum {BLACK, RED} $(selfname)_colour;

//  Our own type definition

typedef struct _$(childtype)
    $(selftype);

</public>

<private name = "header">
$(childtype)
    $(selfname)_empty;

#define $(selfname)_null &$(selfname)_empty
</private>

<method name = "initialise">
    memset (&$(selfname)_empty, 0, sizeof ($(selftype)));
    $(selfname)_empty.$(prefix)_left   = &$(selfname)_empty;
    $(selfname)_empty.$(prefix)_right  = &$(selfname)_empty;
    $(selfname)_empty.$(prefix)_parent = NULL;
    $(selfname)_empty.$(prefix)_colour = BLACK;
</method>

<method name = "new" template = "constructor">
    self = $(selfname)_null;
</method>

<method name = "destroy" template = "destructor">
    <local>
    $(childtype)
        *item;
    </local>
    item = $(selfname)_first (self);
    while (item) {
.if link_to_child ? child_has_links ? 0
        $(childname)_unlink (&item);
.else
        $(childname)_destroy (&item);
.endif
        item = $(selfname)_first (self);
    }
</method>

<method name = "insert" return = "rc">
    <argument name = "self_p" type = "$(selftype) **">Reference to object</argument>
    <argument name = "item" type = "$(childtype) *" />
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(childtype)
       *current,
       *parent;
    int
        cmp = 0;
    </local>
    /* find where node belongs */
    current = *self_p;
    parent  = NULL;
    while (current != $(selfname)_null) {
        parent  = current;
        cmp = $(selfname)_compare (current, item);
        if (cmp < 0)
            current = current->$(prefix)_right;
        else
        if (cmp > 0)
            current = current->$(prefix)_left;
        else {
.if allow_duplicates ? 1
             current = current->$(prefix)_left;
.else
             rc = -1;
.endif
        }
    }
    if (!rc) {
        /* setup new node */
        item->$(prefix)_parent = parent;
        item->$(prefix)_left   = $(selfname)_null;
        item->$(prefix)_right  = $(selfname)_null;
        item->$(prefix)_colour = RED;

        /* insert node in tree */
        if (parent) {
            if (cmp < 0)
                parent->$(prefix)_right = item;
            else
                parent->$(prefix)_left  = item;
        }
        else
            *self_p = item;

        $(selfname)_insert_fixup (self_p, item);
    }
</method>

<method name = "remove">
    <argument name = "self_p" type = "$(selftype) **"/>
    <argument name = "item" type = "$(childtype) *" />
    <local>
    $(childtype)
       *youngest, *descendent;
    $(selfname)_colour
        colour;
    </local>
    if (item && item != $(selfname)_null) {
        if (((($(childtype) *) item)->$(prefix)_left  == $(selfname)_null)
        ||  ((($(childtype) *) item)->$(prefix)_right == $(selfname)_null))
            /* descendent has a $(selfname)_null node as a child */
            descendent = item;
        else
          {
            /* find tree successor with a $(selfname)_null node as a child */
            descendent = (($(childtype) *) item)->$(prefix)_right;
            while (descendent->$(prefix)_left != $(selfname)_null)
                descendent = descendent->$(prefix)_left;
          }

        /* youngest is descendent's only child, if there is one, else $(selfname)_null */
        if (descendent->$(prefix)_left != $(selfname)_null)
            youngest = descendent->$(prefix)_left;
        else
            youngest = descendent->$(prefix)_right;

        /* remove descendent from the parent chain */
        if (youngest != $(selfname)_null)
            youngest->$(prefix)_parent = descendent->$(prefix)_parent;
        if (descendent->$(prefix)_parent)
            if (descendent == descendent->$(prefix)_parent->$(prefix)_left)
                descendent->$(prefix)_parent->$(prefix)_left  = youngest;
            else
                descendent->$(prefix)_parent->$(prefix)_right = youngest;
        else
            *self_p = youngest;

        colour = descendent->$(prefix)_colour;

        if (descendent != ($(childtype) *) item)
          {
            /* Conceptually what we are doing here is moving the data from       */
            /* descendent to tree.  In fact we do this by linking descendent     */
            /* into the structure in the place of tree.                          */
            descendent->$(prefix)_left   = (($(childtype) *) item)->$(prefix)_left;
            descendent->$(prefix)_right  = (($(childtype) *) item)->$(prefix)_right;
            descendent->$(prefix)_parent = (($(childtype) *) item)->$(prefix)_parent;
            descendent->$(prefix)_colour = (($(childtype) *) item)->$(prefix)_colour;

            if (descendent->$(prefix)_parent)
              {
                if (item == descendent->$(prefix)_parent->$(prefix)_left)
                    descendent->$(prefix)_parent->$(prefix)_left  = descendent;
                else
                    descendent->$(prefix)_parent->$(prefix)_right = descendent;
              }
            else
                *self_p = descendent;

            if (descendent->$(prefix)_left != $(selfname)_null)
                descendent->$(prefix)_left->$(prefix)_parent = descendent;

            if (descendent->$(prefix)_right != $(selfname)_null)
                descendent->$(prefix)_right->$(prefix)_parent = descendent;
          }

        if ((youngest != $(selfname)_null)
        &&  (colour   == BLACK))
            $(selfname)_delete_fixup (self_p, youngest);
    }
</method>

<method name = "find eq" return = "found">
    <argument name = "self" type = "$(selftype) *"/>
    <argument name = "item" type = "$(childtype) *" />
    <declare name = "found" type = "$(childtype) *" />
    <local>
    $(childtype)
       *current = self;
    int
        cmp;
    </local>
    found = NULL;
    while (current != $(selfname)_null) {
        cmp = $(selfname)_compare (current, item);
        if (cmp < 0)
            current = current->$(prefix)_right;
        else
        if (cmp > 0)
            current = current->$(prefix)_left;
        else {
            found = current;            /*  In case of duplicates,           */
            current = current->$(prefix)_left;  /*  get the first one.       */
        }
    }
</method>

<method name = "find lt" return = "found">
    <argument name = "self" type = "$(selftype) *"/>
    <argument name = "item" type = "$(childtype) *" />
    <declare name = "found" type = "$(childtype) *" />
    <local>
    $(childtype)
       *current = self;
    int
        cmp;
    </local>
    found = NULL;
    while (current != $(selfname)_null) {
        cmp = $(selfname)_compare (current, item);
        if (cmp < 0) {
            found = current;
            current = current->$(prefix)_right;
        }
        else
            current = current->$(prefix)_left;
    }
</method>

<method name = "find le" return = "found">
    <argument name = "self" type = "$(selftype) *"/>
    <argument name = "item" type = "$(childtype) *" />
    <declare name = "found" type = "$(childtype) *" />
    <local>
    $(childtype)
       *current = self;
    int
        cmp;
    </local>
    found = NULL;
    while (current != $(selfname)_null) {
        cmp = $(selfname)_compare (current, item);
        if (cmp > 0)
            current = current->$(prefix)_left;
        else {
            found = current;
                     current = current->$(prefix)_right;
        }
    }
</method>

<method name = "find gt" return = "found">
    <argument name = "self" type = "$(selftype) *"/>
    <argument name = "item" type = "$(childtype) *" />
    <declare name = "found" type = "$(childtype) *" />
    <local>
    $(childtype)
       *current = self;
    int
        cmp;
    </local>
    found = NULL;
    while (current != $(selfname)_null) {
        cmp = $(selfname)_compare (current, item);
        if (cmp > 0) {
            found = current;
            current = current->$(prefix)_left;
        }
        else
            current = current->$(prefix)_right;
    }
</method>

<method name = "find ge" return = "found">
    <argument name = "self" type = "$(selftype) *"/>
    <argument name = "item" type = "$(childtype) *" />
    <declare name = "found" type = "$(childtype) *" />
    <local>
    $(childtype)
       *current = self;
    int
        cmp;
    </local>
    found = NULL;
    while (current != $(selfname)_null) {
        cmp = $(selfname)_compare (current, item);
        if (cmp < 0)
            current = current->$(prefix)_right;
        else {
            found = current;
            current = current->$(prefix)_left;
        }
    }
</method>

<method name = "first" return = "item">
    <argument name = "self" type = "$(selftype) *"/>
    <declare name = "item" type = "$(childtype) *" default = "NULL"/>
    if (self && self != $(selfname)_null) {
        item = self;
        while (item->$(prefix)_left != $(selfname)_null)
            item = item->$(prefix)_left;
    }
</method>

<method name = "last" return = "item">
    <argument name = "self" type = "$(selftype) *"/>
    <declare name = "item" type = "$(childtype) *" default = "NULL"/>
    if (self && self != $(selfname)_null) {
        item = self;
        while (item->$(prefix)_right != $(selfname)_null)
            item = item->$(prefix)_right;
    }
</method>

<method name = "next" return = "item">
    <argument name = "item" type = "$(childtype) *" />
    <local>
    $(childtype)
       *child;
    </local>
    if (item && item != $(selfname)_null) {
        if (item->$(prefix)_right != $(selfname)_null)
            item = $(selfname)_first (item->$(prefix)_right);
        else {
            child   = $(selfname)_null;
            while ((item->$(prefix)_parent)
               &&  (item->$(prefix)_right == child)) {
                child = item;
                item = item->$(prefix)_parent;
            }
            if (item->$(prefix)_right == child)
                item = NULL;
        }
    }
</method>

<method name = "prev" return = "item">
    <argument name = "item" type = "$(childtype) *" />
    <local>
    $(childtype)
       *child;
    </local>
    if (item && item != $(selfname)_null) {
        if (item->$(prefix)_left != $(selfname)_null)
            item = $(selfname)_last (item->$(prefix)_left);
        else {
            while ((item->$(prefix)_parent)
               &&  (item->$(prefix)_left == child)) {
                child = item;
                item = item->$(prefix)_parent;
            }
            if (item->$(prefix)_left == child)
                item = NULL;
        }
    }
</method>

<method name = "insert fixup" private = "1">
    <argument name = "self_p" type = "$(selftype) **" />
    <argument name = "item" type = "$(childtype) *" />
    <local>
    $(childtype)
        *uncle;
    </local>
    /* check red-black properties */
    while ((item != *self_p)
       &&  (item->$(prefix)_parent->$(prefix)_colour == RED)) {
        /* we have a violation */
        if (item->$(prefix)_parent == item->$(prefix)_parent->$(prefix)_parent->$(prefix)_left) {
            uncle = item->$(prefix)_parent->$(prefix)_parent->$(prefix)_right;
            if (uncle->$(prefix)_colour == RED) {
                /* uncle is RED */
                item ->$(prefix)_parent->$(prefix)_colour = BLACK;
                uncle->$(prefix)_colour = BLACK;
                item ->$(prefix)_parent->$(prefix)_parent->$(prefix)_colour = RED;

                item = item->$(prefix)_parent->$(prefix)_parent;
            }
            else {
                /* uncle is BLACK */
                if (item == item->$(prefix)_parent->$(prefix)_right)
                  {
                    /* make tree a left child */
                    item = item->$(prefix)_parent;
                    $(selfname)_rotate_left (self_p, item);
                  }

                /* recolor and rotate */
                item->$(prefix)_parent->$(prefix)_colour = BLACK;
                item->$(prefix)_parent->$(prefix)_parent->$(prefix)_colour = RED;
                $(selfname)_rotate_right (self_p, item->$(prefix)_parent->$(prefix)_parent);
            }
        }
        else {
            /* mirror image of above code */
            uncle = item->$(prefix)_parent->$(prefix)_parent->$(prefix)_left;
            if (uncle->$(prefix)_colour == RED) {
                /* uncle is RED */
                item ->$(prefix)_parent->$(prefix)_colour = BLACK;
                uncle->$(prefix)_colour = BLACK;
                item ->$(prefix)_parent->$(prefix)_parent->$(prefix)_colour = RED;

                item = item->$(prefix)_parent->$(prefix)_parent;
            }
            else {
                /* uncle is BLACK */
                if (item == item->$(prefix)_parent->$(prefix)_left)
                  {
                    item = item->$(prefix)_parent;
                    $(selfname)_rotate_right (self_p, item);
                  }
                item->$(prefix)_parent->$(prefix)_colour = BLACK;
                item->$(prefix)_parent->$(prefix)_parent->$(prefix)_colour = RED;
                $(selfname)_rotate_left (self_p, item->$(prefix)_parent->$(prefix)_parent);
            }
        }
    }
    (*self_p)->$(prefix)_colour = BLACK;
</method>

<method name = "rotate left" private = "1">
    <argument name = "self_p" type = "$(selftype) **" />
    <argument name = "item" type = "$(childtype) *" />
    <local>
    $(childtype)
        *other = item->$(prefix)_right;
    </local>
    /* establish item->$(prefix)_right link */
    item->$(prefix)_right = other->$(prefix)_left;
    if (other->$(prefix)_left != $(selfname)_null)
        other->$(prefix)_left->$(prefix)_parent = item;

    /* establish other->$(prefix)_parent link */
    if (other != $(selfname)_null)
        other->$(prefix)_parent = item->$(prefix)_parent;

    if (item->$(prefix)_parent) {
        if (item == item->$(prefix)_parent->$(prefix)_left)
            item->$(prefix)_parent->$(prefix)_left  = other;
        else
            item->$(prefix)_parent->$(prefix)_right = other;
    }
    else
        *self_p = other;

    /* link item and other */
    other->$(prefix)_left = item;
    if (item != $(selfname)_null)
        item->$(prefix)_parent = other;
</method>

<method name = "rotate right" private = "1">
    <argument name = "self_p" type = "$(selftype) **" />
    <argument name = "item" type = "$(childtype) *" />
    <local>
    $(childtype)
        *other;
    </local>
    other = item->$(prefix)_left;

    /* establish item->$(prefix)_left link */
    item->$(prefix)_left = other->$(prefix)_right;
    if (other->$(prefix)_right != $(selfname)_null)
        other->$(prefix)_right->$(prefix)_parent = item;

    /* establish other->$(prefix)_parent link */
    if (other != $(selfname)_null)
        other->$(prefix)_parent = item->$(prefix)_parent;

    if (item->$(prefix)_parent) {
        if (item == item->$(prefix)_parent->$(prefix)_right)
            item->$(prefix)_parent->$(prefix)_right = other;
        else
            item->$(prefix)_parent->$(prefix)_left  = other;
    }
    else
        *self_p = other;

    /* link item and other */
    other->$(prefix)_right = item;
    if (item != $(selfname)_null)
        item->$(prefix)_parent = other;
</method>

<method name = "delete fixup" private = "1">
    <argument name = "self_p" type = "$(selftype) **" />
    <argument name = "item" type = "$(childtype) *" />
    <local>
    $(childtype)
       *sibling;
    </local>
    while (item != *self_p && item->$(prefix)_colour == BLACK) {
        if (item == item->$(prefix)_parent->$(prefix)_left) {
            sibling = item->$(prefix)_parent->$(prefix)_right;
            if (sibling->$(prefix)_colour == RED) {
                sibling->$(prefix)_colour = BLACK;
                item->$(prefix)_parent->$(prefix)_colour = RED;
                $(selfname)_rotate_left (self_p, item->$(prefix)_parent);
                sibling = item->$(prefix)_parent->$(prefix)_right;
            }
            if ((sibling->$(prefix)_left->$(prefix)_colour == BLACK)
            &&  (sibling->$(prefix)_right->$(prefix)_colour == BLACK)) {
                sibling->$(prefix)_colour = RED;
                item = item->$(prefix)_parent;
            }
            else {
                if (sibling->$(prefix)_right->$(prefix)_colour == BLACK) {
                    sibling->$(prefix)_left->$(prefix)_colour = BLACK;
                    sibling->$(prefix)_colour = RED;
                    $(selfname)_rotate_right (self_p, sibling);
                    sibling = item->$(prefix)_parent->$(prefix)_right;
                }
                sibling->$(prefix)_colour = item->$(prefix)_parent->$(prefix)_colour;
                item->$(prefix)_parent->$(prefix)_colour = BLACK;
                sibling->$(prefix)_right->$(prefix)_colour = BLACK;
                $(selfname)_rotate_left (self_p, item->$(prefix)_parent);
                item = *self_p;
            }
        }
        else {
            sibling = item->$(prefix)_parent->$(prefix)_left;
            if (sibling->$(prefix)_colour == RED) {
                sibling->$(prefix)_colour = BLACK;
                item->$(prefix)_parent->$(prefix)_colour = RED;
                $(selfname)_rotate_right (self_p, item->$(prefix)_parent);
                sibling = item->$(prefix)_parent->$(prefix)_left;
            }
            if ((sibling->$(prefix)_right->$(prefix)_colour == BLACK)
            &&  (sibling->$(prefix)_left->$(prefix)_colour == BLACK)) {
                sibling->$(prefix)_colour = RED;
                item = item->$(prefix)_parent;
            }
            else {
                if (sibling->$(prefix)_left->$(prefix)_colour == BLACK) {
                    sibling->$(prefix)_right->$(prefix)_colour = BLACK;
                    sibling->$(prefix)_colour = RED;
                    $(selfname)_rotate_left (self_p, sibling);
                    sibling = item->$(prefix)_parent->$(prefix)_left;
                }
                sibling->$(prefix)_colour = item->$(prefix)_parent->$(prefix)_colour;
                item->$(prefix)_parent->$(prefix)_colour = BLACK;
                sibling->$(prefix)_left->$(prefix)_colour = BLACK;
                $(selfname)_rotate_right (self_p, item->$(prefix)_parent);
                item = *self_p;
            }
        }
    }
    item->$(prefix)_colour = BLACK;
</method>

</class>

