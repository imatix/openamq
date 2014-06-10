/*---------------------------------------------------------------------------
    ipr_path.c - ipr_path component

Provides a general set of path manipulation functions.
    Generated from ipr_path.icl by icl_gen using GSL/4.
    
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
#include "ipr_path.h"                   //  Definitions for our class

//  Shorthands for class methods

#define self_create         ipr_path_create
#define self_remove         ipr_path_remove
#define self_resolve        ipr_path_resolve
#define self_selftest       ipr_path_selftest
#define self_show_animation  ipr_path_show_animation

Bool
    ipr_path_animating = TRUE;          //  Animation enabled by default
static Bool s_path_delimiter (char delim);
/*  -------------------------------------------------------------------------
    ipr_path_create

    Type: Component method
    Create a new directory path.  Returns 0 if the directory was created;
    -1 if there was an error.  Under Windows and OpenVMS, accepts directory
    names with '/'.  Will create multiple levels of directory if required.
    -------------------------------------------------------------------------
 */

Bool
    ipr_path_create (
    char * path_to_create               //  Path to create
)
{
    icl_shortstr_t
        path;
#   if (defined (__WINDOWS__))
    char
        *char_ptr;
#   endif
    char
        *slash;
    Bool
        rc = 0;                         //  Not documented

    //
    //  Get working copy so we can mangle it a little                          
    icl_shortstr_cpy (path, path_to_create);
#   if (defined (__WINDOWS__))
    for (char_ptr = path; *char_ptr; char_ptr++)
        if (*char_ptr == '/')
            *char_ptr = '\\';

    //  Handle \systemdrive specially
    if (path [0] == '\\' && path [1] == '\\') {
        slash = strchr (path + 2, '\\');
        if (slash)
            slash = strchr (slash + 1, '\\');
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

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_path_remove

    Type: Component method
    Remove a directory path.  Returns 0 if the directory could be
    removed; -1 if there was an error.  Under Windows and OpenVMS accepts
    a directory name in UNIX format, i.e. containing '/' delimiters.  The
    directory must be empty to be removed.
    -------------------------------------------------------------------------
 */

Bool
    ipr_path_remove (
    char * path                         //  Directory path to remove
)
{
    Bool
        rc = 0;                         //  Not documented

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
                *char_ptr = '\\';

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

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_path_resolve

    Type: Component method
    Accepts a path consisting of zero or more directory names and
    optionally a filename plus extension. Removes '.' and '..' if they occur
    in the path. '..' is resolved by also removing the preceding directory
    name, if any.  Returns a freshly-allocated string containing the
    resulting path.  The caller must free this string using icl_mem_free() when
    finished with it.  The returned path may be empty.  Under OS/2 and DOS,
    treats '\' and '/' both as directory separators. A '..' directory at the
    start of the path resolves into nothing. If the input path started with
    '/', the returned path also does, else it does not.  For compatibility
    with DOS-based systems, '...' is treated as '../..', '....' as
    '../../..', and so on.
    -------------------------------------------------------------------------
 */

char *
    ipr_path_resolve (
    char * old_path                     //  Directory path to resolve
)
{
    char *
        new_path;                       //  Not documented

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

    return (new_path);
}
/*  -------------------------------------------------------------------------
    ipr_path_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_path_selftest (
void)
{
char
    *path;

assert (ipr_path_create ("level1/level2/level3") == 0);
assert (ipr_file_is_directory ("level1/level2/level3"));
assert (ipr_path_remove ("level1/level2/level3") == 0);
assert (ipr_path_remove ("level1/level2") == 0);
assert (ipr_path_remove ("level1") == 0);

path = ipr_path_resolve ("../this/is/not/../valid/");
assert (streq (path, "this/is/valid/"));
icl_mem_free (path);
}
/*  -------------------------------------------------------------------------
    ipr_path_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_path_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_path_animating = enabled;
}
static Bool
s_path_delimiter (char delim)
{
#if (defined (__WINDOWS__))
    if (delim == '\\' || delim == '/' || delim == 0)
        return (TRUE);
    else
#elif (defined (__UNIX__) || defined (__VMS__))
    if (delim == '/' || delim == 0)
        return (TRUE);
    else
#endif
        return (FALSE);
}

//  Embed the version information in the resulting binary

char *EMBED__ipr_path_version_start       = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_path_component           = "ipr_path ";
char *EMBED__ipr_path_version             = "1.0 ";
char *EMBED__ipr_path_copyright           = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_path_filename            = "ipr_path.icl ";
char *EMBED__ipr_path_builddate           = "2011/03/01 ";
char *EMBED__ipr_path_version_end         = "VeRsIoNeNd:ipc";

