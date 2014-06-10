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
    name      = "icl_base"
    comment   = "iCL base class"
    version   = "1.1"
    script    = "icl_gen"
    target    = "stdc"
    role      = "base"
    abstract  = "1"
    >

<doc>
    Defines method templates.
</doc>

<!-- Create a new object, return reference -->

<method name = "constructor" return = "self" abstract = "1">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <doc>
    Creates and initialises a new $(selftype) object, returns a
    reference to the created object.
    </doc>
    <declare name = "self" type = "$(selftype) *" default = "NULL">Object reference</declare>
</method>

<!-- Destroy an object, invalidate parent reference -->

<method name = "destructor" abstract = "1">
    <doc>
    Destroys a $(selftype) object. Takes the address of a
    $(selftype) reference (a pointer to a pointer) and nullifies the
    reference after use.  Does nothing if the reference is already
    null.
    </doc>
    <argument name = "self" type = "$(selftype) *" ref = "1">Reference to object reference</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number</argument>
</method>

<!-- Do something to an object, return success/failure
     The _ASSERT_SANE macro MUST be implemented by all (abstract) classes
     that use icl_base to define functions.  See icl_object.icl for an example.
  -->

<method name = "function" return = "rc" abstract = "1">
    <doc>
    Accepts a $(selftype) reference and returns zero in case of success,
    1 in case of errors.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
</method>

<!-- Animation support ------------------------------------------------------->

<public name = "types">
//  Global variables

extern Bool
    $(selfname)_animating;
</public>
<private name = "header">
Bool
    $(selfname)_animating = TRUE;       //  Animation enabled by default
</private>

<method name = "show animation">
    <doc>
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    </doc>
    <argument name = "enabled" type = "Bool">Are we enabling or disabling animation?</argument>
    $(selfname)_animating = enabled;
</method>

</class>
