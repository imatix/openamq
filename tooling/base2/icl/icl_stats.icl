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
    name      = "icl_stats"
    comment   = "Statistics class"
    version   = "1.1"
    script    = "icl_gen"
    >
<doc>
    Provides statistical reporting for other classes.

    Statistics are held in a shared memory block so they can easily be
    examined from another process, even if the running process is blocked.
</doc>

<inherit class = "icl_base" />

<import class = "icl_apr" />
<import class = "icl_system" />

<public name = "header">
typedef volatile unsigned int icl_stats_t;
</public>

<private name = "header">
#define ICL_STATS_SHARE_SIZE         32768
#define ICL_STATS_SHARE_DEFAULT_NAME "icl_stats"

#define BYTE_ALIGNMENT               4

//  Static variables

static apr_shm_t
    *s_share = NULL;
static char
    *s_share_start,
    *s_share_end;

static Bool
    s_icl_stats_active = FALSE;
#if (defined (BASE_THREADSAFE))
static apr_thread_mutex_t
    *s_icl_stats_mutex  = NULL;
#endif
</private>

<method name = "initialise" private = "1">
    <local>
    char
        share_name [128];
    </local>
#if (defined (BASE_THREADSAFE))
    //  Don't use the icl lock functions or we'll get recursion.
    apr_thread_mutex_lock (icl_global_mutex);
    if (!s_icl_stats_mutex)
        icl_apr_assert (apr_thread_mutex_create (&s_icl_stats_mutex, APR_THREAD_MUTEX_DEFAULT, icl_global_pool));
    apr_thread_mutex_lock   (s_icl_stats_mutex);
    apr_thread_mutex_unlock (icl_global_mutex);
#endif
    if (!s_icl_stats_active) {
        s_icl_stats_active = TRUE;

        if (g_argc > 0) {
            strncpy (share_name, g_argv [0], 120);
            share_name [120] = 0;
        }
        else
            share_name [0] = 0;

        strcat (share_name, "_stats");

        //  Remove share before creating it to avoid clashes.
        apr_shm_remove (share_name, icl_global_pool);
        icl_apr_assert (apr_shm_create (
            &s_share, ICL_STATS_SHARE_SIZE, share_name, icl_global_pool));
        s_share_start = apr_shm_baseaddr_get (s_share);
        s_share_end   = s_share_start;
        *s_share_end  = 0;

        //  Register the class termination call-back functions
        icl_system_register (NULL, icl_stats_terminate);
    }
#if (defined (BASE_THREADSAFE))
    apr_thread_mutex_unlock (s_icl_stats_mutex);
#endif
</method>

<method name = "terminate" private = "1">
    if (s_icl_stats_active) {
#if (defined (BASE_THREADSAFE))
        apr_thread_mutex_destroy (s_icl_stats_mutex);
        s_icl_stats_mutex = NULL;
#endif
        s_icl_stats_active = FALSE;
    }
</method>

<method name = "new" return = "pointer">
    <argument name = "name" type = "const char *">The name of the statistic</argument>
    <declare name = "pointer" type = "icl_stats_t *" default = "0">Pointer to 32-bit statistic</declare>
    <local>
    size_t
        remaining_space,
        name_length;
    </local>
    if (!s_icl_stats_active)
        icl_stats_initialise ();

#if (defined (BASE_THREADSAFE))
    apr_thread_mutex_lock (s_icl_stats_mutex);
#endif
    remaining_space = ICL_STATS_SHARE_SIZE - (s_share_end - s_share_start);
    name_length = strlen (name) + 1;    //  Including null terminator
    name_length = ((name_length + BYTE_ALIGNMENT - 1) / BYTE_ALIGNMENT) * BYTE_ALIGNMENT;

    if (remaining_space >= name_length + 5) {
        strncpy (s_share_end, name, name_length);
        pointer = (icl_stats_t *) ((byte *) s_share_end + name_length);
        *pointer = 0;
        s_share_end += name_length + 4;
        *s_share_end = 0;
    }
    else {
        icl_system_panic ("icl_stats", "E: shared memory too small, increase ICL_STATS_SHARE_SIZE");
        abort ();
    }
