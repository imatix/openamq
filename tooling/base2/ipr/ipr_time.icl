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
    name      = "ipr_time"
    comment   = "Time manipulation"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Provides various date & time manipulation functions.
</doc>

<inherit class = "icl_base" />

<import class = "ipr_str" />
<import class = "ipr_regexp" />

<public>
typedef enum
{
    // YYYY                     (will render to 4 characters)
    ipr_date_format_year,
    // YYYY-MM                  (will render to 7 characters)
    ipr_date_format_month,
    // YYYY-MM-DD               (will render to 10 characters)
    ipr_date_format_day,
    // YYYY-MM-DDThh:mmTZD      (will render to 22 characters)
    ipr_date_format_minute,
    // YYYY-MM-DDThh:mm:ssTZD   (will render to 25 characters)
    ipr_date_format_second,
    // YYYY-MM-DDThh:mm:ss.sTZD (will render to 26 + precision characters)
    ipr_date_format_fraction
} ipr_date_format_t;

</public>

<method name = "iso8601" return = "buffer">
    <doc>
    Converts a time value into an ISO8601 formatted date.  Returns buffer.
    </doc>
    <argument name = "time" type = "apr_time_t">Time to render</argument>
    <argument name = "format" type = "ipr_date_format_t">Format to use</argument>
    <argument name = "precision" type = "byte">Decimals for second fractions</argument>
    <argument name = "utc" type = "Bool">Format as UTC?</argument>
    <argument name = "buffer" type = "char *">Rendered output</argument>
    <local>
    apr_time_exp_t
        exp;
    char
        fraction [8];
    char
        time_zone [12];
    </local>
    //
    if (utc) {
        icl_apr_assert (apr_time_exp_gmt (&exp, time));
        sprintf (time_zone, "Z");
    }
    else {
        icl_apr_assert (apr_time_exp_lt (&exp, time));
        sprintf (time_zone, "%+03d:%02d",
            (int) (exp.tm_gmtoff / 3600),
            (int) (exp.tm_gmtoff % 3600));
    }
    switch (format) {
        case ipr_date_format_year:
            sprintf (buffer, "%04d", (int) exp.tm_year + 1900);
            break;

        case ipr_date_format_month:
            sprintf (buffer, "%04d-%02d", (int) exp.tm_year + 1900,
                (int) exp.tm_mon + 1);
            break;

        case ipr_date_format_day:
            sprintf (buffer, "%04d-%02d-%02d", (int) exp.tm_year + 1900,
                (int) exp.tm_mon + 1, (int) exp.tm_mday);
            break;

        case ipr_date_format_minute:
            sprintf (buffer, "%04d-%02d-%02dT%02d:%02d%s",
                (int) exp.tm_year + 1900, (int) exp.tm_mon + 1, (int) exp.tm_mday,
                (int) exp.tm_hour, (int) exp.tm_min, time_zone);
            break;

        case ipr_date_format_second:
            sprintf (buffer, "%04d-%02d-%02dT%02d:%02d:%02d%s",
                (int) exp.tm_year + 1900, (int) exp.tm_mon + 1, (int) exp.tm_mday,
                (int) exp.tm_hour, (int) exp.tm_min, (int) exp.tm_sec,
                time_zone);
            break;

        case ipr_date_format_fraction:
            sprintf (fraction, "%06d", (int) exp.tm_usec);
            fraction [precision <= 6 ? precision : 6] = 0;
            sprintf (buffer, "%04d-%02d-%02dT%02d:%02d:%02d.%s%s",
                (int) exp.tm_year + 1900, (int) exp.tm_mon + 1, (int) exp.tm_mday,
                (int) exp.tm_hour, (int) exp.tm_min, (int) exp.tm_sec,
                fraction, time_zone);
            break;

        default:
            assert (0);
    }
</method>

