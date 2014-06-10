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
    name      = "ipr_dir"
    comment   = "Directory management"
    version   = "1.0"
    script    = "icl_gen"
    opaque    = "1"
    >
<doc>
    Provides a directory listing object that can be resorted in various
    ways.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "ipr_finfo" />
<import class = "ipr_str" />
<import class = "icl_shortstr" />
<import class = "ipr_file" />

<context>
    ipr_finfo_list_t
        *finfo_list;                    //  List of files in directory
    apr_pool_t
        *pool;                          //  Pool for all allocations

    <property name = "index_size" type = "uint" readonly = "1">Number of files in directory</property>
    <property name = "index" type = "ipr_finfo_t **" readonly = "1">Sorted list of finfo items</property>
</context>

<method name = "new">
    <argument name = "path" type = "char *">Directory path to load</argument>
    <argument name = "sortby" type = "char">Sort key</argument>
    <local>
    apr_dir_t
        *dir = NULL;
    apr_status_t
        rc;
    apr_finfo_t
        apr_finfo;
    ipr_finfo_t
        *finfo;
    </local>
    //
    apr_pool_create (&self->pool, NULL);
    self->finfo_list = ipr_finfo_list_new ();

    rc = apr_dir_open (&dir, path, self->pool);
    if (rc == APR_SUCCESS) {
        rc = apr_dir_read (&apr_finfo, APR_FINFO_MIN + APR_FINFO_OWNER, dir);
        while (rc == APR_SUCCESS || rc == APR_INCOMPLETE) {
            finfo = ipr_finfo_new (&apr_finfo);
            ipr_finfo_list_push (self->finfo_list, finfo);
            ipr_finfo_unlink (&finfo);
            rc = apr_dir_read (&apr_finfo, APR_FINFO_MIN + APR_FINFO_OWNER, dir);
        }
        apr_dir_close (dir);
        self_resort (self, sortby);
    }
    else
        self_destroy (&self);
</method>

<method name = "destroy">
    icl_mem_free (self->index);
    ipr_finfo_list_destroy (&self->finfo_list);
    apr_pool_destroy (self->pool);
    self->pool = NULL;
</method>

<method name = "resort" template = "function">
    <argument name = "sortby" type = "char">Sort key</argument>
    <local>
    ipr_finfo_t
        *finfo;
    uint
        finfo_nbr;
    </local>
    //
    //  Allocate an index to sort the finfo entries
    if (self->index)
        icl_mem_free (self->index);
    self->index_size = ipr_finfo_list_count (self->finfo_list);
    self->index = icl_mem_alloc (self->index_size * sizeof (ipr_finfo_t *));

    finfo = ipr_finfo_list_first (self->finfo_list);
    for (finfo_nbr = 0; finfo_nbr < self->index_size; finfo_nbr++) {
        self->index [finfo_nbr] = finfo;
        finfo = ipr_finfo_list_next (&finfo);
    }
    qsort (self->index, self->index_size, sizeof (ipr_finfo_t *),
        sortby == 'n'? s_sort_by_name:
        sortby == 't'? s_sort_by_time:
        sortby == 'T'? s_sort_by_time_desc:
        sortby == 's'? s_sort_by_size:
        sortby == 'S'? s_sort_by_size_desc:
                       s_sort_by_name);
</method>

<method name = "dump" template = "function">
    <local>
    ipr_finfo_t
        *finfo;
    uint
        finfo_nbr;
    icl_shortstr_t
        buffer;                         //  Formatted directory entry
    </local>
    //
    assert (self->index);
    for (finfo_nbr = 0; finfo_nbr < self->index_size; finfo_nbr++) {
        finfo = self->index [finfo_nbr];
        ipr_finfo_print (finfo, buffer);
        icl_console_print (buffer);
    }
</method>

<method name = "create" return = "rc">
    <doc>
    Create a new directory.  Returns 0 if the directory was created;
    -1 if there was an error.  Under Windows and OpenVMS, accepts directory
    names with '/'.  Will create multiple levels of directory if required.
    </doc>
    <argument name = "path to create" type = "char *">Directory path to create</argument>
    <declare name = "rc" type = "Bool" default = "0" />
    <local>
    icl_shortstr_t
        path;
