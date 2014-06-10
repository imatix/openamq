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
    name      = "icl_object"
    comment   = "iCL object class"
    version   = "1.1"
    script    = "icl_gen"
    target    = "stdc"
    role      = "object"
    before    = "base"
    abstract  = "1"
    >

<doc>
    This is the class that provides almost all of the infrastructure for
    the basic classes.

    The functionality it provides consists of:

    1. Allocation/freeing.  Three models are available by setting the
       option 'alloc': "plain" (the default), "cache" "direct", "private"
       and "manual".
       Direct mode doesn't use icl_mem at all and is only used by
       icl_system and icl_mutex to avoid recursion.
       Manual allows the application programmer to do the allocation and
       is used by ipr_bucket.
    2. Read/write locking.  If the option 'rwlock' is non-zero then
       locking is provided in the object.
       If 'create_rwlock' is defined and zero then the _new method doesn't
       create the rwlock.  This is useful for objects that require rwlocks
       in some but not all instances.
    3. Mutex locking.  Simpler than read/write locking, but no 'read lock'
       method.  Enable with option 'mutex'.
    4. Reference counting.  If the option 'links' is non-zero then
       the object is made safe from unexpected destruction.  The 'destroy'
       method 'zombifies' the object and defers freeing the method until
       the reference count is zero.  The 'unlink' method defers both
       zombification and freeing until the reference count is zero.
</doc>

<inherit name = "icl_base"/>
<inherit name = "icl_init" condition = "alloc ?= &quot;local&quot;" />

<import class = "icl_mem" />
<import class = "icl_system" />

<invoke>
.#  Links option must be copied to top-level now for containers to work.
.class.links ?= inherit->option (name = "links"). value ?

.#  Set default value for allocator
.class.alloc = alloc ? "plain"

.#  Generate property access methods
.my.methods = ""
.for class.context
.   for property
.       property.readonly ?= context.readonly? 0    #   Object not yet fully parsed
.       my.methods += '&lt;method name = "$(name)" return = "$(name)"&gt;'
.       my.methods += '&lt;argument name = "self" type = "$\(selftype) *" /&gt;'
.       my.methods += '&lt;declare name = "$(name)" type = "$(type)" /&gt;'
.       my.methods += property->get.
.       my.methods += '&lt;/method&gt;'
.       if .[readonly] = 0
.           my.methods += '&lt;method name = "set $(name)"&gt;'
.           my.methods += '&lt;argument name = "self" type = "$\(selftype) *" /&gt;'
.           my.methods += '&lt;argument name = "$(name)" type = "$(type)" /&gt;'
.           my.methods += property->put.
.           my.methods += '&lt;/method&gt;'
.       endif
.   endfor
.endfor
.#   Load calculated methods into class
.if my.methods <> ""
.   class.load_string (my.methods, errorstr)
.   if defined (errorstr)
.       abort errorstr
.   endif
.endif
</invoke>

<public name = "header">
.if links ? 0
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
#    define $(class.name:UPPER,c)_HISTORY_LENGTH $(class.history_length ? 32)
#endif
.endif

#define $(selfname:UPPER)_ALIVE         0xFABB
#define $(selfname:UPPER)_DEAD          0xDEAD

#define $(selfname:upper)_ASSERT_SANE(self)\\
{\\
    if (!self) {\\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\\
        icl_system_panic ("", "Attempting to work with a NULL $(class.name:)\\n");\\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\\n");\\
        abort ();\\
    }\\
    if (self->object_tag != $(selfname:UPPER)_ALIVE) {\\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\\
        icl_system_panic ("", "$(class.name:) at %p expected object_tag=0x%x, actual object_tag=0x%x\\n", self, $(selfname:UPPER)_ALIVE, self->object_tag);\\
.if alloc ? "plain" <> "direct"
        $(selfname)_show (self, ICL_CALLBACK_DUMP, stderr);\\
.endif
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\\n");\\
        abort ();\\
    }\\
}
</public>

<context>
.if alloc = "local"
    $(selftype)
        *local_next;
.endif
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
.endif
.if mutex ? 0
#if (defined (BASE_THREADSAFE))
    icl_mutex_t *
        mutex;
