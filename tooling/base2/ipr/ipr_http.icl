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
    name      = "ipr_http"
    comment   = "HTTP module"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    The HTTP module provides utility functions for HTTP encodings and
    other basic functionality.
</doc>
<inherit class = "icl_base" />
<import class = "ipr_regexp" />

<method name = "encode meta" return = "rc">
    <argument name = "output" type = "char *" />
    <argument name = "input"  type = "char **" />
    <argument name = "outmax" type = "size_t" />
    <argument name = "html"   type = "Bool" />
    <declare name = "rc" type = "size_t" />
    <doc>
    Translates special characters into HTML/SGML metacharacters.
    The input buffer is not modified; you supply an output buffer and specify
    the maximum size of this buffer.  The input buffer must end in a null.
    After calling, the input pointer is set to the character after the last
    encoded character.  Returns the final size of the translated data
    excluding the final null byte.  If the resulting output data would be
    too long, translations stops.  If html is TRUE then the metacharacters
    amp, lt, gt and quot are not translated - this allows you to encode
    markup characters within HTML; otherwise they are translated and the
    output doesn't look like HTML.
    </doc>
    <local>
    size_t
        space_left,                     //  Space left in destination
        length;
    char
        *dest;                          //  Pointer to result string
    </local>
    //
    assert (input);
    assert (*input);
    assert (output);

    if (outmax == 0)                    //  Special case for zero space
        rc = 0;
    else {
        space_left = outmax - 1;            //  Allow for final null byte
        dest = output;
        while (**input && space_left > 0) {
            length = s_encode_meta_char (dest, **input, space_left, html);
            if (length) {
                space_left -= length;
                dest += length;
                (*input) ++;
            }
            else
                break;
        }
        *dest = 0;
        rc = (size_t) (dest - output);
    }
</method>

<method name = "decode meta" return = "rc">
    <argument name = "output" type = "char *" />
    <argument name = "input"  type = "char **" />
    <argument name = "outmax" type = "size_t" />
    <declare name = "rc" type = "size_t" />
    <doc>
    Translates special characters from HTML/SGML metacharacters.  The
    input buffer is not modified; you supply an
    output buffer and specify the maximum size of this buffer.  The input
    buffer must end in a null.  The two buffers may be the same, since
    decoding reduces the size of the data.  Returns the final size of the
    translated data excluding the final null byte.  If the resulting data
    is too long, it is brutally truncated.  Invalid meta-characters are
    left as-is.  Normally the input pointer points to the final null;
    however if the input string runs out in the middle of a
    meta-character, the input pointer is left pointing at the start of
    that meta-character (the '&').  If the two buffers are the same, it
    may be the case that this character has been overwritten with the
    terminating null.
    </doc>
    <local>
    char
        *dest,                          //  Pointer to result string
        *end,                           //  Next character after meta-char
        code;                           //  Decoded metachar string
    size_t
        space_left;                     //  Space left in destination
    </local>
    //
    assert (input);
    assert (*input);
    assert (output);

    if (outmax == 0)                    //  Special case for zero space
        rc = 0;
    else {
        space_left = outmax - 1;        //  Allow for final null byte
        dest = output;
        for (; **input; (*input)++) {
            if (**input == '&') {
                end = strchr (*input, ';');
                if (end) {
                    code = ipr_http_decode_meta_charn ((*input) + 1, (size_t) (end - (*input) - 1));
                    if (code)
                        *input = end;   //  Skip past decoded metachar
                    else
                        code = **input; //  Ignore the &, no valid metachar
                }
                else                    //  The string ends before the ';'
                if (strlen (*input) > 10)
                    code = **input;     //  Ignore the &, no valid metachar
                else
                    break;
            }
            else
                code = **input;

            if (space_left > 0) {
                *dest++ = code;
                space_left --;
            }
        }
        *dest = 0;
        rc = (size_t) (dest - output);
    }
</method>

