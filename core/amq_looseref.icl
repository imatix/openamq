<?xml?>
<class
    name      = "amq_looseref"
    comment   = "Loose reference class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >
<doc>
    Allows classes to maintain lists of references to other classes without
    creating explicitly-typed classes for each case.  Uses a void * to refer
    to the target class.
</doc>

<inherit class = "ipr_list_item" />
<option name = "nullify" value = "1" />

<import class = "ipr_classes" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<context>
    void *
        object;                         /*  Target of reference              */
</context>

<method name = "new">
    <argument name = "list"   type = "$(selfname)_list_t *">List to attach to</argument>
    <argument name = "object" type = "void *"              >Target object reference</argument>
    self->object = object;
    $(selfname)_list_queue (list, self);
</method>

<method name = "selftest">
    <local>
    amq_looseref_list_t
        *list;
    </local>

    list = amq_looseref_list_new ();
    amq_looseref_new (list, (void *) list);
    amq_looseref_new (list, (void *) list);
    amq_looseref_new (list, (void *) list);
    amq_looseref_list_destroy (&list);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>
