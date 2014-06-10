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
    name      = "icl_shortstr"
    comment   = "iCL short string class"
    version   = "1.1"
    script    = "icl_gen"
    >
<doc>
    We define the icl_shortstr_t type in order to provide a simple way of
    storing and serialising short strings.  A "short string" is defined
    as one that can be serialised as a single-octet length indicator
    plus 0-255 octets of data.  The icl_shortstr_t type is held in memory
    as C-compatible null-terminated char arrays of 255+1 bytes.  There
    are no constructor/destructor methods for icl_shortstr_t variables since
    this would create incompatabilities with normal C strings.  See the
    selftest method for examples of using icl_shortstr_t.  When writing a
    icl_shortstr_t you should always use the icl_shortstr_cpy method to
    avoid the risk of overflows.
</doc>

<inherit class = "icl_base" />

<public name = "header">
#define ICL_SHORTSTR_MAX    255
typedef char icl_shortstr_t [ICL_SHORTSTR_MAX + 1];
</public>

<method name = "cpy" return = "count">
    <doc>
    Copies data into a specified short string.  Returns the number of chars
    actually copied.  If this is less than strlen (src), the string was
    truncated.  Allows a null source, in which case dest is cleared.
    </doc>
    <argument name = "dest"  type = "char *" >String to copy into</argument>
    <argument name = "src"   type = "char *" >String to copy from</argument>
    <declare  name = "count" type = "size_t" >Number of chars copied</declare>
    <local>
    register char
        *from = src;
    register size_t
        remaining;
    </local>
    if (src) {
        remaining = ICL_SHORTSTR_MAX;
        while (*from && remaining) {
            *dest++ = *from++;
            remaining--;
        }
        *dest = 0;
        count = from - src;
    }
    else {
        *dest = 0;
        count = 0;
    }
</method>

<method name = "cat" return = "count">
    <doc>
    Appends data onto a specified short string.  Returns the number of
    chars actually copied.  If this is less than strlen (src), the string
    was truncated.  Allows a null src in which case dest is unchanged.
    </doc>
    <argument name = "dest"  type = "char *" >String to copy into</argument>
    <argument name = "src"   type = "char *" >String to copy from</argument>
    <declare  name = "count" type = "size_t" default = "0" >Number of chars copied</declare>
    <local>
    register char
        *from = src;
    register size_t
        remaining;
    size_t
        dest_len = strlen (dest);
    </local>
    if (src) {
        dest += dest_len;
        remaining = ICL_SHORTSTR_MAX - dest_len;
        while (*from && remaining) {
            *dest++ = *from++;
            remaining--;
        }
        *dest = 0;
        count = from - src;
    }
</method>

<method name = "ncpy" return = "count">
    <doc>
    Copies length-specified data into a specified short string. The source
    string should not contain zero octets. Returns the number of chars
    actually copied.  If this is less than the specified length, the string
    was truncated.  Allows a null source, in which case dest is cleared.
    The resulting string is always null-terminated.
    </doc>
    <argument name = "dest"  type = "char *" >String to copy into</argument>
    <argument name = "src"   type = "char *" >String to copy from</argument>
    <argument name = "size"  type = "size_t" >Length of source string</argument>
    <declare  name = "count" type = "size_t" >Number of chars copied</declare>
    <local>
    register char
        *from = src;
    register size_t
        remaining;
    </local>
    if (src) {
        remaining = ICL_SHORTSTR_MAX;
        while (*from && remaining && size) {
            *dest++ = *from++;
            remaining--;
            size--;
        }
        *dest = 0;
        count = from - src;
    }
    else {
        *dest = 0;
        count = 0;
    }
</method>