<method name = "decode meta charn" return = "rc">
    <argument name = "input"  type = "char *" />
    <argument name = "length" type = "size_t" />
    <declare name = "rc" type = "char" />
    <doc>
    Decodes a single meta-character (the piece from the character
    after the '&' up to but not including the ';'.  If the meta-character
    is valid, returns the character; otherwise returns zero.  Decodes both
    named and numeric meta-characters.  Use the macro decode_meta_char if
    the input has a terminating zero.
    </doc>
    <local>
#   define META_COUNT 102
    static char
        *meta [META_COUNT];             //  Metacharacter translation tables
    static byte
        code [META_COUNT];
    static Bool
        first_time = TRUE;              //  First time flag
    int
        char_index,                     //  Index into translation table
        min,                            //  Pointers for binary chop
        max,
        cmp,
        num;
    </local>
    //
    //  Initialise translation table first time through
    if (first_time) {
        first_time = FALSE;

        //  The meta-characters must be in alphabetical order so we can use
        //  a binary chop.
#if (defined (__UNIX__) || defined (__WINDOWS__))
        //  UNIX and Windows generally use ISO-8859-1 (Latin-1)
        code [  0] = 0xC6;  meta [  0] = "AElig";
        code [  1] = 0xC1;  meta [  1] = "Aacute";
        code [  2] = 0xC2;  meta [  2] = "Acirc";
        code [  3] = 0xC0;  meta [  3] = "Agrave";
        code [  4] = 0xC5;  meta [  4] = "Aring";
        code [  5] = 0xC3;  meta [  5] = "Atilde";
        code [  6] = 0xC4;  meta [  6] = "Auml";
        code [  7] = 0xC7;  meta [  7] = "Ccedil";
        code [  8] = 0xD0;  meta [  8] = "ETH";
        code [  9] = 0xC9;  meta [  9] = "Eacute";
        code [ 10] = 0xCA;  meta [ 10] = "Ecirc";
        code [ 11] = 0xC8;  meta [ 11] = "Egrave";
        code [ 12] = 0xCB;  meta [ 12] = "Euml";
        code [ 13] = 0xCD;  meta [ 13] = "Iacute";
        code [ 14] = 0xCE;  meta [ 14] = "Icirc";
        code [ 15] = 0xCC;  meta [ 15] = "Igrave";
        code [ 16] = 0xCF;  meta [ 16] = "Iuml";
        code [ 17] = 0xD1;  meta [ 17] = "Ntilde";
        code [ 18] = 0xD3;  meta [ 18] = "Oacute";
        code [ 19] = 0xD4;  meta [ 19] = "Ocirc";
        code [ 20] = 0xD2;  meta [ 20] = "Ograve";
        code [ 21] = 0xD8;  meta [ 21] = "Oslash";
        code [ 22] = 0xD5;  meta [ 22] = "Otilde";
        code [ 23] = 0xD6;  meta [ 23] = "Ouml";
        code [ 24] = 0xDE;  meta [ 24] = "THORN";
        code [ 25] = 0xDA;  meta [ 25] = "Uacute";
        code [ 26] = 0xDB;  meta [ 26] = "Ucirc";
        code [ 27] = 0xD9;  meta [ 27] = "Ugrave";
        code [ 28] = 0xDC;  meta [ 28] = "Uuml";
        code [ 29] = 0xDD;  meta [ 29] = "Yacute";
        code [ 30] = 0xE1;  meta [ 30] = "aacute";
        code [ 31] = 0xE2;  meta [ 31] = "acirc";
        code [ 32] = 0xB4;  meta [ 32] = "acute";
        code [ 33] = 0xE6;  meta [ 33] = "aelig";
        code [ 34] = 0xE0;  meta [ 34] = "agrave";
        code [ 35] = '&';   meta [ 35] = "amp";
        code [ 36] = 0xE5;  meta [ 36] = "aring";
        code [ 37] = 0xE3;  meta [ 37] = "atilde";
        code [ 38] = 0xE4;  meta [ 38] = "auml";
        code [ 39] = 0xA6;  meta [ 39] = "brvbar";
        code [ 40] = 0xE7;  meta [ 40] = "ccedil";
        code [ 41] = 0xB8;  meta [ 41] = "cedil";
        code [ 42] = 0xA2;  meta [ 42] = "cent";
        code [ 43] = 0xA9;  meta [ 43] = "copy";
        code [ 44] = 0xA4;  meta [ 44] = "curren";
        code [ 45] = 0xB0;  meta [ 45] = "deg";
        code [ 46] = 0xF7;  meta [ 46] = "divide";
        code [ 47] = 0xE9;  meta [ 47] = "eacute";
        code [ 48] = 0xEA;  meta [ 48] = "ecirc";
        code [ 49] = 0xE8;  meta [ 49] = "egrave";
        code [ 50] = 0xF0;  meta [ 50] = "eth";
        code [ 51] = 0xEB;  meta [ 51] = "euml";
        code [ 52] = 0xBD;  meta [ 52] = "frac12";
        code [ 53] = 0xBC;  meta [ 53] = "frac14";
        code [ 54] = 0xBE;  meta [ 54] = "frac34";
        code [ 55] = '>';   meta [ 55] = "gt";
        code [ 56] = 0xED;  meta [ 56] = "iacute";
        code [ 57] = 0xEE;  meta [ 57] = "icirc";
        code [ 58] = 0xA1;  meta [ 58] = "iexcl";
        code [ 59] = 0xEC;  meta [ 59] = "igrave";
        code [ 60] = 0xBF;  meta [ 60] = "iquest";
        code [ 61] = 0xEF;  meta [ 61] = "iuml";
        code [ 62] = 0xAB;  meta [ 62] = "laquo";
        code [ 63] = 0x91;  meta [ 63] = "lsquo";
        code [ 64] = '&lt;';   meta [ 64] = "lt";
        code [ 65] = 0xAF;  meta [ 65] = "macr";
        code [ 66] = 0xB5;  meta [ 66] = "micro";
        code [ 67] = 0xB7;  meta [ 67] = "middot";
        code [ 68] = ' ';   meta [ 68] = "nbsp";
        code [ 69] = 0xAC;  meta [ 69] = "not";
        code [ 70] = 0xF1;  meta [ 70] = "ntilde";
        code [ 71] = 0xF3;  meta [ 71] = "oacute";
        code [ 72] = 0xF4;  meta [ 72] = "ocirc";
        code [ 73] = 0xF2;  meta [ 73] = "ograve";
        code [ 74] = 0xAA;  meta [ 74] = "ordf";
        code [ 75] = 0xBA;  meta [ 75] = "ordm";
        code [ 76] = 0xF8;  meta [ 76] = "oslash";
        code [ 77] = 0xF5;  meta [ 77] = "otilde";
        code [ 78] = 0xF6;  meta [ 78] = "ouml";
        code [ 79] = 0xB6;  meta [ 79] = "para";
        code [ 80] = 0xB1;  meta [ 80] = "plusmn";
        code [ 81] = 0xA3;  meta [ 81] = "pound";
        code [ 82] = '"';   meta [ 82] = "quot";
        code [ 83] = 0xBB;  meta [ 83] = "raquo";
        code [ 84] = 0xAE;  meta [ 84] = "reg";
        code [ 85] = 0x92;  meta [ 85] = "rsquo";
        code [ 86] = 0xA7;  meta [ 86] = "sect";
        code [ 87] = 0xAD;  meta [ 87] = "shy";
        code [ 88] = 0xB9;  meta [ 88] = "sup1";
        code [ 89] = 0xB2;  meta [ 89] = "sup2";
        code [ 90] = 0xB3;  meta [ 90] = "sup3";
        code [ 91] = 0xDF;  meta [ 91] = "szlig";
        code [ 92] = 0xFE;  meta [ 92] = "thorn";
        code [ 93] = 0xD7;  meta [ 93] = "times";
        code [ 94] = 0xFA;  meta [ 94] = "uacute";
        code [ 95] = 0xFB;  meta [ 95] = "ucirc";
        code [ 96] = 0xF9;  meta [ 96] = "ugrave";
        code [ 97] = 0xA8;  meta [ 97] = "uml";
        code [ 98] = 0xFC;  meta [ 98] = "uuml";
        code [ 99] = 0xFD;  meta [ 99] = "yacute";
        code [100] = 0xA5;  meta [100] = "yen";
        code [101] = 0xFF;  meta [101] = "yuml";
#elif (defined (__MSDOS__))
        code [  0] = 0x92;  meta [  0] = "AElig";
        code [  1] = 0xB5;  meta [  1] = "Aacute";
        code [  2] = 0xB6;  meta [  2] = "Acirc";
        code [  3] = 0xB7;  meta [  3] = "Agrave";
        code [  4] = 0x8F;  meta [  4] = "Aring";
        code [  5] = 0xC7;  meta [  5] = "Atilde";
        code [  6] = 0x8E;  meta [  6] = "Auml";
        code [  7] = 0x80;  meta [  7] = "Ccedil";
        code [  8] = 0xD1;  meta [  8] = "ETH";
        code [  9] = 0x90;  meta [  9] = "Eacute";
        code [ 10] = 0xD2;  meta [ 10] = "Ecirc";
        code [ 11] = 0xD4;  meta [ 11] = "Egrave";
        code [ 12] = 0xD3;  meta [ 12] = "Euml";
        code [ 13] = 0xD6;  meta [ 13] = "Iacute";
        code [ 14] = 0xD7;  meta [ 14] = "Icirc";
        code [ 15] = 0xDE;  meta [ 15] = "Igrave";
        code [ 16] = 0xD8;  meta [ 16] = "Iuml";
        code [ 17] = 0xA5;  meta [ 17] = "Ntilde";
        code [ 18] = 0xE0;  meta [ 18] = "Oacute";
        code [ 19] = 0xE2;  meta [ 19] = "Ocirc";
        code [ 20] = 0xE3;  meta [ 20] = "Ograve";
        code [ 21] = 0x9D;  meta [ 21] = "Oslash";
        code [ 22] = 0xE5;  meta [ 22] = "Otilde";
        code [ 23] = 0x99;  meta [ 23] = "Ouml";
        code [ 24] = 0xE6;  meta [ 24] = "THORN";
        code [ 25] = 0xE9;  meta [ 25] = "Uacute";
        code [ 26] = 0xEA;  meta [ 26] = "Ucirc";
        code [ 27] = 0xEB;  meta [ 27] = "Ugrave";
        code [ 28] = 0x9A;  meta [ 28] = "Uuml";
        code [ 29] = 0xED;  meta [ 29] = "Yacute";
        code [ 30] = 0xA0;  meta [ 30] = "aacute";
        code [ 31] = 0x83;  meta [ 31] = "acirc";
        code [ 32] = 0xEF;  meta [ 32] = "acute";
        code [ 33] = 0x91;  meta [ 33] = "aelig";
        code [ 34] = 0x85;  meta [ 34] = "agrave";
        code [ 35] = '&';   meta [ 35] = "amp";
        code [ 36] = 0x86;  meta [ 36] = "aring";
        code [ 37] = 0xC6;  meta [ 37] = "atilde";
        code [ 38] = 0x84;  meta [ 38] = "auml";
        code [ 39] = 0xDD;  meta [ 39] = "brvbar";
        code [ 40] = 0x87;  meta [ 40] = "ccedil";
        code [ 41] = 0xB8;  meta [ 41] = "cedil";
        code [ 42] = 0xA2;  meta [ 42] = "cent";
        code [ 43] = 0xB8;  meta [ 43] = "copy";
        code [ 44] = 0xCF;  meta [ 44] = "curren";
        code [ 45] = 0xF8;  meta [ 45] = "deg";
        code [ 46] = 0xF6;  meta [ 46] = "divide";
        code [ 47] = 0x82;  meta [ 47] = "eacute";
        code [ 48] = 0x88;  meta [ 48] = "ecirc";
        code [ 49] = 0xCA;  meta [ 49] = "egrave";
        code [ 50] = 0xF0;  meta [ 50] = "eth";
        code [ 51] = 0x89;  meta [ 51] = "euml";
        code [ 52] = 0xAC;  meta [ 52] = "frac12";
        code [ 53] = 0xAB;  meta [ 53] = "frac14";
        code [ 54] = 0xF3;  meta [ 54] = "frac34";
        code [ 55] = '>';   meta [ 55] = "gt";
        code [ 56] = 0xA1;  meta [ 56] = "iacute";
        code [ 57] = 0x8C;  meta [ 57] = "icirc";
        code [ 58] = 0xAD;  meta [ 58] = "iexcl";
        code [ 59] = 0x8D;  meta [ 59] = "igrave";
        code [ 60] = 0xA8;  meta [ 60] = "iquest";
        code [ 61] = 0x8B;  meta [ 61] = "iuml";
        code [ 62] = 0xAE;  meta [ 62] = "laquo";
        code [ 63] = 0x91;  meta [ 63] = "lsquo";
        code [ 64] = '&lt;';   meta [ 64] = "lt";
        code [ 65] = 0xEE;  meta [ 65] = "macr";
        code [ 66] = 0xB5;  meta [ 66] = "micro";
        code [ 67] = 0xFA;  meta [ 67] = "middot";
        code [ 68] = ' ';   meta [ 68] = "nbsp";
        code [ 69] = 0xAA;  meta [ 69] = "not";
        code [ 70] = 0xA4;  meta [ 70] = "ntilde";
        code [ 71] = 0xF3;  meta [ 71] = "oacute";
        code [ 72] = 0x93;  meta [ 72] = "ocirc";
        code [ 73] = 0x95;  meta [ 73] = "ograve";
        code [ 74] = 0xA6;  meta [ 74] = "ordf";
        code [ 75] = 0xA7;  meta [ 75] = "ordm";
        code [ 76] = 0x9B;  meta [ 76] = "oslash";
        code [ 77] = 0xA2;  meta [ 77] = "otilde";
        code [ 78] = 0x94;  meta [ 78] = "ouml";
        code [ 79] = 0xF4;  meta [ 79] = "para";
        code [ 80] = 0xF1;  meta [ 80] = "plusmn";
        code [ 81] = 0x9C;  meta [ 81] = "pound";
        code [ 82] = '"';   meta [ 82] = "quot";
        code [ 83] = 0xAF;  meta [ 83] = "raquo";
        code [ 84] = 0xA9;  meta [ 84] = "reg";
        code [ 85] = 0x92;  meta [ 85] = "rsquo";
        code [ 86] = 0xF5;  meta [ 86] = "sect";
        code [ 87] = 0xB0;  meta [ 87] = "shy";
        code [ 88] = 0xFB;  meta [ 88] = "sup1";
        code [ 89] = 0xFD;  meta [ 89] = "sup2";
        code [ 90] = 0xFC;  meta [ 90] = "sup3";
        code [ 91] = 0xE1;  meta [ 91] = "szlig";
        code [ 92] = 0xFE;  meta [ 92] = "thorn";
        code [ 93] = 0x9E;  meta [ 93] = "times";
        code [ 94] = 0xA3;  meta [ 94] = "uacute";
        code [ 95] = 0x96;  meta [ 95] = "ucirc";
        code [ 96] = 0x97;  meta [ 96] = "ugrave";
        code [ 97] = 0xA8;  meta [ 97] = "uml";
        code [ 98] = 0x81;  meta [ 98] = "uuml";
        code [ 99] = 0xEC;  meta [ 99] = "yacute";
        code [100] = 0xA5;  meta [100] = "yen";
        code [101] = 0x98;  meta [101] = "yuml";
#endif
    }
    if (*input == '#') {                //  Numeric translation
        input++;
        num = 0;
        if (*input == 'x') {            //  Hex
            input++;
            num = s_decode_hex (&input, 0);
            input++;
        }
        else
            FOREVER {
            if ((*input >= '0') && (*input &lt;= '9'))
                num = (num * 10) + *input - '0';
            else
                break;

            input++;
        }
        if (*input != ';')
            num = 0;

        rc = (char) num;
    }
    else {                              //  Lookup meta-character
        rc = 0;
        min = 0;
        max = META_COUNT;
        while (max > min) {
            char_index = (max + min) / 2;
            cmp = strncmp (input, meta [char_index], length);
            if (cmp == 0) {
                rc = code [char_index];
                break;
            }
            if (cmp > 0)
                min = char_index + 1;
            else
                max = char_index;
        }
    }
</method>

<method name = "unescape uri" return = "uri">
    <doc>
    Removes HTTP hex escaping from a URI string by expanding any sequences
    of characters %xx.  Modifies the supplied string in place and returns
    it.
    </doc>
    <argument name = "uri" type = "char *">URI to unescape</argument>
    <local>
    char
        *nextch,
        *target;                        //  Where we store the result
    </local>
    //
    assert (uri);
    nextch = uri;
    target = uri;

    while (*nextch) {
        //  Unescape %xx sequence
        if (*nextch == '%' && nextch [1] && nextch [2]) {
            nextch++;
            *target = s_decode_hex (&nextch, 2);
            target++;
        }
        else {
            *target++ = *nextch;        //  Otherwise just copy
            nextch++;
        }
    }
    *target = 0;                        //  Terminate target string
</method>

<method name = "time str" return = "string">
    <doc>
    Returns the current date and time formatted using the HTTP standard
    format for log files: "DD/Mon/YYYY:hh:mm:ss".  The formatted time is
    returned in an icl_shortstr_t provided by the caller.
    </doc>
    <argument name = "string" type = "char *">String to receive formatted time</argument>
    <local>
    static char
        *months = "Jan\\0Feb\\0Mar\\0Apr\\0May\\0Jun\\0Jul\\0Aug\\0Sep\\0Oct\\0Nov\\0Dec";
    apr_time_t
        apr_time;
    apr_time_exp_t
        time_exp;
    </local>
    //
    apr_time = apr_time_now ();
    apr_time_exp_lt (&time_exp, apr_time);
    sprintf (string, "%02d/%s/%04d:%02d:%02d:%02d %+03d:%02d",
        time_exp.tm_mday, months + time_exp.tm_mon * 4, time_exp.tm_year + 1900,
        time_exp.tm_hour, time_exp.tm_min, time_exp.tm_sec,
        (int) (time_exp.tm_gmtoff / 3600), (int) (time_exp.tm_gmtoff % 3600));
</method>

<private name = "header">
static size_t
    s_encode_meta_char (char *output, char code, size_t outmax, Bool html);
static int
    s_decode_hex (char **input, size_t outmax);
    </private>

<private name = "footer">
/*
    Translates one character into HTML/SGML metacharacters.  You
    supply an output buffer and specify the maximum size of this buffer.
    Returns the final size of the translated data excluding the final null
    byte.  If the resulting data is too long, translation does not occur
    and the returned value is zero.  If html is TRUE then the metacharacters
    cr, amp, lt, gt and quot are not translated - this allows you to encode
    accented characters within HTML; otherwise they are translated and the
    output doesn't look like HTML.
*/
static size_t
    s_encode_meta_char (char *output, char code, size_t outmax, Bool html)
{
    static char
        *meta [256];                    //  Metacharacter translation table
    static Bool
        first_time = TRUE;              //  First time flag
    size_t
        length;                         //  Length of translation
    char
        *meta_char,                     //  Pointer through metachar string
        buffer [10];                    //  Buffer for conversions

    //  Initialise translation table first time through
    if (first_time) {
        first_time = FALSE;
        memset (meta, 0, sizeof (meta));

#if (defined (__UNIX__) || defined (__WINDOWS__))
        //  UNIX and Windows generally use ISO-8859-1 (Latin-1)
        meta [0x91] = "lsquo";
        meta [0x92] = "rsquo";
        meta [0xA1] = "iexcl";
        meta [0xA2] = "cent";
        meta [0xA3] = "pound";
        meta [0xA4] = "curren";
        meta [0xA5] = "yen";
        meta [0xA6] = "brvbar";
        meta [0xA7] = "sect";
        meta [0xA8] = "uml";
        meta [0xA9] = "copy";
        meta [0xAA] = "ordf";
        meta [0xAB] = "laquo";
        meta [0xAC] = "not";
        meta [0xAD] = "shy";
        meta [0xAE] = "reg";
        meta [0xAF] = "macr";
        meta [0xB0] = "deg";
        meta [0xB1] = "plusmn";
        meta [0xB2] = "sup2";
        meta [0xB3] = "sup3";
        meta [0xB4] = "acute";
        meta [0xB5] = "micro";
        meta [0xB6] = "para";
        meta [0xB7] = "middot";
        meta [0xB8] = "cedil";
        meta [0xB9] = "sup1";
        meta [0xBA] = "ordm";
        meta [0xBB] = "raquo";
        meta [0xBC] = "frac14";
        meta [0xBD] = "frac12";
        meta [0xBE] = "frac34";
        meta [0xBF] = "iquest";
        meta [0xC0] = "Agrave";
        meta [0xC1] = "Aacute";
        meta [0xC2] = "Acirc";
        meta [0xC3] = "Atilde";
        meta [0xC4] = "Auml";
        meta [0xC5] = "Aring";
        meta [0xC6] = "AElig";
        meta [0xC7] = "Ccedil";
        meta [0xC8] = "Egrave";
        meta [0xC9] = "Eacute";
        meta [0xCA] = "Ecirc";
        meta [0xCB] = "Euml";
        meta [0xCC] = "Igrave";
        meta [0xCD] = "Iacute";
        meta [0xCE] = "Icirc";
        meta [0xCF] = "Iuml";
        meta [0xD0] = "ETH";
        meta [0xD1] = "Ntilde";
        meta [0xD2] = "Ograve";
        meta [0xD3] = "Oacute";
        meta [0xD4] = "Ocirc";
        meta [0xD5] = "Otilde";
        meta [0xD6] = "Ouml";
        meta [0xD7] = "times";
        meta [0xD8] = "Oslash";
        meta [0xD9] = "Ugrave";
        meta [0xDA] = "Uacute";
        meta [0xDB] = "Ucirc";
        meta [0xDC] = "Uuml";
        meta [0xDD] = "Yacute";
        meta [0xDE] = "THORN";
        meta [0xDF] = "szlig";
        meta [0xE0] = "agrave";
        meta [0xE1] = "aacute";
        meta [0xE2] = "acirc";
        meta [0xE3] = "atilde";
        meta [0xE4] = "auml";
        meta [0xE5] = "aring";
        meta [0xE6] = "aelig";
        meta [0xE7] = "ccedil";
        meta [0xE8] = "egrave";
        meta [0xE9] = "eacute";
        meta [0xEA] = "ecirc";
        meta [0xEB] = "euml";
        meta [0xEC] = "igrave";
        meta [0xED] = "iacute";
        meta [0xEE] = "icirc";
        meta [0xEF] = "iuml";
        meta [0xF0] = "eth";
        meta [0xF1] = "ntilde";
        meta [0xF2] = "ograve";
        meta [0xF3] = "oacute";
        meta [0xF4] = "ocirc";
        meta [0xF5] = "otilde";
        meta [0xF6] = "ouml";
        meta [0xF7] = "divide";
        meta [0xF8] = "oslash";
        meta [0xF9] = "ugrave";
        meta [0xFA] = "uacute";
        meta [0xFB] = "ucirc";
        meta [0xFC] = "uuml";
        meta [0xFD] = "yacute";
        meta [0xFE] = "thorn";
        meta [0xFF] = "yuml";

#elif (defined (__MSDOS__))
        //  DOS generally uses the PC-1 alphabet
        meta [0x80] = "Ccedil";
        meta [0x81] = "uuml";
        meta [0x82] = "eacute";
        meta [0x83] = "acirc";
        meta [0x84] = "auml";
        meta [0x85] = "agrave";
        meta [0x86] = "aring";
        meta [0x87] = "ccedil";
        meta [0x88] = "ecirc";
        meta [0x89] = "euml";
        meta [0x8B] = "iuml";
        meta [0x8C] = "icirc";
        meta [0x8D] = "igrave";
        meta [0x8E] = "Auml";
        meta [0x8F] = "Aring";
        meta [0x90] = "Eacute";
        meta [0x91] = "aelig";
        meta [0x92] = "AElig";
        meta [0x93] = "ocirc";
        meta [0x94] = "ouml";
        meta [0x95] = "ograve";
        meta [0x96] = "ucirc";
        meta [0x97] = "ugrave";
        meta [0x98] = "yuml";
        meta [0x99] = "Ouml";
        meta [0x9A] = "Uuml";
        meta [0x9B] = "oslash";
        meta [0x9C] = "pound";
        meta [0x9D] = "Oslash";
        meta [0x9E] = "times";
        meta [0xA0] = "aacute";
        meta [0xA1] = "iacute";
        meta [0xA2] = "otilde";
        meta [0xA3] = "uacute";
        meta [0xA4] = "ntilde";
        meta [0xA5] = "Ntilde";
        meta [0xA6] = "ordf";
        meta [0xA7] = "ordm";
        meta [0xA8] = "iquest";
        meta [0xA9] = "reg";
        meta [0xAA] = "not";
        meta [0xAB] = "frac14";
        meta [0xAC] = "frac12";
        meta [0xAD] = "iexcl";
        meta [0xAE] = "laquo";
        meta [0xAF] = "raquo";
        meta [0xB0] = "shy";
        meta [0xB5] = "Aacute";
        meta [0xB6] = "Acirc";
        meta [0xB7] = "Agrave";
        meta [0xB8] = "copy";
        meta [0xC6] = "atilde";
        meta [0xC7] = "Atilde";
        meta [0xCA] = "egrave";
        meta [0xCF] = "curren";
        meta [0xD1] = "ETH";
        meta [0xD2] = "Ecirc";
        meta [0xD3] = "Euml";
        meta [0xD4] = "Egrave";
        meta [0xD6] = "Iacute";
        meta [0xD7] = "Icirc";
        meta [0xD8] = "Iuml";
        meta [0xDD] = "brvbar";
        meta [0xDE] = "Igrave";
        meta [0xE0] = "Oacute";
        meta [0xE1] = "szlig";
        meta [0xE2] = "Ocirc";
        meta [0xE3] = "Ograve";
        meta [0xE5] = "Otilde";
        meta [0xE6] = "THORN";
        meta [0xE9] = "Uacute";
        meta [0xEA] = "Ucirc";
        meta [0xEB] = "Ugrave";
        meta [0xEC] = "yacute";
        meta [0xED] = "Yacute";
        meta [0xEE] = "macr";
        meta [0xEF] = "acute";
        meta [0xF1] = "plusmn";
        meta [0xF3] = "frac34";
        meta [0xF4] = "para";
        meta [0xF5] = "sect";
        meta [0xF6] = "divide";
        meta [0xF8] = "deg";
        meta [0xFA] = "middot";
        meta [0xFB] = "sup1";
        meta [0xFC] = "sup3";
        meta [0xFD] = "sup2";
#endif
    }
    meta_char = meta [(int) code & 255];
    if (meta_char == 0) {
        if (html) {
            output [0] = code;
            return (1);
        }
        else {
            switch (code) {
                case '&':
                    meta_char = "amp";
                    break;
                case '&lt;':
                    meta_char = "lt";
                    break;
                case '>':
                    meta_char = "gt";
                    break;
                case '"':
                    meta_char = "quot";
                    break;
                default:
                    output [0] = code;
                    return (1);
            }
        }
    }
    apr_snprintf (buffer, sizeof (buffer), "&%s;", meta_char);
    length = strlen (buffer);
    if (length &lt; outmax) {
        strncpy (output, buffer, outmax);
        return length;
    }
    else {
        output [0] = 0;
        return (0);
    }
}

/*
    Decodes a hexadecimal string.  Stops after outmax characters
    or when an invalid hex character is reached.  Sets the input pointer
    to the first unprocessed character.  Returns the result.
*/

static int
s_decode_hex (char **input, size_t outmax)
{
    static char
        hex_to_bin [128] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
         0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,//  0..9
        -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,//  A..F
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,//  a..f
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
    };
    int
        nextch;
    size_t
        index;
    int
        result;

    assert (input);
    assert (*input);

    index  = 0;
    result = 0;
    while (outmax == 0 || index &lt; outmax) {
        nextch = (*input) [index] & 127;
        if (nextch && hex_to_bin [nextch] != -1) {
            result = result * 16 + hex_to_bin [nextch];
            index++;
        }
        else
            break;
    }
    (*input) += index;
    return (result);
}
</private>

