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
    name      = "ipr_regexp"
    comment   = "iPR regular expression component"
    version   = "1.0"
    script    = "icl_gen"
    opaque    = "1"
    >

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "rwlock" value = "1"/>
</inherit>
<todo owner = "ph@imatix.com">
    Define an alternate API for retrieving captured substrings.
    The current API does not allow arbitrary iteration through
    these substrings and fails on win32 if insufficient substrings
    are passed on the va_args part of the match method.
</todo>
<doc>
    Provides an interface to the PCRE library.  This is a snippet
    from the PCRE documentation:

    There are two different sets of meta-characters: those  that
    are  recognized anywhere in the pattern except within square
    brackets, and those that are recognized in square  brackets.
    Outside square brackets, the meta-characters are as follows:

    \      general escape character with several uses
    ^      assert start of subject (or line, in multiline mode)
    $      assert end of subject (or line, in multiline mode)
    .      match any character except newline (by default)
    [      start character class definition
    |      start of alternative branch
    (      start subpattern
    )      end subpattern
    ?      extends the meaning of (
           also 0 or 1 quantifier
           also quantifier minimizer
    *      0 or more quantifier
    +      1 or more quantifier
    {      start min/max quantifier

    Part of a pattern that is in square brackets is called a
    "character class".  In a character class the only meta-
    characters are:

    \      general escape character
    ^      negate the class, but only if the first character
    -      indicates character range
    ]      terminates the character class
</doc>

<context>
    pcre
        *regexp;                        //  Parsed regular expression
    char
        *pattern;                       //  Copy of regexp
    int
        match_count;                    //  Number of pattern matches
    int
        *match_list;                    //  Matches
    int
        match_size;                     //  Size of match list
</context>

<public name = "header">
#include "pcre.h"
</public>

<method name = "new">
    <doc>
    Create new regular expression.  Note that we allow '`' to be used
    in place of '\\' which causes difficulties for C and code generation.
    If you really want to use ` in the expression, double the character.
    If pattern is null, creates a new regexp object without expression.
    Before using the regexp you must compile it using the compile method.
    </doc>
    <argument name = "pattern" type = "char *" />
    //
    if (pattern)
        self_compile (self, pattern, TRUE);
</method>

<method name = "destroy">
    icl_mem_free (self->pattern);
    icl_mem_free (self->match_list);
    if (self->regexp)
        pcre_free (self->regexp);
</method>

<method name = "compile" template = "function">
    <doc>
    Create new regular expression.  If csafe is set, allows '`' to be used
    in place of '\\' which causes difficulties for C and code generation.
    If you really want to use ` in the expression, double the character.
    Do not call compile twice on one regexp object.  Returns -1 if the
    regexp was invalid.
    </doc>
    <argument name = "pattern" type = "char *" />
    <argument name = "csafe"   type = "Bool" />
    <local>
    int
        pcre_offset;
    char
        *scan,
        *pcre_error;                    //  Last error if any
    </local>
    //
    //  Copy the pattern so we can play with it
    assert (!self->pattern);
    self->pattern = icl_mem_strdup (pattern);
    if (csafe) {
        scan = self->pattern;
        while (*scan) {
            if (*scan == '`') {
                *scan = '\\\\';
                if (*(scan + 1) == '`')
                    scan++;                 //  Allow `` to mean `
            }
            scan++;
        }
    }
    self->regexp = pcre_compile (self->pattern, 0, (const char **) &pcre_error, &pcre_offset, NULL);
    if (self->regexp) {
        pcre_fullinfo (self->regexp, NULL, PCRE_INFO_CAPTURECOUNT, &self->match_count);
        self->match_size = (self->match_count + 1) * 3;
        self->match_list = (int *) icl_mem_alloc (self->match_size * sizeof (int));
    }
    else
        rc = -1;
</method>

<method name = "match" template = "function">
    <doc>
    Parses a string using a regular expression and loads zero or more
    patterns into supplied strings. Returns the number of matched
    strings, or 1 if no strings were supplied and the expression
    matched. Returns zero if the expression did not match at all.
    </doc>
    <argument name = "string" type = "char *" />
    <argument name = "argptr" type = "..."    />
    <local>
    char
        **symbol;                       //  Passed to the function
    int
        match_okay,                     //  Did string match pattern?
        match_index,                    //  Work through each match...
        start,
        length;
    </local>

    assert (string);
    match_okay = pcre_exec (
        self->regexp, NULL, string, strlen (string), 0, 0,
        self->match_list, self->match_size);

    for (match_index = 0; match_index < self->match_count; match_index++) {
        symbol = va_arg (argptr, char **);
        if (symbol) {
            if (match_okay >= 0) {
                icl_mem_strfree (symbol);
                start  = self->match_list [match_index * 2 + 2];
                length = self->match_list [match_index * 2 + 3] - start;
                *symbol = (char *) icl_mem_alloc (length + 1);
                if (length > 0)
                    memcpy (*symbol, string + start, length);
                (*symbol) [length] = 0;
            }
            else {
                //  No match, so return empty string
                if (*symbol) {
                    icl_mem_strfree (symbol);
                    *symbol = icl_mem_strdup ("");
                }
            }
        }
    }
    if (match_okay >= 0) {
        if (self->match_count)
            rc = self->match_count;     //  Matched N items
        else
            rc = 1;                     //  Match succeeded
    }
    else
        rc = 0;                         //  Match failed
