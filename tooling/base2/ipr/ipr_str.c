/*---------------------------------------------------------------------------
    ipr_str.c - ipr_str component

    Provides various string manipulation functions.
    Generated from ipr_str.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "ipr_str.h"                    //  Definitions for our class

//  Shorthands for class methods

#define self_skip           ipr_str_skip
#define self_wipe           ipr_str_wipe
#define self_lower          ipr_str_lower
#define self_upper          ipr_str_upper
#define self_subch          ipr_str_subch
#define self_crop           ipr_str_crop
#define self_block          ipr_str_block
#define self_format         ipr_str_format
#define self_filter         ipr_str_filter
#define self_match          ipr_str_match
#define self_prefixed       ipr_str_prefixed
#define self_defix          ipr_str_defix
#define self_clean          ipr_str_clean
#define self_lexeq          ipr_str_lexeq
#define self_lexcmp         ipr_str_lexcmp
#define self_soundex        ipr_str_soundex
#define self_matches        ipr_str_matches
#define self_lex_matches    ipr_str_lex_matches
#define self_find           ipr_str_find
#define self_random         ipr_str_random
#define self_numeric        ipr_str_numeric
#define self_md5            ipr_str_md5
#define self_selftest       ipr_str_selftest
#define self_show_animation  ipr_str_show_animation

Bool
    ipr_str_animating = TRUE;           //  Animation enabled by default
/*  -------------------------------------------------------------------------
    ipr_str_skip

    Type: Component method
    Skips leading spaces in string, and returns a pointer to the
    first non-blank character.  If this is a null, the end of the string
    was reached. Does not modify string.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_skip (
    char * string                       //  Input string
)
{

//
while (isspace (*string))
    string++;

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_wipe

    Type: Component method
    Sets all characters in string up to but not including the
    final null character to ch.  Returns string.  Modifies string.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_wipe (
    char * string,                      //  Input string
    char ch                             //  Character to use
)
{
char *scan;

//
assert (string);
scan = string;
while (*scan)
    *scan++ = ch;

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_lower

    Type: Component method
    Converts all alphabetic characters in string to lowercase,
    stopping at the final null character.  Returns string, which
    may be null.  Modifies string.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_lower (
    char * string                       //  Input string
)
{
char *scan;

//
if (string) {
    scan = string;
    while (*scan) {
        *scan = (char) tolower (*scan);
        scan++;
    }
}

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_upper

    Type: Component method
    Converts all alphabetic characters in string to uppercase,
    stopping at the final null character.  Returns string, which
    may be null.  Modifies string.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_upper (
    char * string                       //  Input string
)
{
char *scan;

//
if (string) {
    scan = string;
    while (*scan) {
        *scan = (char) toupper (*scan);
        scan++;
    }
}

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_subch

    Type: Component method
    Substitutes instances of one character in a string to some other
    character. Returns string. Does nothing if the string is NULL.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_subch (
    char * string,                      //  Input string
    char from,                          //  Character to look for
    char to                             //  Character to replace with
)
{
char
    *scan;

if (string) {
    scan = string;
    while (*scan) {
        if (*scan == from)
           *scan = to;
        scan++;
    }
}

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_crop

    Type: Component method
    Drops trailing whitespace from string by truncating string
    to the last non-whitespace character.  Returns string, which may
    be null.  Modifies string.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_crop (
    char * string                       //  Input string
)
{
char *last;

//
if (string) {
    last = string + strlen (string);
    while (last > string) {
        if (!isspace (*(last - 1)))
            break;
        last--;
    }
    *last = 0;
}

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_block

    Type: Component method
    Formats a string of repeated characters of the specified size.  The string
    must be large enough to hold the formatted block.  Returns the string.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_block (
    char * string,                      //  Input string
    char content,                       //  Character to repeat
    size_t count                        //  Number of repetitions
)
{

//
assert (string);
string [count] = 0;
while (count)
    string [--count] = content;

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_format

    Type: Component method
    Formats a new string from the supplied printf format and arguments.  The
    caller should free the string using icl_mem_free() when done.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_format (
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    char *
        _rc;
    va_list
        args;
    va_start (args, format);
    _rc = ipr_str_format_v (format, args );
    va_end (args);
    return _rc;
}
/*  -------------------------------------------------------------------------
    ipr_str_format_v

    Type: Component method
    Formats a new string from the supplied printf format and arguments.  The
    caller should free the string using icl_mem_free() when done.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_format_v (
    char * format,                      //  Format specifier
va_list args                            //  Variable arguments
)
{
int
    str_size = 255,                 //  Arbitrary short length
    fmt_size;
    char *
        string;                         //  Not documented

//
//  Try a short allocation, reallocate if that's too small
string = icl_mem_alloc (str_size + 1);
fmt_size = apr_vsnprintf (string, str_size, format, args);
if (fmt_size >= str_size) {
    //  If formatted value overflowed string, resize and repeat
    icl_mem_free (string);
    str_size = fmt_size;
    string = icl_mem_alloc (str_size + 1);
    apr_vsnprintf (string, str_size, format, args);
}

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_filter

    Type: Component method
    Converts any unprintable characters in string into dots. Modifies
    string.  Newlines and tabs are considered printable.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_filter (
    char * string                       //  Input string
)
{
char *scan;

//
if (string) {
    scan = string;
    while (*scan) {
        if (!isprint (*scan) && *scan != '\n' && *scan != '\t')
            *scan = '.';
        scan++;
    }
}

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_match

    Type: Component method
    Calculates a similarity index for the two strings.  This is a value from
    0 to 32767 with higher values indicating a closer match. The two strings
    are compared without regard for case.  The algorithm was designed by Leif
    Svalgaard (leif@ibm.net).
    -------------------------------------------------------------------------
 */

