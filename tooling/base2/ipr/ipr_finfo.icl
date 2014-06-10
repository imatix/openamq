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
    name      = "ipr_finfo"
    comment   = "File information and status"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class wraps the APR finfo object so that we can use it in iCL
    lists, etc.  Generates a list container - ipr_finfo_list.
</doc>

<inherit class = "icl_object" />
<inherit class = "icl_list_item">
    <option name = "count" value = "1" />
</inherit>
<option name = "alloc" value = "cache" />

<import class = "ipr_time" />

<context>
    icl_shortstr_t
        name;                           //  File name, without path
    apr_fileperms_t
        protection;                     //  Access permissions
    apr_filetype_e
        filetype;                       //  Type of file - APR_REG, APR_DIR,...
    apr_ino_t
        inode;                          //  inode on file system
    apr_dev_t
        device;                         //  ID of device that file is on
    apr_int32_t
        nlink;                          //  Number of hard links to file
    apr_uid_t
        user;                           //  Owning user id
    apr_gid_t
        group;                          //  Owning group id
    apr_off_t
        size;                           //  Size of file in bytes
    apr_time_t
        atime;                          //  Time file was last accessed
    apr_time_t
        mtime;                          //  Time file was last modified
    apr_time_t
        ctime;                          //  Time file was created
</context>

<method name = "new">
    <argument name = "finfo" type = "apr_finfo_t *">APR finfo struct</argument>
    icl_shortstr_cpy (self->name, (char *) finfo->name);
    self->protection = finfo->protection;
    self->filetype   = finfo->filetype;
    self->inode      = finfo->inode;
    self->device     = finfo->device;
    self->nlink      = finfo->nlink;
    self->user       = finfo->user;
    self->group      = finfo->group;
    self->size       = finfo->size;
    self->atime      = finfo->atime;
    self->mtime      = finfo->mtime;
    self->ctime      = finfo->ctime;
</method>

<method name = "matches" template = "function">
    <doc>
    Returns TRUE if the file name matches ths specified pattern which is
    a wildcard that can contain ? (one character) and * (zero or more
    characters).  On Windows, does a case-insensitive comparison.
    </doc>
    <argument name = "pattern" type = "char *">Wildcard pattern</argument>
    <local>
    char
        *pattern_ptr,                   //  Points to pattern
        *filename_ptr;                  //  Points to filename
    </local>
    //
    assert (pattern);

    filename_ptr = (char *) self->name;
    pattern_ptr = pattern;
    FOREVER {
        //  If at end of pattern and filename, we have match
        if (*pattern_ptr == 0 && *filename_ptr == 0) {
            rc = TRUE;                  //  Successful match
            break;
        }
        //  If end of either but not both, match failed
        if (*pattern_ptr == 0 || *filename_ptr == 0) {
            rc = FALSE;                 //  No match
            break;
        }
        //  If pattern character is '?', we match one char
        if (*pattern_ptr == '?'
#if (defined (__WINDOWS__))
        ||  toupper (*pattern_ptr) == toupper (*filename_ptr)) {
#else
        ||  *pattern_ptr == *filename_ptr) {
#endif
            pattern_ptr++;
            filename_ptr++;
        }
        else
        //  If pattern character is '*', we match as much as possible
        if (*pattern_ptr == '*') {
            pattern_ptr++;              //  Try to match following char
            while (*filename_ptr && *filename_ptr != *pattern_ptr)
                filename_ptr++;
        }
        else {
            rc = FALSE;                 //  No match
            break;
        }
    }
</method>

<method name = "print" template = "function">
    <doc>
    Formats the file information into a printable line of text. The result
    is formatted into a icl_shortstr_t provided by the caller.
    </doc>
    <argument name = "buffer" type = "char *">Buffer to prepare</argument>
    <local>
    icl_shortstr_t
        formatted_time;
    </local>
    //
    ipr_time_iso8601 (
        self->mtime, ipr_date_format_minute, 0, ipr_time_zone (), formatted_time);

    icl_shortstr_fmt (buffer, "%c%c%c%c%c%c%c%c%c%c %8ld %s %s%c",
        self->filetype == APR_DIR? 'd': '-',
        self->protection & APR_FPROT_UREAD?    'r': '-',
        self->protection & APR_FPROT_UWRITE?   'w': '-',
        self->protection & APR_FPROT_UEXECUTE? 'x': '-',
        self->protection & APR_FPROT_GREAD?    'r': '-',
        self->protection & APR_FPROT_GWRITE?   'w': '-',
        self->protection & APR_FPROT_GEXECUTE? 'x': '-',
        self->protection & APR_FPROT_WREAD?    'r': '-',
        self->protection & APR_FPROT_WWRITE?   'w': '-',
        self->protection & APR_FPROT_WEXECUTE? 'x': '-',
        (long) self->size,
        formatted_time,
        self->name,
        self->filetype == APR_DIR? '/':
            self->protection & APR_FPROT_UEXECUTE? '*':
            ' ');
</method>

<method name = "selftest">
    ipr_finfo_list_t
        *finfo_list;                    //  List of files in directory
    apr_pool_t
        *pool;                          //  Pool for all allocations
    apr_dir_t
        *dir = NULL;
    apr_status_t
        rc;
    apr_finfo_t
        apr_finfo;
    ipr_finfo_t
        *finfo;
    icl_shortstr_t
        finfo_print;
    //
    //  Load current directory into finfo list
    apr_pool_create (&pool, NULL);
    assert (apr_dir_open (&dir, ".", pool) == APR_SUCCESS);

    finfo_list = ipr_finfo_list_new ();
    rc = apr_dir_read (&apr_finfo, APR_FINFO_MIN + APR_FINFO_OWNER, dir);
    while (rc == APR_SUCCESS || rc == APR_INCOMPLETE) {
        finfo = ipr_finfo_new (&apr_finfo);
        if (ipr_finfo_matches (finfo, "*.icl")) {
            ipr_finfo_list_push (finfo_list, finfo);
            ipr_finfo_print (finfo, finfo_print);
        }
        ipr_finfo_unlink (&finfo);
        rc = apr_dir_read (&apr_finfo, APR_FINFO_MIN + APR_FINFO_OWNER, dir);
    }
    apr_dir_close (dir);

    ipr_finfo_list_destroy (&finfo_list);
    apr_pool_destroy (pool);
</method>

</class>
