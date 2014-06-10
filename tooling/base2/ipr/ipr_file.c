/*---------------------------------------------------------------------------
    ipr_file.c - ipr_file component

Provides a general set of file and filename manipulation functions that
return information about a file specified by name. The maximum length of
a filename is set at 255 characters, to fit in an icl_shortstr_t. All
filenames should be defined using this type.
    Generated from ipr_file.icl by icl_gen using GSL/4.
    
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
#include "ipr_file.h"                   //  Definitions for our class

//  Shorthands for class methods

#define self_exists         ipr_file_exists
#define self_size           ipr_file_size
#define self_time           ipr_file_time
#define self_lines          ipr_file_lines
#define self_inode          ipr_file_inode
#define self_slurp          ipr_file_slurp
#define self_where          ipr_file_where
#define self_delete         ipr_file_delete
#define self_rename         ipr_file_rename
#define self_truncate       ipr_file_truncate
#define self_strip_path     ipr_file_strip_path
#define self_set_path       ipr_file_set_path
#define self_set_extension  ipr_file_set_extension
#define self_move           ipr_file_move
#define self_is_readable    ipr_file_is_readable
#define self_is_writeable   ipr_file_is_writeable
#define self_is_directory   ipr_file_is_directory
#define self_get_info       ipr_file_get_info
#define self_digest         ipr_file_digest
#define self_tmpfile        ipr_file_tmpfile
#define self_etag           ipr_file_etag
#define self_selftest       ipr_file_selftest
#define self_show_animation  ipr_file_show_animation

Bool
    ipr_file_animating = TRUE;          //  Animation enabled by default
static dbyte s_file_mode        (const char *filename);
#   if (defined (__WINDOWS__))
static Bool s_system_devicename (const char *filename);
static Bool s_dos_exe_file      (const char *filename);
#   endif
/*  -------------------------------------------------------------------------
    ipr_file_exists

    Type: Component method
    Returns TRUE if the specified file exists, else FALSE.
    -------------------------------------------------------------------------
 */

