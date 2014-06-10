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
    name      = "icl_demo_thread"
    comment   = "icl demo class, used to test the iCL framework"
    version   = "1.1"
    script    = "icl_gen"
    >

<doc>
    Tests and demonstrates the iCL class framework.  This class creates
    a file object that you can open, close, and work with.
</doc>

<inherit class = "icl_thread" />

<import class = "icl_demo_cache"/>

<option name = "links" value = "1"/>

<private name = "header">
void * APR_THREAD_FUNC s_test (apr_thread_t *apr_thread, void * data);
</private>

<context>
    char *text;
</context>

<method name = "new">
    <argument name = "startup" type = "apr_thread_start_t">Function to start the new thread</argument>
    <argument name = "text" type = "char *"/>
    self->startup = startup;
    self->text    = icl_mem_strdup (text);
</method>

<method name = "destroy">
    icl_mem_free (self->text);
</method>

<private name = "footer">
void * APR_THREAD_FUNC
s_test (apr_thread_t *apr_thread, void *data)
{
    $(selftype)
        *self = data;
    icl_demo_cache_t
        *demo_cache;
    int
        i;

    icl_console_print (self->text);
    for (i = 0; i < 100000; i++) {
        demo_cache = icl_demo_cache_new (NULL,0);
        icl_demo_cache_destroy (&demo_cache);
    }
    icl_console_print (self->text);
    $(selfname)_destroy (&self);
    return NULL;
}
</private>

<method name = "selftest">
    <local>
    icl_demo_thread_t
        *thread_1,
        *thread_2,
        *thread_3,
        *thread_4,
        *thread_5,
        *thread_6,
        *thread_7,
        *thread_8;
    </local>
    icl_console_mode (ICL_CONSOLE_THREAD, TRUE);
    
    thread_1 = icl_demo_thread_new (s_test, "1");
    thread_2 = icl_demo_thread_new (s_test, "2");
    thread_3 = icl_demo_thread_new (s_test, "3");
    thread_4 = icl_demo_thread_new (s_test, "4");
    thread_5 = icl_demo_thread_new (s_test, "5");
    thread_6 = icl_demo_thread_new (s_test, "6");
    thread_7 = icl_demo_thread_new (s_test, "7");
    thread_8 = icl_demo_thread_new (s_test, "8");
    icl_demo_thread_start (thread_8);
    icl_demo_thread_start (thread_7);
    icl_demo_thread_start (thread_6);
    icl_demo_thread_start (thread_5);
    icl_demo_thread_start (thread_4);
    icl_demo_thread_start (thread_3);
    icl_demo_thread_start (thread_2);
    icl_demo_thread_start (thread_1);

    icl_demo_thread_join    (thread_1);
    icl_demo_thread_destroy (&thread_1);

    icl_demo_thread_join    (thread_2);
    icl_demo_thread_destroy (&thread_2);

    icl_demo_thread_join    (thread_3);
    icl_demo_thread_destroy (&thread_3);

    icl_demo_thread_join    (thread_4);
    icl_demo_thread_destroy (&thread_4);

    icl_demo_thread_join    (thread_5);
    icl_demo_thread_destroy (&thread_5);

    icl_demo_thread_join    (thread_6);
    icl_demo_thread_destroy (&thread_6);

    icl_demo_thread_join    (thread_7);
    icl_demo_thread_destroy (&thread_7);

    icl_demo_thread_join    (thread_8);
    icl_demo_thread_destroy (&thread_8);
</method>

</class>