#endif
.endif
.if links ? 0
    volatile int
        links;                          //  Number of links to the item
    volatile qbyte
        zombie;                         //  Object destroyed but not freed
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    //  Possession history, for tracing
    volatile qbyte
        history_last;
    char
        *history_file [$(class.name:UPPER,c)_HISTORY_LENGTH];
    int
        history_line  [$(class.name:UPPER,c)_HISTORY_LENGTH];
    char
        *history_type [$(class.name:UPPER,c)_HISTORY_LENGTH];
    int
        history_links [$(class.name:UPPER,c)_HISTORY_LENGTH];
#endif
.endif
    dbyte
        object_tag;                     //  Object validity marker
</context>

<method name = "initialise" condition = "alloc ?= &quot;local&quot;">
.if alloc = "local"
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_threadkey_private_create (&threadkey, NULL, icl_global_pool));
#endif
.endif
</method>

<method name = "terminate">
.if alloc = "local"
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_threadkey_private_delete (threadkey));
#endif
.endif
</method>

<method name = "new" template = "constructor">
    <header>
    self = $(base)alloc_ (\
.my.alloc = class->method (name = "alloc")
.for my.alloc.argument
$(name:c)$(!last()??", "?)\
.endfor
);
    if (self) {
        self->object_tag   = $(selfname:UPPER)_ALIVE;
.if links ? 0
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
.endif
.
.if rwlock ? 0
.   if create_rwlock ? 1
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
.   else
#if (defined (BASE_THREADSAFE))
        self->rwlock = NULL;
#endif
.   endif
.endif
.if mutex ? 0
#if (defined (BASE_THREADSAFE))
        self->mutex = icl_mutex_new ();
#endif
.endif
.if alloc = "plain" | alloc = "cache" | alloc = "private"
#if defined (DEBUG)
        icl_mem_set_callback (self, $(base)show_);
#endif
.endif
    </header>
    <footer>
    }
    </footer>
</method>

<method name = "show">
    <argument name = "item" type = "void *">The opaque pointer</argument>
    <argument name = "opcode" type = "int">The callback opcode</argument>
    <argument name = "trace_file" type = "FILE *">File to print to</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number</argument>
    <local>
    $(selftype)
        *self;
    int
        container_links;
.if links ? 0
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    qbyte
        history_index;
#endif
.endif
    </local>
    <header>
    self = item;
    container_links = 0;
    </header>
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    &lt;$(selfname)\
.if links ? 0
 zombie = \\"%u\\" links = \\"%u\\" containers = \\"%u\\"\
