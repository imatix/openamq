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
    name      = "icl_demo_serial"
    comment   = "Demonstrate the serialisation builder"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1" />
</inherit>

<public name = "include">
#include "icl.h"
</public>

<invoke script = "icl_serialise" />
<data>
    <field type = "shortint" name = "channel id"   >Channel number</field>
    <field type = "shortint" name = "handle id"    >Handle number</field>
    <field type = "shortint" name = "service type" >Service type</field>
    <field type = "shortint" name = "confirm tag"  >Confirmation tag</field>
    <field type = "bit"      name = "producer"     >Request producer access</field>
    <field type = "bit"      name = "consumer"     >Request consumer access</field>
    <field type = "bit"      name = "browser"      >Request browser access</field>
    <field type = "bit"      name = "temporary"    >Request a temporary dest</field>
    <field type = "shortstr" name = "dest name"    >Default destination name</field>
    <field type = "shortstr" name = "mime type"    >Default MIME type</field>
    <field type = "shortstr" name = "encoding"     >Default content encoding</field>
    <field type = "longstr"  name = "options"      >Destination options</field>
</data>

<context>
    byte
        data [4096];
    size_t
        size;
</context>

<method name = "selftest">
    <local>
    $(selftype)
        *self;
    </local>

    self = $(selfname)_new ();
    self->size = $(selfname)_save (self, self->data, sizeof (self->data));
    assert (self->size);
    $(selfname)_load (self, self->data, self->size);
    $(selfname)_destroy (&self);
</method>

</class>
