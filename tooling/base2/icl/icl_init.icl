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
    name      = "icl_init"
    comment   = "iCL initialisation class"
    version   = "1.1"
    script    = "icl_gen"
    target    = "stdc"
    role      = "base"
    abstract  = "1"
    >
<doc>
    Abstract class to define class initialisation methods.
</doc>

<private name = "header">
static Bool
    s_$(selfname)_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_$(selfname)_mutex      = NULL;
#endif
</private>

<!-- Class initialisation -->

<method name = "initialise" private = "1">
    <header>
    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_$(selfname)_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_$(selfname)_mutex)
            s_$(selfname)_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_$(selfname)_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_$(selfname)_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);
        </header>
        <footer>
            s_$(selfname)_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_$(selfname)_mutex);
#endif

    }
    </footer>
</method>

<method name = "new" abstract = "1">
    <inherit name = "auto init"/>
</method>

<!-- Class termination -->

<method name = "terminate" private = "1">
    <header>
    if (s_$(selfname)_active) {
    </header>
    <footer>
#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_$(selfname)_mutex);
#endif
        s_$(selfname)_active = FALSE;
    }
    </footer>
</method>

<!-- Abstract method to invoke initialiseation -->
<method name = "auto init" abstract = "1">
    <header>
    if (!s_$(selfname)_active)
        self_initialise ();
    </header>
</method>

</class>