<method name = "selftest">
    char
        *uri,
        *text_ptr;
    icl_shortstr_t
        html_string,
        text_string;

    uri = icl_mem_strdup ("This%20is%20%61%20string");
    ipr_http_unescape_uri (uri);
    assert (streq (uri, "This is a string"));
    icl_mem_free (uri);

    text_ptr = "<&?>";
    ipr_http_encode_meta (html_string, &text_ptr, ICL_SHORTSTR_MAX, TRUE);
    assert (streq (html_string, "<&?>"));

    text_ptr = "aBc";
    ipr_http_encode_meta (html_string, &text_ptr, ICL_SHORTSTR_MAX, FALSE);
    assert (streq (html_string, "aBc"));

    text_ptr = "<&?>";
    ipr_http_encode_meta (html_string, &text_ptr, ICL_SHORTSTR_MAX, FALSE);
    text_ptr = html_string;
    ipr_http_decode_meta (text_string, &text_ptr, ICL_SHORTSTR_MAX);
    assert (streq (text_string, "<&?>"));

    icl_shortstr_fmt (html_string, "%%%02x-%%%02x-%%%02x", 'a', 'B', 'c');
    ipr_http_unescape_uri (html_string);
    assert (streq (html_string, "a-B-c"));

    ipr_http_time_str (text_string);
    assert (ipr_regexp_eq ("[0-9]{2}/[a-zA-Z]{3}/[0-9]{4}:[0-9]{2}:[0-9]{2}:[0-9]{2}", text_string));
</method>

</class>