</method>

<method name = "eq" return = "rc">
    <doc>
    Matches a string with a regular expression and returns TRUE
    if the string matches, FALSE if not.  Does not require any
    set-up, but is slower than the match method when comparing
    many strings, since a $(selfname) object is created and
    destroyed for each call.
    </doc>
    <argument name = "regexp" type = "char *">Regular expression</argument>
    <argument name = "string" type = "char *">String to compare</argument>
    <declare name = "rc" type = "int">Return value</declare>
    <local>
    $(selftype)
        *self;                          //  The compiled regexp
    </local>
    self = self_new (regexp);
    if (ipr_regexp_match (self, string))
        rc = TRUE;
    else
        rc = FALSE;

    self_destroy (&self);
</method>

<method name = "selftest" export = "none">
    char
        *regexp_str;                    //  The regexp string
    ipr_regexp_t
        *regexp;                        //  The compiled regexp
    char
        *item = NULL;                   //  A matched item
    int
        nbr_items;                      //  Number of matched items

    //  This is an example of a Perl5-compatible regular expression
    //  Note that ipr_regexp lets us use ` instead of a back slash,
    //  since back-slashes require a lot of escaping in iCL code.
    //  The ( ) groups indicate matched items.  {} is used to define
    //  min/max occurrences.
    //
    regexp_str = "^[0-9]{3}`-[0-9]{3}`-[0-9]{3,6}$";

    //  The simplest way to get a yes/no match is to use ipr_regexp_eq
    assert (ipr_regexp_eq  (regexp_str, "123-456-789"));
    assert (ipr_regexp_eq  (regexp_str, "123-456-789123"));
    assert (!ipr_regexp_eq (regexp_str, "123-456-7891239"));
    assert (!ipr_regexp_eq (regexp_str, "123-456-abc"));

    //  Here is an expression with three subpatterns:
    regexp_str = "([0-9]+)`-([0-9]+)`-([0-9]+)";

    //  We compile our regular expression into an ipr_regexp object
    //  If we did a lot of matching on a single expression, it would
    //  be worth keeping this object around.
    //
    regexp = ipr_regexp_new (regexp_str);

    //  Now we do the matching
    //  Note that item must be initialised to NULL, otherwise the
    //  match method will free it first (to allow reuse).
    //
    nbr_items = ipr_regexp_match (regexp, "123-456-789", NULL, &item, NULL);

    //  We must have matched all three items and extracted the second one
    //
    assert (nbr_items == 3);
    assert (streq (item, "456"));

    //  Clean up - destroy all objects and free allocated memory
    //
    ipr_regexp_destroy (&regexp);
    icl_mem_free (item);

    //  Let's try some more complex expressions
    //
    regexp_str = "^`w+`.eur$";
    regexp = ipr_regexp_new (regexp_str);
    assert (ipr_regexp_match (regexp, "stocks.eur") == 1);
    assert (ipr_regexp_match (regexp, "stocks.usd") == 0);
    ipr_regexp_destroy (&regexp);

    //  To define subsequences that are not captured, we use (?: ... )
    //
    regexp_str = "^`w+(?:[`./]`w+)*[`./]eur$";
    regexp = ipr_regexp_new (regexp_str);
    assert (ipr_regexp_match (regexp, "stocks/aapl/eur") == 1);
    assert (ipr_regexp_match (regexp, "stocks.msft/eur") == 1);
    assert (ipr_regexp_match (regexp, "stocks/csco.usd") == 0);
    ipr_regexp_destroy (&regexp);

    regexp = ipr_regexp_new ("^^[a-zA-Z0-9-_.:]*$$");
    for (nbr_items = 0; nbr_items < 10000; nbr_items++)
        ipr_regexp_match (regexp, "This-is-a-queue");
    ipr_regexp_destroy (&regexp);

    //  Heap stress test
    for (nbr_items = 0; nbr_items < 25000; nbr_items++) {
        regexp = ipr_regexp_new ("^^[a-zA-Z0-9-_.:]*$$");
        ipr_regexp_destroy (&regexp);
    }

    regexp = ipr_regexp_new (NULL);
    ipr_regexp_compile (regexp, "`w", FALSE);
    assert (ipr_regexp_match (regexp, "`w"));
    ipr_regexp_destroy (&regexp);
</method>

</class>