#if (defined (BASE_THREADSAFE))
    apr_thread_mutex_unlock (s_icl_stats_mutex);
#endif
</method>

<method name = "dump">
    <local>
    char *
        start;
    size_t
        name_length;
    </local>
    icl_console_print ("----------------- START OF STATISTICS DUMP -----------------");
    start = s_share_start;
    while (start < s_share_end) {
        name_length = strlen (start) + 1;    //  Including null terminator
        name_length = ((name_length + BYTE_ALIGNMENT - 1) / BYTE_ALIGNMENT) * BYTE_ALIGNMENT;
        icl_console_print ("%s: %u", start, * (icl_stats_t *) (start + name_length));
        start += name_length + 4;
    }
    icl_console_print ("------------------ END OF STATISTICS DUMP ------------------");
</method>

<method name = "inc" inline = "1">
    <argument name = "name" type = "const char *">The name of the statistic</argument>
    <argument name = "stat" type = "icl_stats_t **">The statistic to increment</argument>
    if (!*stat)
        *stat = icl_stats_new (name);
    icl_atomic_inc32 (*stat);
</method>

<method name = "dec" inline = "1">
    <argument name = "name" type = "const char *">The name of the statistic</argument>
    <argument name = "stat" type = "icl_stats_t **">The statistic to decrement</argument>
    if (!*stat)
        *stat = icl_stats_new (name);
    icl_atomic_dec32 (*stat);
</method>

<method name = "add" inline = "1">
    <argument name = "name" type = "const char *">The name of the statistic</argument>
    <argument name = "stat"   type = "icl_stats_t **">The statistic to add to</argument>
    <argument name = "value"  type = "icl_stats_t">The value to add to the statistic</argument>
    if (!*stat)
        *stat = icl_stats_new (name);
    icl_atomic_add32 (*stat, value);
</method>

<method name = "assign" inline = "1">
    <argument name = "name" type = "const char *">The name of the statistic</argument>
    <argument name = "stat"  type = "icl_stats_t **">The statistic to assign to</argument>
    <argument name = "value" type = "icl_stats_t">The value to assign to the statistic</argument>
    if (!*stat)
        *stat = icl_stats_new (name);
    icl_atomic_set32 (*stat, value);
</method>

<method name = "max" inline = "1">
    <argument name = "name" type = "const char *">The name of the statistic</argument>
    <argument name = "stat"   type = "icl_stats_t **">The statistic to add to</argument>
    <argument name = "value"  type = "icl_stats_t">The value to add to the statistic</argument>
    <local>
    icl_stats_t
        cur_value,
        new_value;
    </local>
    if (!*stat)
        *stat = icl_stats_new (name);
    cur_value = icl_atomic_get32 (*stat);
    while (value > cur_value) {
        new_value = icl_atomic_cas32 (*stat, value, cur_value);
        if (new_value == cur_value)
            cur_value = value;
        else
            cur_value = new_value;
    }
</method>

<method name = "min" inline = "1">
    <argument name = "name" type = "const char *">The name of the statistic</argument>
    <argument name = "stat"   type = "icl_stats_t **">The statistic to add to</argument>
    <argument name = "value"  type = "icl_stats_t">The value to add to the statistic</argument>
    <local>
    icl_stats_t
        cur_value,
        new_value;
    </local>
    if (!*stat)
        *stat = icl_stats_new (name);
    cur_value = icl_atomic_get32 (*stat);
    while (value < cur_value) {
        new_value = icl_atomic_cas32 (*stat, value, cur_value);
        if (cur_value == cur_value)
            cur_value = value;
        else
            cur_value = new_value;
    }
</method>

<method name = "selftest" export = "none">
    <local>
    icl_stats_t
        *stat;
    </local>
    stat = icl_stats_new ("My test statistic");
    icl_stats_assign ("My test statistic", &stat, 123456789);
    icl_stats_add    (NULL, &stat, 876543210);
</method>

</class>