<method name = "iso8601 decode" return = "decoded">
    <doc>
    Takes an ISO8601 date/time string in various formats and converts to
    an apr_time_t.  Returns an apr_time_t if ok, else returns zero. Accepts
    these formats:
    YYYY
    YYYY-MM
    YYYY-MM-DD
    YYYY-MM-DDThh:mmTZD
    YYYY-MM-DDThh:mm:ssTZD
    YYYY-MM-DDThh:mm:ss.sTZD
    The returned date and time are in UTC.
    </doc>
    <argument name = "iso8601" type = "char *">ISO format date/time</argument>
    <declare name = "decoded" type = "apr_time_t" default = "0">Returned date/time</declare>
    <local>
    apr_time_exp_t
        exp;
    ipr_regexp_t
        *regexp;
    char
        *year = NULL,
        *month = NULL,
        *day = NULL,
        *hour = NULL,
        *min = NULL,
        *sec = NULL,
        *fraction = NULL,
        *sign = NULL,
        *tz_hour = NULL,
        *tz_min = NULL;
    </local>
    //
    assert (iso8601);
    regexp = ipr_regexp_new (
        "([0-9]{4})(?:-([0-9]{2})(?:-([0-9]{2})" \\
        "(?:T([0-9]{2}):([0-9]{2})(?::([0-9]{2})(?:(`.[0-9]+))?)?" \\
        "(?:Z|(?:([-+])([0-9]{2}):([0-9]{2})))?)?)?)?");

    if (ipr_regexp_match (regexp, iso8601,
        &year, &month, &day, &hour, &min, &sec, &fraction, &sign, &tz_hour, &tz_min) == 10) {

        exp.tm_year = atoi (year) - 1900;
        exp.tm_mon  = *month? atoi (month) - 1: 0;
        exp.tm_mday = atoi (day);
        exp.tm_hour = atoi (hour);
        exp.tm_min  = atoi (min);
        exp.tm_sec  = atoi (sec);
        exp.tm_usec = (uint) strtod (fraction, NULL);
        exp.tm_gmtoff = atoi (tz_hour) * 60 + atoi (tz_min);
        if (*sign == '-')
            exp.tm_gmtoff *= -1;

        //  APR does not check the date, and is easily crashed with invalid dates,
        //  so we will check the date before getting APR to convert it.
        if (s_exp_valid (&exp))
            apr_time_exp_gmt_get (&decoded, &exp);

        icl_mem_free (year);
        icl_mem_free (month);
        icl_mem_free (day);
        icl_mem_free (hour);
        icl_mem_free (min);
        icl_mem_free (sec);
        icl_mem_free (fraction);
        icl_mem_free (sign);
        icl_mem_free (tz_hour);
        icl_mem_free (tz_min);
    }
    ipr_regexp_destroy (&regexp);
</method>

<method name = "mime" return = "buffer">
    <doc>
    Encode date and time (in long format) in MIME RFC1123 date format, e.g.
    Mon, 12 Jan 1995 12:05:01 GMT.  Returns buffer.
    </doc>
    <argument name = "time" type = "apr_time_t">Time to format</argument>
    <argument name = "buffer" type = "char *">Shortstring for result</argument>
    <local>
    apr_time_exp_t
        exp;
    char
        *months [] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
            };
    char
        *days [] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    </local>
    //
    if (apr_time_exp_gmt (&exp, time) == APR_SUCCESS)
        icl_shortstr_fmt (buffer, "%s, %02d %s %04d %02d:%02d:%02d UTC",
            days [exp.tm_wday],
            exp.tm_mday,
            months [exp.tm_mon],
            exp.tm_year + 1900,
            exp.tm_hour,
            exp.tm_min,
            exp.tm_sec);
</method>

<method name = "mime decode" return = "decoded">
    <doc>
    Takes a MIME date and time string in various formats and converts to
    an apr_time_t.  Returns an apr_time_t if ok, else returns zero. Accepts
    these formats:
    Mon Jan 12 12:05:01 1995            ctime format
    Monday, 12-Jan-95 12:05:01 GMT      RFC 850
    Monday, 12-Jan-1995 12:05:01 GMT    RFC 850 iMatix extension
    Mon, 12 Jan 1995 12:05:01 GMT       RFC 1123
    The returned date and time are in UTC.
    </doc>
    <argument name = "mime" type = "char *">MIME-encoded date/time</argument>
    <declare name = "decoded" type = "apr_time_t" default = "0">Returned date/time</declare>
    <local>
    apr_time_exp_t
        exp;
    char
        month_name [20],
        buffer [50];
    char
        *months [] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
            };
    </local>
    //
    assert (mime);

    //  Whatever format we're looking at, it will start with weekday.
    //  Skip to first space.
    mime = strchr (mime, ' ');
    if (mime) {
        mime = ipr_str_skip (mime);
        if (isalpha (*mime))
            //  ctime
            sscanf (mime, "%s %d %d:%d:%d %d",
                month_name, &exp.tm_mday, &exp.tm_hour, &exp.tm_min, &exp.tm_sec, &exp.tm_year);
        else
        if (mime [2] == '-') {
            //  RFC 850
            sscanf (mime, "%s %d:%d:%d", buffer, &exp.tm_hour, &exp.tm_min, &exp.tm_sec);
            buffer [2] = 0;
            exp.tm_mday = atoi (buffer);
            buffer [6] = 0;
            strcpy (month_name, &buffer [3]);
            exp.tm_year = atoi (&buffer [7]);
        }
        else
            //  RFC 1123
            sscanf (mime, "%d %s %d %d:%d:%d",
                &exp.tm_mday, month_name, &exp.tm_year, &exp.tm_hour, &exp.tm_min, &exp.tm_sec);

        //  Year does not include century
        if (exp.tm_year > 1900)
            exp.tm_year -= 1900;

        //  Translate month name
        for (exp.tm_mon = 0; exp.tm_mon < 12; exp.tm_mon++)
            if (ipr_str_lexeq (months [exp.tm_mon], month_name))
                break;

        //  Exploded time is in UTC
        exp.tm_gmtoff = 0;
        exp.tm_usec   = 0;
        exp.tm_isdst  = 0;
        exp.tm_wday   = 0;
        exp.tm_yday   = 0;

        //  APR does not check the date, and is easily crashed with invalid dates,
        //  so we will check the date before getting APR to convert it.
        if (s_exp_valid (&exp))
            apr_time_exp_gmt_get (&decoded, &exp);
    }
