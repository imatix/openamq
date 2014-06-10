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
    name      = "ipr_path"
    comment   = "Path and file management"
    version   = "1.0"
    script    = "icl_gen"
    >

<doc>
Provides a general set of path manipulation functions.
</doc>

<inherit class = "icl_base" />

<import class = "ipr_file" />
<import class = "ipr_str" />

<method name = "create" return = "rc">
    <doc>
    Create a new directory path.  Returns 0 if the directory was created;
    -1 if there was an error.  Under Windows and OpenVMS, accepts directory
    names with '/'.  Will create multiple levels of directory if required.
    </doc>
    <argument name = "path to create" type = "char *">Path to create</argument>
    <declare name = "rc" type = "Bool" default = "0" />
    <local>
    icl_shortstr_t
        path;
#   if (defined (__WINDOWS__))
    char
        *char_ptr;
#   endif
    char
        *slash;
    </local>
    //
    //  Get working copy so we can mangle it a little                          
    icl_shortstr_cpy (path, path_to_create);
#   if (defined (__WINDOWS__))
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
            *slash = 0;                 //  Cut at slash                       
        if (!ipr_file_is_directory (path)) {
#   if (defined (__UNIX__) || defined (__VMS_XOPEN) || defined (__OS2__))
            rc = mkdir (path, 0775);    //  User RWE Group RWE World RE        

#   elif (defined (WIN32))
            if (CreateDirectory (path, NULL))
                rc = 0;
            else
                rc = -1;
#   elif (defined (__WINDOWS__))
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
    Remove a directory path.  Returns 0 if the directory could be
    removed; -1 if there was an error.  Under Windows and OpenVMS accepts
    a directory name in UNIX format, i.e. containing '/' delimiters.  The
    directory must be empty to be removed.
    </doc>
    <argument name = "path" type = "char *">Directory path to remove</argument>
    <declare name = "rc" type = "Bool" default = "0" />
    //
#   if (defined (__UNIX__) || defined (__VMS_XOPEN) || defined (__OS2__))
    if (ipr_file_is_directory (path))
        rc = rmdir (path);
    else
        rc = -1;

#   elif (defined (__WINDOWS__))
    icl_shortstr_t
        copy_path;
    char
        *char_ptr;

    //  Check that directory exists                                            
    if (ipr_file_is_directory (path)) {
        icl_shortstr_cpy (copy_path, path);
        for (char_ptr = copy_path; *char_ptr; char_ptr++)
            if (*char_ptr == '/')
                *char_ptr = '\\\\';

        if (RemoveDirectory (copy_path))
            rc = 0;
        else
            rc = -1;
    }
    else
        rc = -1;
#   else
    rc = -1;                            //  Unsupported platform               
#   endif
</method>

<method name = "resolve" return = "new_path">
    <doc>
    Accepts a path consisting of zero or more directory names and
    optionally a filename plus extension. Removes '.' and '..' if they occur
    in the path. '..' is resolved by also removing the preceding directory
    name, if any.  Returns a freshly-allocated string containing the
    resulting path.  The caller must free this string using icl_mem_free() when
    finished with it.  The returned path may be empty.  Under OS/2 and DOS,
    treats '\\' and '/' both as directory separators. A '..' directory at the
    start of the path resolves into nothing. If the input path started with
    '/', the returned path also does, else it does not.  For compatibility
    with DOS-based systems, '...' is treated as '../..', '....' as
    '../../..', and so on.
    </doc>
    <argument name = "old_path" type = "char *">Directory path to resolve</argument>
    <declare name = "new_path" type = "char *" />
#   if (defined (__UNIX__) || defined (__WINDOWS__) || defined (__VMS__))
    char
        *new_ptr,                       //  Pointer into new_path            
        last_char = '/';                //  Start of path counts as delim    
    int
        nbr_dots;                       //  Size of '..', '...' specifier    

    assert (old_path);

    new_path = icl_mem_strdup (old_path);
    new_ptr  = new_path;
    while (*old_path) {
        if (s_path_delimiter (last_char) && *old_path == '.') {
            //  Handle one or more dots followed by a path delimiter         
            nbr_dots = 0;               //  Count number of dots             
            while (old_path [nbr_dots] == '.')
                nbr_dots++;

            if (s_path_delimiter (old_path [nbr_dots])) {
                old_path += nbr_dots;   //  Skip past dots                   
                if (*old_path)
                    old_path++;         //    and past / if any              

                //  Now backtrack in new path, dropping directories as       
                //  many times as needed (0 or more times)                   
                while (nbr_dots > 1) {
                    if (new_ptr > new_path + 1) {
                        new_ptr--;      //  Drop delimiter                   
                        while (new_ptr > new_path) {
                            if (s_path_delimiter (*(new_ptr - 1)))
                                break;  //    and end after delimiter        
                            new_ptr--;
                        }
                    }
                    else
                        break;          //  At start of name - finish        
                    nbr_dots--;
                }
            }
            else
                //  Handle '.something'                                      
                last_char = *new_ptr++ = *old_path++;
        }
        else
            last_char = *new_ptr++ = *old_path++;
    }
    *new_ptr = 0;                       //  Terminate string properly
#else
    //  Path resolution not supported, so just copy path
    new_path = icl_mem_strdup (old_path);     
#endif
</method>

<private name = "header">
static Bool s_path_delimiter (char delim);
</private>

<private name = "footer">
static Bool
s_path_delimiter (char delim)
{
#if (defined (__WINDOWS__))
    if (delim == '\\\\' || delim == '/' || delim == 0)
        return (TRUE);
    else
#elif (defined (__UNIX__) || defined (__VMS__))
    if (delim == '/' || delim == 0)
        return (TRUE);
    else
#endif
        return (FALSE);
}
</private>

<method name = "selftest">
    <local>
    char
        *path;
    </local>
    assert (ipr_path_create ("level1/level2/level3") == 0);
    assert (ipr_file_is_directory ("level1/level2/level3"));
    assert (ipr_path_remove ("level1/level2/level3") == 0);
    assert (ipr_path_remove ("level1/level2") == 0);
    assert (ipr_path_remove ("level1") == 0);

    path = ipr_path_resolve ("../this/is/not/../valid/");
    assert (streq (path, "this/is/valid/"));
    icl_mem_free (path);
</method>

</class>