#   if (defined (GATES_FILESYSTEM))
    char
        *char_ptr;
#   endif
    char
        *slash;
    </local>

    //  Get working copy so we can mangle it a little
    icl_shortstr_cpy (path, path_to_create);
#   if (defined (GATES_FILESYSTEM))
    for (char_ptr = path; *char_ptr; char_ptr++)
        if (*char_ptr == '/')
            *char_ptr = '\\\\';

    //  Handle \\system\drive specially
    if (path [0] == '\\\\' && path [1] == '\\\\') {
        slash = strchr (path + 2, '\\\\');
        if (slash)
            slash = strchr (slash + 1, '\\\\');
    }
    else
#   endif
    slash = strchr (path + 1, PATHEND);

    //  Create each component of directory as required
    FOREVER {                           //  Create any parent directories
        if (slash)
            *slash = '\\0';             //  Cut at slash
        if (!ipr_file_is_directory (path)) {
#   if (defined (__UNIX__) || defined (__VMS_XOPEN) || defined (__OS2__))
            rc = mkdir (path, 0775);    //  User RWE Group RWE World RE

#   elif (defined (WIN32))
            if (CreateDirectory (path, NULL))
                rc = 0;
            else
                rc = -1;
#   elif (defined (GATES_FILESYSTEM))
            rc = mkdir (path);          //  Protection?  What's that?
#   else
            rc = -1;                    //  Not a known system
#   endif
            if (rc)                     //  End if error
                break;
        }
        if (slash == NULL)              //  End if last directory
            break;
        *slash = PATHEND;               //  Restore path name
        slash = strchr (slash + 1, PATHEND);
    }
</method>

<method name = "remove" return = "rc">
    <doc>
    Remove a directory.  Returns 0 if the directory could be
    removed; -1 if there was an error.  Under MS-DOS and OpenVMS accepts
    a directory name in UNIX format, i.e. containing '/' delimiters.  The
    directory does not need to be empty to be removed; any child directories
    and contained files will be deleted.
    </doc>
    <argument name = "path" type = "char *">Directory path to remove</argument>
    <declare name = "rc" type = "Bool" default = "0" />
    <local>
    ipr_dir_t
        *dir;                           //  Get list of contained files
    uint
        finfo_nbr;
    ipr_finfo_t
        *finfo;
    icl_shortstr_t
        path_filename;                  //  Path/filename
#   if (defined (GATES_FILESYSTEM))
    icl_shortstr_t
        copy_path;
    char
        *char_ptr;
#   endif
    </local>
    //
    //  Delete all child directories and contained files
    dir = ipr_dir_new (path, 'n');
    assert (dir->index);
    for (finfo_nbr = 0; finfo_nbr < dir->index_size; finfo_nbr++) {
        finfo = dir->index [finfo_nbr];
        icl_shortstr_fmt (path_filename, "%s/%s", path, finfo->name);
        if (finfo->filetype == APR_DIR) {
            if (*finfo->name != '.')
                ipr_dir_remove (path_filename);
        }
        else
            ipr_file_delete (path_filename);
    }
    ipr_dir_destroy (&dir);

#   if (defined (__UNIX__) || defined (__VMS_XOPEN) || defined (__OS2__))
    if (ipr_file_is_directory (path))
        rc = rmdir (path);
    else
        rc = -1;

#   elif (defined (GATES_FILESYSTEM))
    //  Check that directory exists
    if (ipr_file_is_directory (path)) {
        icl_shortstr_cpy (copy_path, path);
        for (char_ptr = copy_path; *char_ptr; char_ptr++)
            if (*char_ptr == '/')
                *char_ptr = '\\\\';
#   if (defined (WIN32))
        if (RemoveDirectory (copy_path))
            rc = 0;
        else
            rc = -1;
#   else
        rc = rmdir (copy_path);
#   endif
    }
    else
        rc = -1;
#   else
    rc = -1;                            //  Unsupported platform
#   endif
</method>