</method>

<method name = "now" return = "time_now">
    <doc>
    Returns the current local time as a HHMMSS value from 0 to 235959.
    </doc>
    <declare name = "time_now" type = "long">Time to return</declare>
    <local>
    apr_time_t
        apr_time;
    apr_time_exp_t
        time_exp;
    </local>
    //
    apr_time = apr_time_now ();
    apr_time_exp_lt (&time_exp, apr_time);
    time_now = time_exp.tm_hour * 10000 + time_exp.tm_min * 100 + time_exp.tm_sec;
</method>

<method name = "zone" return = "seconds">
    <doc>
    Returns current timezone as number of seconds to add to UTC to get local
    time.  This is negative for timezones east of UTC and positive for timezones
    west of UTC.  If timezone support is not available, returns zero.
    </doc>
    <declare name = "seconds" type = "long">Time zone</declare>
    <local>
    static Bool
        first_time = TRUE;
    </local>
    if (first_time) {
        tzset ();
        first_time = FALSE;
    }
    seconds = timezone;
</method>

<private name = "header">
static Bool s_exp_valid (apr_time_exp_t *exp);
</private>

<private name = "footer">
static Bool s_exp_valid (apr_time_exp_t *exp)
{
    int
        year;
    Bool
        is_leap,
        rc;
    static byte
        month_days [] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    year = exp->tm_year + 1900;
    is_leap = ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);

    if (exp->tm_year < 2900
    && ((exp->tm_mon < 12 && exp->tm_mday <= month_days [exp->tm_mon])
    ||  (exp->tm_mon == 1 && exp->tm_mday == 29 && !is_leap))
    &&   exp->tm_hour < 24
    &&   exp->tm_min  < 60
    &&   exp->tm_sec  < 60)
        rc = TRUE;
    else
        rc = FALSE;

    return (rc);
}
</private>

<method name = "selftest">
    <local>
    char
        buffer [100];
    apr_time_exp_t
        exp = {123456, 6, 5, 4, 3, 2, 99, 0, 0, 0, -5401};
    apr_time_t
        tm,
        tm2;
    </local>
    //
    apr_time_exp_get (&tm, &exp);
    ipr_time_iso8601 (tm, ipr_date_format_year,     0, FALSE, buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    ipr_time_iso8601 (tm, ipr_date_format_month,    0, FALSE, buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    ipr_time_iso8601 (tm, ipr_date_format_day,      0, FALSE, buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    ipr_time_iso8601 (tm, ipr_date_format_minute,   0, TRUE,  buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    ipr_time_iso8601 (tm, ipr_date_format_minute,   0, FALSE, buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    ipr_time_iso8601 (tm, ipr_date_format_second,   0, TRUE,  buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    ipr_time_iso8601 (tm, ipr_date_format_second,   0, FALSE, buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    ipr_time_iso8601 (tm, ipr_date_format_fraction, 2, TRUE,  buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    ipr_time_iso8601 (tm, ipr_date_format_fraction, 2, FALSE, buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    ipr_time_iso8601 (tm, ipr_date_format_fraction, 6, TRUE,  buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    ipr_time_iso8601 (tm, ipr_date_format_fraction, 6, FALSE, buffer);
    tm2 = ipr_time_iso8601_decode (buffer);

    //  Check that MIME decoding works
    assert (ipr_time_mime_decode ("Mon Jan 12 12:05:01 1995"));
    assert (ipr_time_mime_decode ("Monday, 12-Jan-95 12:05:01 GMT"));
    assert (ipr_time_mime_decode ("Monday, 12-Jan-1995 12:05:01 GMT"));
    assert (ipr_time_mime_decode ("Mon, 12 Jan 1995 12:05:01 GMT"));

    assert (ipr_time_mime_decode ("Mon Junk 12 12:05:01 1995") == 0);
    assert (ipr_time_mime_decode ("Monday, 32-Jan-95 12:05:01 GMT") == 0);
    assert (ipr_time_mime_decode ("Monday, 12-Jan-1995 65:05:01 GMT") == 0);
    assert (ipr_time_mime_decode ("Mon, 12 Jan 1995 12:65:01 GMT") == 0);

    //  Check we can convert back and forwards correctly
    tm = apr_time_now ();
    ipr_time_mime (tm, buffer);
    tm2 = ipr_time_mime_decode (buffer);
    assert ((long) (tm / 1000000) == (long) (tm2 / 1000000));
</method>

</class>