int
    ipr_str_match (
    char * string1,                     //  First string
    char * string2                      //  Second string
)
{
static int
    name_weight [30] = {
        20, 15, 13, 11, 10, 9, 8, 8, 7, 7, 7, 6, 6, 6, 6,
         6,  5,  5,  5,  5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4
    };
int
    comp_index,
    name_index,
    start_of_string,
    longest_so_far,
    substring_contribution,
    substring_length,
    compare_length,
    longest_length,
    length_difference,
    name_length,
    char_index,
    similarity_weight;
char
    cur_name_char;
    int
        similarity_index;               //  Not documented

//
assert (string1);
assert (string2);

name_length    = strlen (string1);
compare_length = strlen (string2);
if (name_length > compare_length) {
    length_difference = name_length - compare_length;
    longest_length    = name_length;
}
else {
    length_difference = compare_length - name_length;
    longest_length    = compare_length;
}
if (compare_length) {
    similarity_weight = 0;
    substring_contribution = 0;

    for (char_index = 0; char_index < name_length; char_index++) {
        start_of_string = char_index;
        cur_name_char   = (char) tolower (string1 [char_index]);
        longest_so_far  = 0;
        comp_index      = 0;

        while (comp_index < compare_length) {
            while (comp_index < compare_length
            &&     tolower (string2 [comp_index]) != cur_name_char)
                comp_index++;

            substring_length = 0;
            name_index = start_of_string;

            while (comp_index < compare_length
            && tolower (string2 [comp_index]) == tolower (string1 [name_index])) {
                if (comp_index == name_index)
                    substring_contribution++;
                comp_index++;
                if (name_index < name_length) {
                    name_index++;
                    substring_length++;
                }
            }
            substring_contribution += (substring_length + 1) * 3;
            if (longest_so_far < substring_length)
                longest_so_far = substring_length;
        }
        similarity_weight += (substring_contribution + longest_so_far + 1) * 2;
        similarity_weight /= name_length + 1;
    }
    similarity_index  = (name_length < 30? name_weight [name_length]: 3)
                      * longest_length;
    similarity_index /= 10;
    similarity_index += 2 * length_difference / longest_length;
    similarity_index  = 100 * similarity_weight / similarity_index;
}
else
    similarity_index = 0;

    return (similarity_index);
}
/*  -------------------------------------------------------------------------
    ipr_str_prefixed

    Type: Component method
    If string starts with specified prefix, returns TRUE.  If
    string does not start with specified prefix, returns FALSE.
    -------------------------------------------------------------------------
 */

