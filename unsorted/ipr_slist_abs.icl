<?xml?>
<class
    name      = "ipr_slist_abs"
    comment   = "Singly linked list abstract base class definition"
    version   = "1.0b0"
    copyright = "Copyright (c) 2004 iMatix Corporation"
    script    = "icl_gen"
    target    = "iprc"
    abstract  = "1"
    >
<doc>
    Abstract Base class for singly linked list objects.

    To use this class, you must inherit it and:
    - Add a 'item_data' object to the context block.
    - Add a 'data' argument to the new method
    - Implement s_slist_data_copy    (datatype *dest, datatype *source)
    - Implement s_slist_data_destroy (datatype *data)
    - Implement s_slist_data_print   (datatype *data)
</doc>

<public name = "header" >
typedef struct _$(selfname)_list_t $(selfname)_list_t;
</public>

/*
 *  Re-Implementation note: list_data MUST be reimplemented.
 */
<context>
    $(selfname)_list_t *list_head;
    $(selftype)        *item_next;
</context>

<!--  Object allocator  ------------------------------------------------------>

<method name = "list_new" return = "self" >
    <doc>
        Create a new list head
    </doc>
    <declare name = "self" type = "$(selfname)_list_t *" default = "NULL" />

    self = ($(selfname)_list_t *) mem_alloc (sizeof ($(selfname)_list_t));
    memset (self, 0, sizeof ($(selfname)_list_t));
</method>

/*
 *  Re-Implementation note: data argument MUST be reimplemented
 */
<method name = "new" return = "self" >
    <doc>
        Insert an item into the list.
    </doc>
    <argument name = "list" type = "$(selfname)_list_t *" />
    <declare name  = "self" type = "$(selftype) *" default = "NULL" />

    if (list) {
        self = ($(selftype) *) mem_alloc (sizeof ($(selftype)));
        self->list_head = list;
        self->item_next = NULL;
        s_slist_data_copy (&self->item_data, data);

        /* Try to insert this into the list */
        if (s_slist_append (list, self) < 0) {
            mem_free (self);
            self = NULL;
        }
    }
</method>

<!-- Object destructor ------------------------------------------------------->

<method name = "list_destroy" export = "after" return = "rc" >
    <doc>
        Destroy the linked list
    </doc>
    <argument name = "self" type = "$(selfname)_list_t *" />
    <declare  name = "rc"   type = "int" default = "-1"    />

    if (self) {
        while (self->item_head)
            $(selfname)_destroy (self->item_head);

        memset (self, 0, sizeof ($(selfname)_list_t));
        mem_free (self);

        rc = 0;
    }
</method>

<method name = "destroy" return = "rc" export = "after" >
    <doc>
        Remove an item from the hash table.
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <declare  name = "rc"   type = "int" default = "-1" />

    if (self) {
        rc = s_slist_item_unlink (self);
        if (rc >= 0) {
            s_slist_data_destroy (&self->item_data);
            mem_free (self);
            rc = 0;
        }
    }
</method>

<!-- Object accessors -------------------------------------------------------->

<method name = "list_first" return = "item" >
    <doc>
        Return the first item in the list.
    </doc>
    <argument name = "list" type = "$(selfname)_list_t *" />
    <declare  name = "item" type = "$(selftype) *" default = "NULL" />

    if (list) {
        item = list->item_head;
    }
</method>

<method name = "first" return = "item" >
    <doc>
        Return the first item in the list.
    </doc>
    <argument name = "item" type = "$(selftype) *" />

    if (item && item->list_head) {
        item = item->list_head->item_head;
    }
</method>

<method name = "next" return = "item" >
    <doc>
        Return the next item in the list.
    </doc>
    <argument name = "item" type = "$(selftype) *" />

    if (item) {
        item = item->item_next;
    }
</method>

<!-- Misc functions ---------------------------------------------------------->

<method name = "list_print" >
    <doc>
        Prints out the content of the list to stdout.
    </doc>
    <argument name = "list" type = "$(selfname)_list_t *" />
    <local>
        $(selftype) *item;
    </local>

    if (list && list->item_head) {
        item = $(selfname)_list_first (list);
        while (item) {
            s_slist_data_print (&item->item_data);
            item = $(selfname)_next (item);
        }
    }
</method>

<private>

struct _$(selfname)_list_t {
    $(selftype) *item_head;
};

static int s_slist_item_unlink ($(selftype) *item)
{
    $(selftype) *tmp;

    if (!item || !item->list_head)
        return -1;

    /* Item we're trying to unlink is the first one */
    if (item == item->list_head->item_head) {
        item->list_head->item_head = item->list_head->item_head->item_next;
        return 0;
    }

    tmp = item->list_head->item_head;

    while (tmp->item_next && tmp->item_next != item)
        tmp = tmp->item_next;

    /* If we've found the item, unlink it */
    if (tmp->item_next) {
        tmp->item_next = tmp->item_next->item_next;
        return 0;
    }

    /* Item not found in list */
    return -1;
}

static int s_slist_append ($(selfname)_list_t *list, $(selftype) *item)
{
    $(selftype) *tmp;

    if (!list || !item)
        return -1;

    /* First item on the list */
    if (!list->item_head) {
        list->item_head = item;
        return 0;
    }

    tmp = list->item_head;

    while (tmp->item_next)
        tmp = tmp->item_next;

    tmp->item_next = item;
    return 0;
}
</private>

</class>