Bool
    ipr_file_exists (
    char * filename                     //  File to examine
)
{
    Bool
        rc = 0;                         //  Not documented

//
assert (filename);
rc = s_file_mode (filename) > 0;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_file_size

    Type: Component method
    Returns the size, in bytes, of the specified file or directory.
    The size of a directory is not a portable concept. If there is an error,
    returns 0.
    -------------------------------------------------------------------------
 */

int64_t
    ipr_file_size (
    char * filename                     //  File to measure
)
{
ipr_finfo_t
    *finfo = NULL;
    int64_t
        size = 0;                       //  Not documented

    //
    assert (filename);
#   if (defined (__WINDOWS__))
    if (!s_system_devicename (filename))
#   endif
    finfo = ipr_file_get_info (filename);
    if (finfo) {
        size = finfo->size;
        ipr_finfo_destroy (&finfo);
    }

    return (size);
}
/*  -------------------------------------------------------------------------
    ipr_file_time

    Type: Component method
    Returns the modification time of the file, or zero if the file
    is not present or readable.  Returns an apr_time_t in microseconds,
    compatible with APR.
    -------------------------------------------------------------------------
 */

apr_time_t
    ipr_file_time (
    char * filename                     //  File to check
)
{
ipr_finfo_t
    *finfo = NULL;
    apr_time_t
        mtime = 0;                      //  Not documented

    //
    assert (filename);
#   if (defined (__WINDOWS__))
    if (!s_system_devicename (filename))
#   endif
    finfo = ipr_file_get_info (filename);
    if (finfo) {
        mtime = finfo->mtime;
        ipr_finfo_destroy (&finfo);
    }

    return (mtime);
}
/*  -------------------------------------------------------------------------
    ipr_file_lines

    Type: Component method
    Returns the size of the file, in lines, or zero if the file
    is not present or readable.  Reads the entire file, and will
    be slow on large files.  Lines of text that do not end in a
    newline will not be counted.
    -------------------------------------------------------------------------
 */

size_t
    ipr_file_lines (
    char * filename                     //  File to check
)
{
FILE
    *file_stream;
int
    ch;
    size_t
        rc = 0;                         //  Not documented

//
assert (filename);
file_stream = fopen (filename, "r");
if (file_stream) {
    while ((ch = fgetc (file_stream)) != EOF)
        if (ch == '\n')
            rc++;
    fclose (file_stream);
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_file_inode

    Type: Component method
    Returns the inode of the file, or zero if the file is not present or
    readable.  Returns an apr_ino_t compatible with APR.
    -------------------------------------------------------------------------
 */

apr_ino_t
    ipr_file_inode (
    char * filename                     //  File to check
)
{
ipr_finfo_t
    *finfo = NULL;
    apr_ino_t
        inode = 0;                      //  Not documented

    //
    assert (filename);
#   if (defined (__WINDOWS__))
    if (!s_system_devicename (filename))
#   endif
    finfo = ipr_file_get_info (filename);
    if (finfo) {
        inode = finfo->inode;
        ipr_finfo_destroy (&finfo);
    }

    return (inode);
}
/*  -------------------------------------------------------------------------
    ipr_file_slurp

    Type: Component method
    Reads an entire file, and returns a bucket containing the file
    data.  The file is read as binary data on systems that make such
    distinctions.  Returns NULL if the file cannot be found.  The
    current implementation returns at most one bucket of data from
    the file.
    -------------------------------------------------------------------------
 */

ipr_bucket_t *
    ipr_file_slurp (
    char * filename                     //  File to read
)
{
size_t
    file_size;
FILE
    *file_stream = NULL;
    ipr_bucket_t *
        bucket = NULL;                  //  Not documented

//
assert (filename);
//  Measure amount of data to read
file_size = (size_t) ipr_file_size (filename);
if (file_size >= 0) {
    file_stream = fopen (filename, "rb");
    if (file_size > IPR_BUCKET_MAX_SIZE)
        file_size = IPR_BUCKET_MAX_SIZE;
}
//  Read the data into the bucket
if (file_stream) {
    bucket = ipr_bucket_new (file_size);
    bucket->cur_size = file_size;
    if (fread (bucket->data, (size_t) file_size, 1, file_stream) != 1)
        ipr_bucket_destroy (&bucket);
    fclose (file_stream);
}

    return (bucket);
}
/*  -------------------------------------------------------------------------
    ipr_file_where

    Type: Component method
    Scans a user-specified path symbol for a specific file, and returns a
    fully-specified filename.  If the path is null or empty, it is ignored.
    Otherwise, the path is translated as an environment variable, and cut
    into a list of directory names.  The path is cut up as follows:

        Unix      directory names separated by ':'. :: means current.
        win32     directory names separated by ';'. ;; means current.
        OS/2      directory names separated by ';'. ;; means current.
        VMS       directory names separated by ','. " ", means current.
        Other     single directory name.

    Always searches the current directory before considering the path value.
    When the path cannot be translated, and is not null or empty, it is used
    as a literal value.

    All parameters are case-sensitive; the precise effect of this depends on
    the system.  On win32, filenames are always folded to uppercase, but the
    path must be supplied in uppercase correctly.  On UNIX, all parameters are
    case sensitive.  On VMS, path and filenames are folded into uppercase.

    Under VMS, all filenames are handled in POSIX mode, i.e. /disk/path/file
    instead of $disk:[path]file.

    Returns 0 in case of success, -1 in case the file was not found.
    -------------------------------------------------------------------------
 */

int
    ipr_file_where (
    char * filename,                    //  File to locate
    char * path,                        //  Path to use
    char * fullname                     //  Resulting name
)
{
char
    *pathptr,                       //  End of directory in path
    *charptr;
icl_shortstr_t
    workname;
int
    length;                         //  Length of directory name
    int
        rc = 0;                         //  Return code

//
//  Translate path into list of directories
if (path && *path) {
    pathptr = getenv (path);
    if (pathptr == NULL)
        pathptr = path;             //  If not found, use literally
    for (charptr = pathptr; *charptr; charptr++)
        if (*charptr == '\\')
            *charptr = '/';
}
else
    pathptr = NULL;

//  If file exists as defined, prefix with current directory if not an
//  absolute filename, then return the resulting filename
if (strchr (filename, '/')) {
    if (ipr_file_exists (filename))
        icl_shortstr_cpy (fullname, filename);
    else
        rc = -1;                    //  Assume we don't find the file
}
else
if (ipr_file_exists (filename))
    icl_shortstr_fmt (fullname, "./%s", filename);
else {
    rc = -1;                        //  Assume we don't find the file
    while (pathptr && *pathptr) {
        length = strcspn (pathptr, PATHSEP);
        strncpy (workname, pathptr, length);
        pathptr += length;
        if (*pathptr)
            pathptr++;

        //  Add path-to-filename delimiter
        if (length && workname [length - 1] != '/')
            workname [length++] = '/';

        workname [length] = 0;
        icl_shortstr_cat (workname, filename);
        if (ipr_file_exists (workname)) {
            icl_shortstr_cpy (fullname, workname);
            rc = 0;
            break;
        }
    }
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_file_delete

    Type: Component method
    Delete specified file, return 0 if ok, else -1.
    -------------------------------------------------------------------------
 */

int
    ipr_file_delete (
    char * filename                     //  File to delete
)
{
    int
        rc = 0;                         //  Not documented

    //
    assert (filename);
#   if (defined (__WINDOWS__))
    if (s_system_devicename (filename))
        rc = -1;                        //  Not allowed on device names
    else {
        rc = !DeleteFile (filename);
        if (rc && errno == EACCES) {
            //  Under WinNT and Win95, a delete of a freshly-created file can
            //  sometimes fail with a permission error which passes after a
            //  short delay.  Ugly but it seems to work.
            Sleep (100);                //  Win32 milliseconds
            rc = !DeleteFile (filename);
        }
    }
#   elif (defined (__VMS__))
    rc = remove (filename);
#   else
    rc = unlink (filename);
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_file_rename

    Type: Component method
    Rename file as specified, return 0 if ok, else -1.  Will move files
    between directories.  If destination file already exists, it will be
    deleted.
    -------------------------------------------------------------------------
 */

int
    ipr_file_rename (
    char * newname,                     //  Rename file to this
    char * oldname                      //  Actual name of file
)
{
#   if (defined (__WINDOWS__))
    char
        *dos_newname;
#   endif
    int
        rc = 0;                         //  Not documented

    //
    assert (oldname);
    assert (newname);
#   if (defined (__WINDOWS__))
    if (s_system_devicename (oldname)
    ||  s_system_devicename (newname))
        rc = -1;                        //  Not allowed on device names
    else {
        dos_newname = icl_mem_strdup (newname);
        ipr_str_subch (dos_newname, '/', '\\');
        DeleteFile (dos_newname);
        rc = rename (oldname, dos_newname);
        if (rc && errno == EACCES) {
            /*  Under WinNT and Win95, a rename of a freshly-created file can
             *  sometimes fail with a permission error which passes after a
             *  short delay.  Ugly but it seems to work.
             */
            Sleep (200);
            rc = rename (oldname, dos_newname);
        }
        icl_mem_free (dos_newname);
    }
#   else
    rc = rename (oldname, newname);
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_file_truncate

    Type: Component method
    Truncate a file: creates or empties the file.
    -------------------------------------------------------------------------
 */

void
    ipr_file_truncate (
    char * filename                     //  File to truncate
)
{
FILE
    *file_stream = NULL;

//
assert (filename);
file_stream = fopen (filename, "wb");
fclose (file_stream);
}
/*  -------------------------------------------------------------------------
    ipr_file_strip_path

    Type: Component method
    Removes the leading path, if any, from the filename.  Returns
    the filename.  The filename can be specified using the local
    operating system syntax.
    -------------------------------------------------------------------------
 */

char *
    ipr_file_strip_path (
    char * filename                     //  Filename to process
)
{
char
    *path_end;

    //
    assert (filename);
    path_end = strrchr (filename, '/');
#   if (defined (__WINDOWS__))
    if (path_end == NULL)
        path_end = strrchr (filename, '\\');
#   endif
    if (path_end)
        memmove (filename, path_end + 1, strlen (path_end));

    return (filename);
}
/*  -------------------------------------------------------------------------
    ipr_file_set_path

    Type: Component method
    Prefixes the filename with the specified path.  Strips any
    existing path from the filename.  Returns filename.
    -------------------------------------------------------------------------
 */

char *
    ipr_file_set_path (
    char * filename,                    //  Filename to process
    char * path                         //  Desired path
)
{
icl_shortstr_t
    formatted;

    //
    assert (filename);
    assert (path);

    ipr_file_strip_path (filename);
    icl_shortstr_cpy (formatted, path);
#   if (defined (__WINDOWS__))
    if (strlast (path) != '/' && strlast (path) != '\\')
#   else
    if (strlast (path) != '/')
#   endif
        icl_shortstr_cat (formatted, "/");

    icl_shortstr_cat (formatted, filename);
    icl_shortstr_cpy (filename, formatted);

    return (filename);
}
/*  -------------------------------------------------------------------------
    ipr_file_set_extension

    Type: Component method
    Adds extension to filename if it does not already have an
    extension.  Returns filename.
    -------------------------------------------------------------------------
 */

char *
    ipr_file_set_extension (
    char * filename,                    //  Filename to process
    char * extension                    //  Extension to add
)
{
char
    *terminal;

    assert (filename);
    assert (extension);

    for (terminal = filename + strlen (filename); terminal > filename; terminal--)
#   if (defined (__WINDOWS__))
        if (*terminal == '.' || *terminal == '/' || *terminal == '\\')
            break;
#   else
        if (*terminal == '.' || *terminal == '/')
            break;
#   endif
    if (*terminal != '.') {
        if (*extension == '.')
            extension++;
        icl_shortstr_cat (terminal, ".");
        icl_shortstr_cat (terminal, extension);
    }

    return (filename);
}
/*  -------------------------------------------------------------------------
    ipr_file_move

    Type: Component method
    Moves a file to a specific directory.
    -------------------------------------------------------------------------
 */

char *
    ipr_file_move (
    char * directory,                   //  Desired directory path
    char * filename                     //  Filename to process
)
{
icl_shortstr_t
    destination;

//
assert (directory);
assert (filename);

icl_shortstr_cpy  (destination, filename);
ipr_file_set_path (destination, directory);
ipr_file_rename   (destination, filename);

    return (filename);
}
/*  -------------------------------------------------------------------------
    ipr_file_is_readable

    Type: Component method
    Returns TRUE if the current process can read the specified
    file or directory.  The filename may end in a slash (/ or ) only if
    it is a directory.
    -------------------------------------------------------------------------
 */

Bool
    ipr_file_is_readable (
    char * filename                     //  File to examine
)
{
    Bool
        rc = 0;                         //  Not documented

//
assert (filename);
if (ipr_file_is_directory (filename))
    rc = ((s_file_mode (filename) & S_IREAD) != 0);
else
if (strlast (filename) == '/')
    rc = FALSE;
else
    rc = ((s_file_mode (filename) & S_IREAD) != 0);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_file_is_writeable

    Type: Component method
    Returns TRUE if the current process can write the specified file or
    directory.  The filename may end in a slash (/ or ) only if it is
    a directory.
    -------------------------------------------------------------------------
 */

Bool
    ipr_file_is_writeable (
    char * filename                     //  File to examine
)
{
    Bool
        rc = 0;                         //  Not documented

//
assert (filename);
if (ipr_file_is_directory (filename))
    rc = ((s_file_mode (filename) & S_IWRITE) != 0);
else
if (strlast (filename) == '/')
    rc = FALSE;
else
    rc = ((s_file_mode (filename) & S_IWRITE) != 0);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_file_is_directory

    Type: Component method
    Returns TRUE if the specified file is a directory.  The
    filename may end in a slash (/ or ).   Under MS-DOS/OS2/Windows, a
    directory name may consist solely of a disk-drive specifier.  Under
    VMS the directory may optionally take the extension '.dir'.
    -------------------------------------------------------------------------
 */

Bool
    ipr_file_is_directory (
    char * filename                     //  File to examine
)
{
icl_shortstr_t
    dir_name;
    Bool
        rc = 0;                         //  Not documented

    assert (filename);

    //TODO when ipr_dir_clean_path is implemented
    //dir_name = mem_strdup (clean_path (filename));
    icl_shortstr_cpy (dir_name, filename);
#   if (defined (__VMS__))
    if (!ipr_file_exists (dir_name))
        ipr_file_set_extension (dir_name, "dir");
#   endif
    rc = (s_file_mode (dir_name) & S_IFDIR) != 0;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_file_get_info

    Type: Component method
    Returns a finfo block containing file information in APR-compatible
    formats.  Returns NULL if the file was not readable.
    -------------------------------------------------------------------------
 */

ipr_finfo_t *
    ipr_file_get_info (
    char * filename                     //  File to examine
)
{
apr_pool_t
    *pool;                          //  Pool for all allocations
apr_file_t
    *handle;
apr_finfo_t
    apr_finfo;
    ipr_finfo_t *
        finfo = NULL;                   //  Not documented

//
assert (filename);
apr_pool_create (&pool, NULL);
if (!apr_file_open (&handle, filename, APR_READ, APR_OS_DEFAULT, pool)) {
    memset (&apr_finfo, 0, sizeof (apr_finfo_t));
    apr_file_info_get (&apr_finfo, APR_FINFO_NORM, handle);
    finfo = ipr_finfo_new (&apr_finfo);
    apr_file_close (handle);
}
apr_pool_destroy (pool);

    return (finfo);
}
/*  -------------------------------------------------------------------------
    ipr_file_digest

    Type: Component method
    Calculates the digest (currently SHA1) of a file, and returns this
    as a printable hex string.  The returned string must be freed by
    the caller.  If the file could not be read, returns NULL.
    -------------------------------------------------------------------------
 */

char *
    ipr_file_digest (
    char * filename                     //  File to digest
)
{
FILE
    *file_stream;                   //  File stream to digest
ipr_bucket_t
    *bucket;                        //  Bucket of data from file
apr_sha1_ctx_t
    context;
apr_byte_t
    bin_digest [APR_SHA1_DIGESTSIZE];
    char *
        digest = NULL;                  //  Not documented

    //
    assert (filename);
#   define DIGEST_BLOCK 1000000         //  Read 1MB at a time from file
    file_stream = fopen (filename, "rb");
    if (file_stream) {
        apr_sha1_init (&context);
        bucket = ipr_bucket_new (DIGEST_BLOCK);
        while ((bucket->cur_size = fread (bucket->data, 1, DIGEST_BLOCK, file_stream)) > 0)
            apr_sha1_update (&context, (char *) bucket->data, bucket->cur_size);
        fclose (file_stream);
        ipr_bucket_destroy (&bucket);

        apr_sha1_final (bin_digest, &context);
        digest = icl_mem_alloc (ICL_SHORTSTR_MAX);
        apr_base64_encode (digest, (char *) bin_digest, APR_MD5_DIGESTSIZE);
    }

    return (digest);
}
/*  -------------------------------------------------------------------------
    ipr_file_tmpfile

    Type: Component method
    Generates a temporary file name.  The caller must free the returned name
    when finished with it.
    -------------------------------------------------------------------------
 */

char *
    ipr_file_tmpfile (
void)
{
    char *
        filename;                       //  Filename to generate

//
filename = icl_mem_alloc (4 + 10 + 1);      //  tmp_ + 10-digit random + 0
ipr_str_random (filename, "tmp_Ax10");
while (ipr_file_exists (filename))
    ipr_str_random (filename, "tmp_Ax10");

    return (filename);
}
/*  -------------------------------------------------------------------------
    ipr_file_etag

    Type: Component method
    Generates an entity tag (ETag) for the file.  The type argument can be
    'fixed', which generates an ETag of the file's modified time, size, and
    inode, or 'float', which generates an ETag of the file's modified time
    and size.  A floating ETag is file system independent, so gives the same
    ETag for the same file in different locations.  A fixed ETag includes the
    location (inode on the file system).  If the type is not 'fixed' or 'float'
    the resulting ETag is empty.
    -------------------------------------------------------------------------
 */

char *
    ipr_file_etag (
    char * filename,                    //  File to digest
    char * type                         //  ETag type: fixed, float
)
{
ipr_finfo_t
    *finfo = NULL;
    char *
        etag = NULL;                    //  ETag to generate

    //
    assert (filename);
#   if (defined (__WINDOWS__))
    if (!s_system_devicename (filename))
#   endif
    finfo = ipr_file_get_info (filename);
    if (finfo) {
        if (streq (type, "fixed"))
            etag = ipr_str_format ("%qx-%qx-%lx",
                (long long unsigned int) finfo->mtime,
                (long long unsigned int) finfo->size,
                finfo->inode);
        else
        if (streq (type, "float"))
            etag = ipr_str_format ("%qx-%qx",
                (long long unsigned int) finfo->mtime,
                (long long unsigned int) finfo->size);
        else
            etag = icl_mem_strdup ("");

        ipr_finfo_destroy (&finfo);
    }

    return (etag);
}
/*  -------------------------------------------------------------------------
    ipr_file_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_file_selftest (
void)
{
#   define TEST_FILE  "ipr_file.icl"
    ipr_bucket_t
        *bucket;
    icl_shortstr_t
        fullname;
    char
        *tmpname,
        *digest,
        *etag;
    int
        rc;
    FILE
        *file;

//  Some basic methods on files
assert (ipr_file_is_directory ("."));

//  Check that we can get the properties for a file
assert (ipr_file_lines (TEST_FILE));
assert (ipr_file_time  (TEST_FILE));
assert (ipr_file_size  (TEST_FILE));
assert (ipr_file_inode (TEST_FILE));

//  Check we can slurp the file
bucket = ipr_file_slurp (TEST_FILE);
assert (bucket);
assert (bucket->cur_size == (size_t) ipr_file_size (TEST_FILE));
ipr_bucket_destroy (&bucket);

//  Test the ipr_file_strip_path method
icl_shortstr_cpy (fullname, "/some/path/filename");
ipr_file_strip_path (fullname);
assert (streq (fullname, "filename"));

icl_shortstr_cpy (fullname, "/filename");
ipr_file_strip_path (fullname);
assert (streq (fullname, "filename"));

icl_shortstr_cpy (fullname, "filename");
ipr_file_strip_path (fullname);
assert (streq (fullname, "filename"));

icl_shortstr_cpy (fullname, "/");
ipr_file_strip_path (fullname);
assert (streq (fullname, ""));

//  Test the ipr_file_set_path method
icl_shortstr_cpy (fullname, "/some/path/filename");
ipr_file_set_path (fullname, "/new");
assert (streq (fullname, "/new/filename"));

icl_shortstr_cpy (fullname, "/filename");
ipr_file_set_path (fullname, "/new");
assert (streq (fullname, "/new/filename"));

icl_shortstr_cpy (fullname, "filename");
ipr_file_set_path (fullname, "/new");
assert (streq (fullname, "/new/filename"));

icl_shortstr_cpy (fullname, "/");
ipr_file_set_path (fullname, "/new");
assert (streq (fullname, "/new/"));

//  Test the ipr_file_set_extension method
icl_shortstr_cpy (fullname, "/some/path/filename");
ipr_file_set_extension (fullname, ".log");
assert (streq (fullname, "/some/path/filename.log"));

icl_shortstr_cpy (fullname, "/some/path.dir/filename");
ipr_file_set_extension (fullname, ".log");
assert (streq (fullname, "/some/path.dir/filename.log"));

icl_shortstr_cpy (fullname, "filename");
ipr_file_set_extension (fullname, ".log");
assert (streq (fullname, "filename.log"));

icl_shortstr_cpy (fullname, "/some/path/filename.zzz");
ipr_file_set_extension (fullname, ".log");
assert (streq (fullname, "/some/path/filename.zzz"));

icl_shortstr_cpy (fullname, "/some/path.dir/filename.zzz");
ipr_file_set_extension (fullname, ".log");
assert (streq (fullname, "/some/path.dir/filename.zzz"));

icl_shortstr_cpy (fullname, "filename.zzz");
ipr_file_set_extension (fullname, ".log");
assert (streq (fullname, "filename.zzz"));

//  Look for something well-known on the path
rc = ipr_file_where (TEST_FILE, "PATH", fullname);
assert (rc == 0);

file = fopen ("testdata1", "w");
assert (file);
fprintf (file, "test data 1\n");
fclose (file);
assert (ipr_file_lines ("testdata1") == 1);

file = fopen ("testdata2", "w");
assert (file);
fprintf (file, "test data 21\n");
fprintf (file, "test data 22\n");
fclose (file);
assert (ipr_file_lines ("testdata2") == 2);

ipr_file_rename ("testdata1", "testdata2");
assert (ipr_file_exists ("testdata1"));
assert (ipr_file_exists ("testdata2") == FALSE);
assert (ipr_file_lines ("testdata1") == 2);

ipr_file_delete ("testdata1");
assert (ipr_file_exists ("testdata2") == FALSE);

tmpname = ipr_file_tmpfile ();
assert (!ipr_file_exists (tmpname));
icl_mem_free (tmpname);

//  Digest method
digest = ipr_file_digest (fullname);
assert (digest);
icl_mem_free (digest);

//  ETag calculation
etag = ipr_file_etag (fullname, "fixed");
assert (strlen (etag) > 17);
icl_mem_free (etag);

etag = ipr_file_etag (fullname, "float");
assert (strlen (etag) > 15);
icl_mem_free (etag);

etag = ipr_file_etag (fullname, "other");
assert (strlen (etag) == 0);
icl_mem_free (etag);
}
/*  -------------------------------------------------------------------------
    ipr_file_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_file_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_file_animating = enabled;
}
/*  -------------------------------------------------------------------------
 *  s_file_mode
 *
 *  Returns the file mode for the specified file or directory name; returns
 *  0 if the specified file does not exist.
 */

static dbyte
s_file_mode (const char *filename)
{
#   if (defined (__WINDOWS__))
    DWORD
        dwfa;
    dbyte
        mode;

    assert (filename);
    if (s_system_devicename (filename))
        return (0);                     //  Not allowed on device names

    dwfa = GetFileAttributes (filename);
    if (dwfa == 0xffffffff)
        return (0);
    mode = 0;

    if (dwfa & FILE_ATTRIBUTE_DIRECTORY)
        mode |= S_IFDIR;
    else
        mode |= S_IFREG;
    if (!(dwfa & FILE_ATTRIBUTE_HIDDEN))
        mode |= S_IREAD;
    if (!(dwfa & FILE_ATTRIBUTE_READONLY))
        mode |= S_IWRITE;
    if (s_dos_exe_file (filename))
        mode |= S_IEXEC;
    return (mode);
#   else
    static struct stat
        stat_buf;

    assert (filename);
    if (strnull (filename))
        return (0);
    else
    if (stat ((char *) filename, &stat_buf) == 0)
        return ((dbyte) stat_buf.st_mode);
    else
        return (0);
#   endif
}

#   if (defined (__WINDOWS__))
static Bool
s_system_devicename (const char *supplied_filename)
{
    //  Under Win32 we can open the file, and ask what type it is; if it is
    //  not a disk file we reject it.  If we can't open it, we indicate that
    //  it is safe, so that the open will be retried and the correct error
    //  codes set for the caller.
    HANDLE
        fh;
    Bool
        is_devicefile = TRUE;

    fh = CreateFile (
        supplied_filename, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (fh != INVALID_HANDLE_VALUE) {
        if (GetFileType(fh) == FILE_TYPE_DISK)
            is_devicefile = FALSE;
        CloseHandle(fh);
    }
    else
        is_devicefile = FALSE;          // Doesn't exist

    return (is_devicefile);
}

/*  s_dos_exe_file -- internal
 *
 *  Returns TRUE if the file corresponds to the criteria for an executable
 *  file under Windows.
 */
static Bool
s_dos_exe_file (const char *filename)
{
    Bool
        executable;                     //  Return code
    FILE
        *stream;                        //  Opened file stream

    stream = fopen (filename, "r");
    if (stream) {
        executable = (fgetc (stream) == 'M' && fgetc (stream) == 'Z');
        fclose (stream);
    }
    else
        executable = FALSE;             //  File not found

    return (executable);
}
#   endif

//  Embed the version information in the resulting binary

char *EMBED__ipr_file_version_start       = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_file_component           = "ipr_file ";
char *EMBED__ipr_file_version             = "1.0 ";
char *EMBED__ipr_file_copyright           = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_file_filename            = "ipr_file.icl ";
char *EMBED__ipr_file_builddate           = "2011/03/01 ";
char *EMBED__ipr_file_version_end         = "VeRsIoNeNd:ipc";

