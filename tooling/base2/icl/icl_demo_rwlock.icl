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
    name      = "icl_demo_rwlock"
    comment   = "icl demo class, used to test the iCL framework"
    version   = "1.1"
    script    = "icl_gen"
    >

<doc>
    Tests and demonstrates the iCL class framework.  This class creates
    a file object that you can open, close, and work with.
</doc>

<inherit class = "icl_object" >
    <option name = "rwlock" value = "1"/>
</inherit>

<context>
    FILE *handle;
</context>

<method name = "new">
    <argument name = "filename" type = "char *">Name of file to open</argument>
    self->handle = fopen (filename, "w+");
</method>

<method name = "destroy">
    fclose (self->handle);
</method>

<method name = "write" template = "function">
    <inherit name = "wrlock"/>
    <argument name = "data" type = "byte *">Pointer to data to write</argument>
    <argument name = "size" type = "size_t">Amount of data, in octets</argument>
    if (fwrite (data, 1, size, self->handle) < (size_t) size)
        rc = FALSE;
</method>

<method name = "read" template = "function">
    <inherit name = "rdlock"/>
    <argument name = "data" type = "byte *">Pointer to receive data</argument>
    <argument name = "size" type = "size_t">Maximum amount to read, in octets</argument>
    if (fread (data, 1, size, self->handle) == 0)
        rc = FALSE;
</method>

<method name = "reset" template = "function">
    <inherit name = "wrlock"/>
    rewind (self->handle);
</method>

<method name = "selftest">
    <local>
    $(selftype)
        *output;
    byte
        buffer [100];
    </local>
    output = icl_demo_rwlock_new ("icl_demo.tst");
    icl_demo_rwlock_write (output, (byte *) "this is a string", 17);
    icl_demo_rwlock_reset (output);
    icl_demo_rwlock_read  (output, buffer, sizeof (buffer));
    icl_demo_rwlock_destroy (&output);
    icl_demo_rwlock_destroy (&output);
</method>

</class>