<private name = "header">
static int
    s_sort_by_name       (const void *info1, const void *info2);
static int
    s_sort_by_time       (const void *info1, const void *info2);
static int
    s_sort_by_time_desc  (const void *info1, const void *info2);
static int
    s_sort_by_size       (const void *info1, const void *info2);
static int
    s_sort_by_size_desc  (const void *info1, const void *info2);
</private>

<private name = "footer">
//  Sort by name
static int
s_sort_by_name (const void *info1, const void *info2)
{
    int
        rc;
    ipr_finfo_t
        *finfo1 = *(ipr_finfo_t **) info1,
        *finfo2 = *(ipr_finfo_t **) info2;

#if defined (__WINDOWS__)
    rc = ipr_str_lexcmp (finfo1->name, finfo2->name);
#else
    rc = strcmp (finfo1->name, finfo2->name);
#endif
    return (rc);
}

//  Sort by date/time then name
static int
s_sort_by_time (const void *info1, const void *info2)
{
    int
        rc;
    ipr_finfo_t
        *finfo1 = *(ipr_finfo_t **) info1,
        *finfo2 = *(ipr_finfo_t **) info2;

    if (finfo1->mtime < finfo2->mtime)
        rc = -1;
    else
    if (finfo1->mtime > finfo2->mtime)
        rc = 1;
    else
#if defined (__WINDOWS__)
        rc = ipr_str_lexcmp (finfo1->name, finfo2->name);
#else
        rc = strcmp (finfo1->name, finfo2->name);
#endif
    return (rc);
}

//  Sort by descending date/time, then name
static int
s_sort_by_time_desc (const void *info1, const void *info2)
{
    int
        rc;
    ipr_finfo_t
        *finfo1 = *(ipr_finfo_t **) info1,
        *finfo2 = *(ipr_finfo_t **) info2;

    if (finfo1->mtime < finfo2->mtime)
        rc = 1;
    else
    if (finfo1->mtime > finfo2->mtime)
        rc = -1;
    else
#if defined (__WINDOWS__)
        rc = ipr_str_lexcmp (finfo1->name, finfo2->name);
#else
        rc = strcmp (finfo1->name, finfo2->name);
#endif
    return (rc);
}

//  Sort by size then name
static int
s_sort_by_size (const void *info1, const void *info2)
{
    int
        rc;
    ipr_finfo_t
        *finfo1 = *(ipr_finfo_t **) info1,
        *finfo2 = *(ipr_finfo_t **) info2;

    if (finfo1->size < finfo2->size)
        rc = -1;
    else
    if (finfo1->size > finfo2->size)
        rc = 1;
    else
#if defined (__WINDOWS__)
        rc = ipr_str_lexcmp (finfo1->name, finfo2->name);
#else
        rc = strcmp (finfo1->name, finfo2->name);
#endif
    return (rc);
}

//  Sort by descending size, then name
static int
s_sort_by_size_desc (const void *info1, const void *info2)
{
    int
        rc;
    ipr_finfo_t
        *finfo1 = *(ipr_finfo_t **) info1,
        *finfo2 = *(ipr_finfo_t **) info2;

    if (finfo1->size < finfo2->size)
        rc = 1;
    else
    if (finfo1->size > finfo2->size)
        rc = -1;
    else
#if defined (__WINDOWS__)
        rc = ipr_str_lexcmp (finfo1->name, finfo2->name);
#else
        rc = strcmp (finfo1->name, finfo2->name);
#endif
    return (rc);
}
</private>

<method name = "selftest">
    <local>
    ipr_dir_t
        *dir;
    </local>
    //
    dir = ipr_dir_new (".", 'n');
    ipr_dir_resort (dir, 't');
    ipr_dir_resort (dir, 'T');
    ipr_dir_resort (dir, 's');
    ipr_dir_resort (dir, 'S');
    ipr_dir_dump (dir);
    ipr_dir_destroy (&dir);

    assert (ipr_dir_create ("level1/level2/level3") == 0);
    assert (ipr_file_is_directory ("level1/level2/level3"));
    assert (ipr_dir_remove ("level1") == 0);
</method>

</class>