Bool
    ipr_str_prefixed (
    char * string,                      //  Input string
    char * prefix                       //  Prefix string
)
{
    Bool
        rc;                             //  Not documented

//
assert (string);
assert (prefix);

if (*string == *prefix              //  Check that first letters match
&&  strlen (string) >= strlen (prefix)
&&  memcmp (string, prefix, strlen (prefix)) == 0)
    rc = TRUE;
else
    rc = FALSE;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_str_defix

    Type: Component method
    If string starts with specified prefix, returns pointer to character
    after prefix. Null character is not considered part of the prefix. If
    string does not start with specified prefix, returns NULL.  Does not
    modify string.  String can be null.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_defix (
    char * string,                      //  Input string
    char * prefix                       //  Prefix to remove
)
{
    char *
        result = NULL;                  //  Not documented

//
assert (prefix);
if (string
&&  strlen (string) >= strlen (prefix)
&&  memcmp (string, prefix, strlen (prefix)) == 0)
    result = (char *) string + strlen (prefix);

    return (result);
}
/*  -------------------------------------------------------------------------
    ipr_str_clean

    Type: Component method
    Swaps all instances of one character in a string to some other character.
    Returns string.  Does nothing if the string is NULL. Modifies string.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_clean (
    char * string,                      //  Input string
    char from,                          //  Char to look for
    char to                             //  Char to replace with
)
{
char *scan;

if (string) {
    scan = string;
    while (*scan) {
        if (*scan == from)
           *scan = to;
        scan++;
    }
}

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_lexeq

    Type: Component method
    Returns TRUE if two string match without regard to case, else returns
    FALSE.
    -------------------------------------------------------------------------
 */

Bool
    ipr_str_lexeq (
    char * string1,                     //  Input string 1
    char * string2                      //  Input string 2
)
{
    Bool
        rc;                             //  Not documented

//
assert (string1);
assert (string2);

do {
    rc = (tolower (*string1) == tolower (*string2));
}
while (*string1++ && *string2++ && rc);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_str_lexcmp

    Type: Component method
    Performs a case-insensitive string comparison; returns -1, 0, or 1
    with in same fashion as strcmp.
    -------------------------------------------------------------------------
 */

int
    ipr_str_lexcmp (
    char * string1,                     //  Input string 1
    char * string2                      //  Input string 2
)
{
    int
        rc;                             //  Not documented

//
assert (string1);
assert (string2);

do {
    rc = (byte) tolower (*string1) - (byte) tolower (*string2);
}
while (*string1++ && *string2++ && rc == 0);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_str_soundex

    Type: Component method
    Calculates the SOUNDEX code for the string.  SOUNDEX converts letters to
    uppercase, and translates each letter according to this table: A0 B1 C2
    D3 E0 F1 G2 H0 I0 J2 K2 L4 M5 N5 O0 P1 Q2 R6 S2 T3 U0 V1 W0 X2 Y0 Z2.
    Non-letters are ignored, letters that translate to zero, and multiple
    occurences of the same value are also ignored.  Before calling this
    function you may initialise the encoding to a string of zeroes, to
    indicate the length of the encoding you want. The default encoding is
    4 digits long ("0000").
    -------------------------------------------------------------------------
 */

char *
    ipr_str_soundex (
    char * string,                      //  Input string
    char * encoding                     //  Resulting soundex
)
{
#   define SOUNDEX_TABLE                     "00000000000000000000000000000000"   "00000000000000000000000000000000"   "00123012002245501262301020200000"   "00123012002245501262301020200000"   "00000000000000000000000000000000"   "00000000000000000000000000000000"   "00000000000000000000000000000000"   "00000000000000000000000000000000"
    static char
       *soundex_table = SOUNDEX_TABLE;
    int
        index;
    char
        last_value = 0,
        this_value;

//
assert (string);

if (*encoding != '0')
    strcpy (encoding, "0000");
*encoding = toupper (*string);
last_value = soundex_table [(byte) *string];
string++;
index = 1;                          //  Store results at [index]

while (*string && isalpha (*string)) {
    //  Letters H and W don't act as separator between letters having
    //  same code value
    this_value = soundex_table [(byte) *string];
    while ((*string == 'H' || *string == 'h'
    ||      *string == 'W' || *string == 'w')
    && *(string + 1))
         this_value = soundex_table [(byte) *++string];

    if (this_value == last_value    //  Ignore doubles
    ||  this_value == '0') {        //    and 'quiet' letters
        string++;
        last_value = this_value;
        continue;
    }
    string++;
    last_value = this_value;
    encoding [index++] = this_value;
    if (encoding [index] != '0')
        break;
}

    return (encoding);
}
/*  -------------------------------------------------------------------------
    ipr_str_matches

    Type: Component method
    Returns TRUE if the string matches the pattern.  The pattern syntax is
    based on UNIX shell glob expressions as follows:

    * - Matches zero or more instances of any character
    ? - Matches one instance of any character

    All other characters match themselves.  Escaping * or ? and character
    classes is not supported.
    -------------------------------------------------------------------------
 */

Bool
    ipr_str_matches (
    char * string,                      //  Input string
    char * pattern                      //  Input pattern
)
{
char
    *regexp,                        //  'pattern' converted to a regexp
    *regexp_start;                  //  Saved pointer to start of regexp
    Bool
        rc;                             //  Not documented

//
assert (string);
assert (pattern);
//  Resulting regexp will never be longer than 2*(pattern), but we need
//  3 bytes extra space for the anchors and string terminator
regexp = icl_mem_alloc ((strlen (pattern) * 2) + 3);
regexp_start = regexp;
*regexp = '^';                      //  Anchor expression at start of string
regexp++;
while (*pattern) {
    if (*pattern == '*') {          //  Convert '*' to '.*'
        *regexp++ = '.';
        *regexp   = '*';
    }
    else
    if (*pattern == '?') {          //  Convert '?' to '.'
        *regexp   = '.';
    }
    else
    if (isalnum (*pattern)) {       //  Pass alphanumeric characters through
        *regexp = *pattern;
    }
    else {                          //  Escape anything else with '`'
        *regexp++ = '`';
        *regexp   = *pattern;
    }
    pattern++;
    regexp++;
}
*regexp = '$';                      //  Anchor expression at end of string
regexp++;
*regexp = 0;
regexp = regexp_start;
rc = ipr_regexp_eq (regexp, string);
icl_mem_free (regexp);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_str_lex_matches

    Type: Component method
    Does the same as the match method but without regard to case in
    either string or pattern.
    -------------------------------------------------------------------------
 */

Bool
    ipr_str_lex_matches (
    char * string,                      //  Input string
    char * pattern                      //  Input pattern
)
{
char
    *string_lc,
    *pattern_lc;
    Bool
        rc;                             //  Not documented

//
assert (string);
assert (pattern);
string_lc = icl_mem_strdup (string);
pattern_lc = icl_mem_strdup (pattern);
rc = ipr_str_matches (ipr_str_lower (string_lc), ipr_str_lower (pattern_lc));
icl_mem_free (string_lc);
icl_mem_free (pattern_lc);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_str_find

    Type: Component method
    Searches for the needle in the haystack, ignoring case.  Returns a pointer
    to the beginning of the substring, if found, else null.  If you need a
    case sensitive search, use the standard strstr() library function.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_find (
    char * haystack,                    //  String to search in
    char * needle                       //  String to search for
)
{
char
    *haystack_lc,
    *needle_lc;
    char *
        found;                          //  Not documented

//
assert (haystack);
assert (needle);
haystack_lc = icl_mem_strdup (haystack);
needle_lc = icl_mem_strdup (needle);
found = strstr (ipr_str_lower (haystack_lc), ipr_str_lower (needle_lc));
icl_mem_free (haystack_lc);
icl_mem_free (needle_lc);

    return (found);
}
/*  -------------------------------------------------------------------------
    ipr_str_random

    Type: Component method
    Randomizes a string according to a pattern.  The pattern can contain
    these characters:
    9 - generates a random digit [0-9]
    X - generates a random letter [A-Z]
    A - generates a random alphanumeric [A-Z0-9]
    x - following digits are repeat counter
    other - used unchanged
    String must be long enough for result.  Returns string.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_random (
    char * string,                      //  String to fill
    char * pattern                      //  Pattern
)
{
static Bool
    randomized = FALSE;
static char
    *char36 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char
    *target = string;
int
    repeats;                        //  Repeat counter value

assert (pattern);
assert (string);
if (!randomized) {
    randomize ();
    randomized = TRUE;
}
while (*pattern) {
    //  Grab repeat counter if any
    if (pattern [1] == 'x' && isdigit (pattern [2])) {
        repeats = atoi (pattern + 2);
    }
    else
        repeats = 1;

    if (*pattern == '9') {
        while (repeats--)
            *target++ = '0' + randomof (10);
    }
    else
    if (*pattern == 'X') {
        while (repeats--)
            *target++ = 'A' + randomof (26);
    }
    else
    if (*pattern == 'A') {
        while (repeats--)
            *target++ = char36 [randomof (36)];
    }
    else {
        while (repeats--)
            *target++ = *pattern;
    }
    //  Skip repeat counter if any
    if (pattern [1] == 'x' && isdigit (pattern [2])) {
        pattern += 2;
        while (isdigit (*pattern))
            pattern++;
    }
    else
        pattern++;
}
*target = 0;

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_numeric

    Type: Component method
    Returns TRUE if the string is purely numeric, else returns FALSE.
    -------------------------------------------------------------------------
 */

Bool
    ipr_str_numeric (
    char * string                       //  Input string 1
)
{
    Bool
        rc = TRUE;                      //  Not documented

//
assert (string);
while (*string && rc)
    rc = isdigit (*string++);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_str_md5

    Type: Component method
    Calculates the MD5 hash of a string and returns the ASCII hex encoding
    of the hash.  Allocates a new string which the caller should free. The
    string can be a printf specifier with arguments.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_md5 (
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    char *
        _rc;
    va_list
        args;
    va_start (args, format);
    _rc = ipr_str_md5_v (format, args );
    va_end (args);
    return _rc;
}
/*  -------------------------------------------------------------------------
    ipr_str_md5_v

    Type: Component method
    Calculates the MD5 hash of a string and returns the ASCII hex encoding
    of the hash.  Allocates a new string which the caller should free. The
    string can be a printf specifier with arguments.
    -------------------------------------------------------------------------
 */

char *
    ipr_str_md5_v (
    char * format,                      //  Format specifier
va_list args                            //  Variable arguments
)
{
byte
    digest [APR_MD5_DIGESTSIZE];
int
    str_size = 1024,
    fmt_size;
uint
    char_nbr;
char
    hex [] = "0123456789abcdef";
    char *
        string;                         //  Not documented

//
//  Format a string with the value
string = icl_mem_alloc (str_size + 1);
fmt_size = apr_vsnprintf (string, str_size, format, args);
if (fmt_size >= str_size) {
    //  If formatted value overflowed string, resize and repeat
    icl_mem_free (string);
    str_size = fmt_size;
    string = icl_mem_alloc (str_size + 1);
    fmt_size = apr_vsnprintf (string, str_size, format, args);
}
apr_md5 (digest, string, fmt_size);
icl_mem_free (string);

//  Format digest into printable hex value
string = icl_mem_alloc (APR_MD5_DIGESTSIZE * 2 + 1);
for (char_nbr = 0; char_nbr < APR_MD5_DIGESTSIZE; char_nbr++) {
    string [char_nbr * 2]     = hex [digest [char_nbr] >> 4];
    string [char_nbr * 2 + 1] = hex [digest [char_nbr] & 15];
}
string [APR_MD5_DIGESTSIZE * 2] = 0;

    return (string);
}
/*  -------------------------------------------------------------------------
    ipr_str_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_str_selftest (
void)
{
icl_shortstr_t
    string;
char
    *result;
int
    match1,
    match2;
Bool
    rc;

//  Skip leading spaces in a string
strcpy (string, "   abc");
result = ipr_str_skip (string);
assert (streq (result, "abc"));

//  Wipe a string with a single character
strcpy (string, "abc");
ipr_str_wipe (string, '?');
assert (streq (string, "???"));

//  Lower-case a string
strcpy (string, "AbcDef");
ipr_str_lower (string);
assert (streq (string, "abcdef"));

//  Upper-case a string
strcpy (string, "AbcDef");
ipr_str_upper (string);
assert (streq (string, "ABCDEF"));

//  Crop trailing spaces off a string
strcpy (string, "abc   ");
ipr_str_crop (string);
assert (streq (string, "abc"));

//  Test block method
assert (streq (ipr_str_block (string, 'X', 10), "XXXXXXXXXX"));
assert (streq (ipr_str_block (string, 'X', 0), ""));

//  Test format method
result = ipr_str_format ("");
assert (strnull (result));
icl_mem_free (result);

result = ipr_str_format ("%s %s %s", "this", "is", "a string");
assert (streq (result, "this is a string"));
icl_mem_free (result);

//  Calculate similarity match for two strings
match1 = ipr_str_match ("incredible", "hulk");
match2 = ipr_str_match ("incredible", "crucible");
assert (match1 < match2);

//  Check if a string starts with something specific
rc = ipr_str_prefixed ("incredible", "crucible");
assert (!rc);
rc = ipr_str_prefixed ("incredible", "in");
assert (rc);

//  Remove a specified prefix from a string
strcpy (string, "incredible");
result = ipr_str_defix (string, "in");
assert (streq (result, "credible"));

//  Convert one character to another through the string
strcpy (string, "incredible");
ipr_str_clean (string, 'e', 'y');
assert (streq (string, "incrydibly"));

//  Case-insensitive compare two strings
rc = ipr_str_lexeq ("incredible", "InEdible");
assert (!rc);
rc = ipr_str_lexeq ("incredible", "InCredible");
assert (rc);

rc = ipr_str_lexcmp ("incredible", "InEdible");
assert (rc < 0);
rc = ipr_str_lexcmp ("incredible", "InCredible");
assert (rc == 0);

//  Calculate Soundex encoding of a string
ipr_str_soundex ("Incredible", string);
assert (streq (string, "I526"));
ipr_str_soundex ("Inkredible", string);
assert (streq (string, "I526"));
ipr_str_soundex ("Imcrododo", string);
assert (streq (string, "I526"));

//  Test some basic pattern matching
assert (ipr_str_matches ("test_string", "*str*"));
assert (ipr_str_matches ("#4", "#*"));
assert (ipr_str_matches ("name.ext", "*.ext"));
assert (ipr_str_matches ("longword123", "longword???"));
assert (!ipr_str_matches ("longword12", "longword???"));

assert (ipr_str_lex_matches ("test_string", "*STR*"));
assert (ipr_str_lex_matches ("Name.Ext", "*.ext"));
assert (ipr_str_lex_matches ("longWORD123", "LONGword???"));
assert (!ipr_str_lex_matches ("longWORD12", "LONGword???"));

assert (ipr_str_find ("longWORD123", "NGword"));
assert (!ipr_str_find ("longWORD12", "NGwod"));

//  Test string randomization
assert (strlen (ipr_str_random (string, "9")) == 1);
assert (strlen (ipr_str_random (string, "9x10")) == 10);
assert (strlen (ipr_str_random (string, "x10")) == 3);
assert (strlen (ipr_str_random (string, "9x")) == 2);
assert (strlen (ipr_str_random (string, "9x0")) == 0);
assert (strlen (ipr_str_random (string, "AAAA-AAAA-AAAA-AAAA-AAAA-AAAA")) == 29);
assert (strlen (ipr_str_random (string, "AAAA-AAAA-AAAA-AAAA-AAAA-AAAA")) == 29);
assert (strlen (ipr_str_random (string, "XXXX-XXXX-XXXX-9999-9999-9999")) == 29);

assert (ipr_str_numeric ("123456"));
assert (!ipr_str_numeric ("123 456"));

result = ipr_str_md5 ("%s:%s:%s", "Mufasa", "testrealm@host.com", "Circle Of Life");
assert (result);
assert (streq (result, "939e7578ed9e3c518a452acee763bce9"));
icl_mem_free (result);
}
/*  -------------------------------------------------------------------------
    ipr_str_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_str_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_str_animating = enabled;
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_str_version_start        = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_str_component            = "ipr_str ";
char *EMBED__ipr_str_version              = "1.0 ";
char *EMBED__ipr_str_copyright            = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_str_filename             = "ipr_str.icl ";
char *EMBED__ipr_str_builddate            = "2011/03/01 ";
char *EMBED__ipr_str_version_end          = "VeRsIoNeNd:ipc";