<method name = "ncat" return = "count">
    <doc>
    Appends length-specified data onto a specified short string.  The source
    stringh should not contain zero octets.  Returns the number of chars
    actually copied.  If this is less than the specified length, the string
    was truncated.  Allows a null src in which case dest is unchanged.  The
    resulting string is always null-terminated.
    </doc>
    <argument name = "dest"  type = "char *" >String to copy into</argument>
    <argument name = "src"   type = "char *" >String to copy from</argument>
    <argument name = "size"  type = "size_t" >Length of source string</argument>
    <declare  name = "count" type = "size_t" default = "0" >Number of chars copied</declare>
    <local>
    register char
        *from = src;
    register size_t
        remaining;
    size_t
        dest_len = strlen (dest);
    </local>
    if (src) {
        dest += dest_len;
        remaining = ICL_SHORTSTR_MAX - dest_len;
        while (*from && remaining && size) {
            *dest++ = *from++;
            remaining--;
            size--;
        }
        *dest = 0;
        count = from - src;
    }
</method>

<method name = "fmt" return = "dest">
    <doc>
    Formats an icl_shortstr_t with the specified format and arguments.
    Returns the destination string.
    </doc>
    <argument name = "dest"   type = "char *" >String to format into</argument>
    <argument name = "format" type = "char *" >Format specifier</argument>
    <argument name = "args"   type = "..."    >Variable arguments</argument>
    apr_vsnprintf (dest, ICL_SHORTSTR_MAX, format, args);
</method>

<method name = "read" return = "count">
    <doc>
    Reads a short line of text from an open text file.  If the
    line is more than ICL_SHORTSTR_MAX characters long, it will
    be truncated. Safe to use on DOS or Unix-formatted files.
    </doc>
    <argument name = "string" type = "char *">Short string to fill</argument>
    <argument name = "file"   type = "FILE *">File to read from</argument>
    <declare name = "count" type = "size_t" default = "0" >Number of chars read</declare>
    <local>
    int
        nextch;                         //  Character read from file
    </local>
    FOREVER {
        nextch = fgetc (file);
        if (nextch == 13)             
            continue;                   //  Ignore DOS CR
        else
        if (nextch == 10                //  Have end of line
        ||  nextch == EOF               //    or end of file
        ||  nextch == 26) {             //    or DOS Ctrl-Z
            string [count] = 0;         //  Terminate string
            break;
        }
        else {
            string [count++] = (char) nextch;
            if (count == ICL_SHORTSTR_MAX) {
                string [count] = 0;
                break;                  //  Oversized string
            }
        }
    }
</method>


<!-- Testing ----------------------------------------------------------------->

<method name = "selftest" export = "none">
    <local>
    icl_shortstr_t
        sstr;                           //  Short string for tutorial
    size_t
        length;                         //  Number of characters in string
    icl_shortstr_t
        string1,                        //  Test string
        string2;                        //  Test string
    </local>

    //  The cpy method sets the string and returns its new size
    length = icl_shortstr_cpy (sstr, "hello world\\n");

    //  The cat method returns the number of characters appended
    length += icl_shortstr_cat (sstr, "goodbye world!\\n");

    assert (streq  (sstr, "hello world\\ngoodbye world!\\n"));
    assert (strlen (sstr) == length);

    //  The ncpy and ncat methods can be used to copy just part of a string
    length  = icl_shortstr_ncpy (sstr, "Help!", 3);
    length += icl_shortstr_ncat (sstr, "local", 2);

    assert (streq  (sstr, "Hello"));
    assert (strlen (sstr) == length);

    //- Regression testing starts here ---------------------------------------

    //  Check that the cat method does not overflow
    icl_shortstr_cpy (string1, "...............X...............X...............X...............A");
    icl_shortstr_cat (string1, "...............X...............X...............X...............B");
    icl_shortstr_cat (string1, "...............X...............X...............X...............C");
    icl_shortstr_cat (string1, "...............X...............X...............X...............D");
    icl_shortstr_cat (string1, "...............X...............X...............X...............E");
    assert (strlen (string1) == ICL_SHORTSTR_MAX);

    //  Copy the truncated shortstr to another shortstr
    icl_shortstr_cpy (string2, string1);
    icl_shortstr_cat (string2, "*****************");
    assert (strlen (string2) == ICL_SHORTSTR_MAX);
</method>

</class>