.endif
 file = \\"%s\\" line = \\"%lu\\"\
  pointer = \\"%p\\" />\\n\"\
.if links ? 0
, self->zombie, self->links, container_links\
.endif
, file, (unsigned long) line\
, self);
.if links ? 0
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    if (self->history_last > $(class.name:UPPER,c)_HISTORY_LENGTH) {
        fprintf (trace_file, "        &lt;!-- possess history too large (%d) - call iMatix-tech -->\\n\",
            self->history_last);
        history_index = (self->history_last + 1) % $(class.name:UPPER,c)_HISTORY_LENGTH;
        self->history_last %= $(class.name:UPPER,c)_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % $(class.name:UPPER,c)_HISTORY_LENGTH) {
        fprintf (trace_file, "       &lt;%s file = \\"%s\\" line = \\"%lu\\" links = \\"%lu\\" />\\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    &lt;/$(selfname)>\\n");
#endif
.endif
</method>

<method name = "destroy public" template = "destructor" cname = "destroy">
    <!--  Disable animation of file and line, then redefine it, just to make
          sure that they are the last items in the animation.               -->
    <argument name = "file" animate = "0">Source file</argument>
    <argument name = "line" animate = "0">Line number</argument>
    <animate  name = "file" value = "file" format = "%s" />
    <animate  name = "line" value = "line" format = "%i" />
    <animate  name = "links" value = "self?self->links:0" format = "%i" condition = "links">The number of links to the object</animate>
    <animate  name = "zombie" value = "self?self->zombie:0" format = "%i" condition = "links">Is the referenced object a zombie</animate>
    <doc>
    Performs an agressive destroy of an object.  This involves:
    1. Removing the object from any containers it is in.
    2. Making the object an zombie
    3. Decrementing the object's link count
    4. If the link count is zero then freeing the object.
    </doc>
    if (self) {
.if links ? 0
        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            $(base)annihilate_ (self_p, file, line);

        if (self->links == 0) {
            icl_system_panic ("$(selfname)", "E: missing link on $(selfname) object");
            $(base)show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
            $(base)free_ (($(selftype) *) self, file, line);
.else
        $(base)annihilate (self_p);
        $(base)free (($(selftype) *) self);
.endif
        *self_p = NULL;
    }
</method>

<method name = "destroy" cname = "annihilate" private = "1">
    <argument name = "self" type = "$(selftype) *" ref = "1">Reference to object reference</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__" condition = "links ? 0">Source file</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__" condition = "links ? 0">Line number</argument>
    <local>
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif
.endif
.if mutex ? 0
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif
.endif
.if links ? 0
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    int
        history_last;
#endif
.endif
    </local>
    <header>
.if links ? 0
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = file;
    self->history_line  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = line;
    self->history_type  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = self->links;
#endif

.endif
    $(selfname:upper)_ASSERT_SANE (self);
.if icl.container ? 0
    $(selfname)_remove_from_all_containers (self);
.endif
.
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif
.endif
.if mutex ? 0
#if (defined (BASE_THREADSAFE))
    mutex = self->mutex;
    if (mutex)
         icl_mutex_lock (mutex);
#endif
.endif
    </header>
    <footer>
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif
.endif
.if mutex ? 0
#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif
.endif
    </footer>
</method>

<method name = "alloc" return = "self" private = "1">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare name = "self" type = "$(selftype) *" default = "NULL">Object reference</declare>
    <local>
.if alloc = "local"
#if (defined (BASE_THREADSAFE))
    $(selftype)
        **s_cache;
    void
        *s_cache_p;
#endif
.endif
    </local>
    <header>
.if alloc = "plain"
    self = ($(selftype) *) icl_mem_alloc_ (sizeof ($(selftype)), file, line);
.elsif alloc = "direct"
    self = ($(selftype) *) malloc (sizeof ($(selftype)));
.elsif alloc = "cache"
    //  Initialise cache if necessary
    if (!s_cache)
        $(base)cache_initialise ();

    self = ($(selftype) *) icl_mem_cache_alloc_ (s_cache, file, line);
.elsif alloc = "local"
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_threadkey_private_get (&s_cache_p, threadkey));
    s_cache = s_cache_p;
    if (!s_cache) {
        s_cache = malloc (sizeof ($(selftype) **));
        *s_cache = NULL;
        icl_apr_assert (apr_threadkey_private_set (s_cache, threadkey));
    }
    else
        self = *s_cache;

    if (self) {
        *s_cache = self->local_next;
        self->local_next = NULL;
    }
    else
        self = ($(selftype) *) malloc (sizeof ($(selftype)));
#else
    if (s_cache) {
        self = s_cache;
        s_cache = self->local_next;
        self->local_next = NULL;
    }
    else
        self = ($(selftype) *) malloc (sizeof ($(selftype)));
#endif
.elsif alloc = "private"
.#   Use a dedicated cache for this object.  In this case it is assumed that
.#   the "alloc" code in the object only needs to be executed it the object
.#   wasn't cached.
    //  Initialise cache if necessary
    if (!s_cache)
        $(base)cache_initialise ();

    self = ($(selftype) *) icl_mem_cache_pop_ (s_cache, file, line);
    if (!self) {
        self = ($(selftype) *) icl_mem_cache_new_ (s_cache, file, line);
.elsif alloc = "manual"
.#   Nothing here - application programmer must write allocation code
.else
.    abort "Unknown allocation method: $(alloc)"
.endif
.   if nullify ? 1
    if (self)
        memset (self, 0, sizeof ($(selftype)));
.   endif
    </header>
    <footer>
.if alloc = "private"
    }
.endif
    </footer>
</method>

<method name = "free" private = "1">
    <argument name = "self" type = "$(selftype) *">Object reference</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__" condition = "links ? 0">Source file</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__" condition = "links ? 0">Line number</argument>
    <doc>
    Freess a $(selftype) object.
    </doc>
    <local>
.if alloc = "local"
#if (defined (BASE_THREADSAFE))
    $(selftype)
        **s_cache;
    void
        *s_cache_p;
#endif
.endif
.if links ? 0
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    int
        history_last;
#endif
.endif
    </local>
    <header>
    if (self) {
.if links ? 0
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = file;
        self->history_line  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = line;
        self->history_type  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = "free";
        self->history_links [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = self->links;
#endif
.endif
    </header>
    <footer>
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
.endif
.if mutex ? 0
#if (defined (BASE_THREADSAFE))
    if (self->mutex)
        icl_mutex_destroy (&self->mutex);
#endif
.endif
.if nullify ? 1
        memset (&self->object_tag, 0, sizeof ($(selftype)) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof ($(selftype)));
.endif
.if alloc = "direct"
        self->object_tag = $(selfname:UPPER)_DEAD;
        free (self);
.elsif alloc = "plain" | alloc = "cache" | alloc = "private"
        self->object_tag = $(selfname:UPPER)_DEAD;
        icl_mem_free (self);
.elsif alloc = "local"
        self->object_tag = $(selfname:UPPER)_DEAD;
#if (defined (BASE_THREADSAFE))
        icl_apr_assert (apr_threadkey_private_get (&s_cache_p, threadkey));
        s_cache = s_cache_p;
        self->local_next = *s_cache;
        *s_cache = self;
#else
        self->local_next = s_cache;
        s_cache = self;
#endif
.elsif alloc = "manual"
.#   Nothing here - application programmer must write freeing code
.endif
    }
    self = NULL;
    </footer>
</method>

<!-- Do something to an object, return success/failure
     The _ASSERT_SANE macro MUST be implemented by all (abstract) classes
     that use icl_base to define functions.  See icl_object.icl for an example.
  -->

<method name = "function" abstract = "1">
    <header>
.if links ? 0
    if (!self->zombie) {
.endif
    </header>
    <footer>
.if links ? 0
    }
    else
        rc = -1;                        //  Return error on zombie object.
.endif
</footer>
</method>

<method name = "reject zombie" abstract = "1">
<header>
    $(selfname:upper)_ASSERT_SANE (self);
.if links ? 0
    if (!self->zombie) {
.endif
</header>
<footer>
.if links ? 0
    }
.endif
</footer>
</method>

<method name = "scope" abstract = "1">
    <doc>
    This abstract method can be inherited into a method.  When this is done,
    a scope called 'scope' is opened at the start of the method.  Objects
    within the class may then be allocated inside the scope with their
    'new in scope' method.  The scope, and thus all such objects, are destroyed
    at the end of this method.
    </doc>
    <local>
    icl_scope_t
        *scope;
    </local>
    <header>
    scope = icl_scope_new ();
    </header>
    <footer>
    icl_scope_destroy (&scope);
    </footer>
</method>

<!--  The <invoke> below serves to build an inline new_in_scope method that
      takes the same argument list as the new method, plus a 'scope' argument.
      The method just generated does as its name suggests: creates a new
      object and places it inside the scope so that it can be destroyed
      with the scope.

     If the class nas no 'new' method or the 'new' method is private
      then no 'new in scope' method is created.                             -->
<invoke phase = "parse">
.my.new = class->method (name = "new") ?
.my.destroy = class->method (name = "new") ?
.if defined (my.new) & my.new.private ? 0 = 0 & defined (my.destroy) & my.new.destroy ? 0 = 0
.   new class.method
.       method.name = "new in scope"
.#       method.inline = "1"
.       new method.argument
.           argument.name = "self_p"
.           argument.type = "$(selftype) * *"
.       endnew
.       new method.argument
.           argument.name = "_scope"
.           argument.type = "icl_scope_t *"
.       endnew
.       for my.new.argument where !defined (my.new->dismiss (0.argument = argument.name))
.           copy argument to method
.       endfor
.       new method.declare
.           declare.name = "_destroy"
.           declare.type = "icl_destroy_t *"
.       endnew
.       method. = "
    *self_p = $(selfname:c)_new_ ("
.       for my.new.argument where !defined (my.new->dismiss (0.argument = argument.name))
.           method. += "$(argument.name:c)$(!last()??','?)"
.       endfor
.       method. += ");

    if (*self_p) {
        _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) $(selfname)_destroy_);
        icl_destroy_list_queue (_scope, _destroy);
        icl_destroy_unlink (&_destroy);
    }
"
.   endnew
.endif
</invoke>

<method name = "read lock" template = "function" condition = "rwlock">
#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif
</method>

<method name = "write lock" template = "function" condition = "rwlock">
#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif
</method>

<method name = "lock" template = "function" condition = "mutex">
#if (defined (BASE_THREADSAFE))
    icl_mutex_lock (self->mutex);
#endif
</method>

<method name = "unlock" template = "function" condition = "rwlock?0 | mutex?0">
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif
.endif
.if mutex ? 0
#if (defined (BASE_THREADSAFE))
    icl_mutex_unlock (self->mutex);
#endif
.endif
</method>

<method name = "link" return = "self" condition = "links">
    <argument name = "self" type = "$(selftype) *"/>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <animate  name = "links" value = "self?self->links:0" format = "%i">The number of links to the object</animate>
    <animate  name = "zombie" value = "self?self->zombie:0" format = "%i">Is the referenced object a zombie</animate>
    <doc>
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    </doc>
    <local>
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    int
        history_last;
#endif
    </local>
    if (self) {
        $(selfname:upper)_ASSERT_SANE (self);
.if alloc = "plain" | alloc = "cache" | alloc = "private"
        if (file)
            icl_mem_possess_ (self, file, line);
.endif
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = file;
        self->history_line  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = line;
        self->history_type  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = "link";
        self->history_links [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = self->links;
#endif
    }
</method>

<method name = "unlink" condition = "links">
    <argument name = "self" type = "$(selftype) *" ref = "1" animate = "0">Reference to object reference</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__" animate = "0">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__" animate = "0">Line number for call</argument>
    <animate  name = "file" value = "file" format = "%s" />
    <animate  name = "line" value = "line" format = "%i" />
    <animate  name = "links" value = "self?self->links:0" format = "%i" condition = "links">The number of links to the object</animate>
    <animate  name = "zombie" value = "self?self->zombie:0" format = "%i" condition = "links">Is the referenced  object a zombie</animate>
    <doc>
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    </doc>
    <local>
.if links ? 0
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
    int
        history_last;
#endif
.endif
    </local>
    <header>
    if (self) {
.if links ? 0
        $(selfname:upper)_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("$(selfname)", "E: missing link on $(selfname) object");
            $(base)show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_$(class.name:UPPER,c))
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = file;
        self->history_line  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = line;
        self->history_type  [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % $(class.name:UPPER,c)_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
.endif
    </header>
    <footer>
.if links ? 0
            if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
                $(base)annihilate_ (self_p, file, line);
            $(base)free_ (($(selftype) *) self, file, line);
        }
.endif
        *self_p = NULL;
    }
    </footer>
</method>

<method name = "wrlock" abstract = "1">
    <local>
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif
.endif
    </local>
    <header>
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif
.endif
    </header>
    <footer>
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif
.endif
    </footer>
</method>

<method name = "rdlock" abstract = "1" >
    <local>
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif
.endif
    </local>
    <header>
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif
.endif
    </header>
    <footer>
.if rwlock ? 0
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif
.endif
    </footer>
</method>

<method name = "mutex" abstract = "1" >
    <local>
.if mutex ? 0
#if (defined (BASE_THREADSAFE))
    icl_mutex_t
        *mutex;
#endif
.endif
    </local>
    <header>
.if mutex ? 0
#if (defined (BASE_THREADSAFE))
    mutex = self ? self->mutex : NULL;
    if (mutex)
        icl_mutex_lock (mutex);
#endif
.endif
    </header>
    <footer>
.if mutex ? 0
#if (defined (BASE_THREADSAFE))
    if (mutex)
        icl_mutex_unlock (mutex);
#endif
.endif
    </footer>
</method>

<private name = "header">
.if alloc = "cache" | alloc = "private"
.
static icl_cache_t
    *s_cache = NULL;

.elsif alloc = "local"
#if (defined (BASE_THREADSAFE))
static apr_threadkey_t
    *threadkey;                         //  To hold thread-local cache header
#else
static $(selftype)
        *s_cache = NULL;
#endif
.endif
</private>

<method name = "cache initialise" private = "1"
        condition = "alloc ?= &quot;cache&quot; | alloc ?= &quot;private&quot;">
    <doc>
    Initialise the cache and register purge method with the meta-cache.
    </doc>
.if alloc = "cache"
    s_cache = icl_cache_get (sizeof ($(selftype)));
.elsif alloc = "private"
    s_cache = icl_cache_create (sizeof ($(selftype)));
.endif
    icl_system_register ($(base)cache_purge, $(base)cache_terminate);
</method>

<method name = "cache purge" private = "1"
        condition = "alloc ?= &quot;cache&quot; | alloc ?= &quot;private&quot;">
.if alloc ?= "cache" | alloc = "private"
    icl_mem_cache_purge (s_cache);
.endif
</method>

<method name = "cache terminate" private = "1"
        condition = "alloc ?= &quot;cache&quot; | alloc ?= &quot;private&quot;">
.if alloc ?= "cache" | alloc = "private"
    s_cache = NULL;
.endif
</method>

</class>
