/*---------------------------------------------------------------------------
    ipr_uid.c - ipr_uid component

    Provides functions to access and maniuplate the current process
    user and group IDs.
    Generated from ipr_uid.icl by icl_gen using GSL/4.
    
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
#include "ipr_uid.h"                    //  Definitions for our class

//  Shorthands for class methods

#define self_user_name      ipr_uid_user_name
#define self_group_name     ipr_uid_group_name
#define self_set_real_user  ipr_uid_set_real_user
#define self_set_root_user  ipr_uid_set_root_user
#define self_set_real_group  ipr_uid_set_real_group
#define self_set_root_group  ipr_uid_set_root_group
#define self_get_uid        ipr_uid_get_uid
#define self_get_gid        ipr_uid_get_gid
#define self_run_as         ipr_uid_run_as
#define self_selftest       ipr_uid_selftest
#define self_show_animation  ipr_uid_show_animation

Bool
    ipr_uid_animating = TRUE;           //  Animation enabled by default
#define REAL_ID             0           //  Arguments for get_uid/get_gid
#define EFFECTIVE_ID        1
#if (defined (DOES_UID))
/*  -------------------------------------------------------------------------
    Function: get_uid -- internal

    Synopsis: Returns the real (REAL_ID) or effective (EFFECTIVE_ID) uid.
    These values are loaded the first time that the function is called: you
    should not rely on the effective uid after changing the uid.
    -------------------------------------------------------------------------*/

static uid_t
get_uid (int type)
{
    static int
        ruid = -1,
        euid = -1;

    if (ruid == -1)
        ruid = getuid ();
    if (euid == -1)
        euid = geteuid ();

    if (type == REAL_ID)
        return (ruid);
    else
    if (type == EFFECTIVE_ID)
        return (euid);
    else
        return (-1);
}

/*  -------------------------------------------------------------------------
    Function: get_gid -- internal

    Synopsis: Returns the real (REAL_ID) or effective (EFFECTIVE_ID) gid.
    These values are loaded the first time that the function is called: you
    should not rely on the effective gid after changing the gid.
    -------------------------------------------------------------------------*/

static gid_t
get_gid (int type)
{
    static int
        rgid = -1,
        egid = -1;

    if (rgid == -1)
        rgid = getgid ();
    if (egid == -1)
#   if (defined (__UTYPE_HPUX) || defined (__UTYPE_BEOS))
        egid = getgid ();
#   else
        egid = getegid ();
#   endif

    if (type == REAL_ID)
        return (rgid);
    else
    if (type == EFFECTIVE_ID)
        return (egid);
    else
        return (0);
}
#endif
/*  -------------------------------------------------------------------------
    ipr_uid_user_name

    Type: Component method
    Get user name from passwd file.  Places the user name into the
    provided short string.  Returns 0 on success, 1 if the name could
    not be found.
    -------------------------------------------------------------------------
 */

int
    ipr_uid_user_name (
    uid_t uid,                          //  UID to look up
    char * name                         //  String to receive name
)
{
#   if (defined (DOES_UID))
    struct passwd
        *passwd_entry;
#   endif
    int
        rc;                             //  Return value

    rc = 0;
#   if (defined (__MSDOS__))
    icl_shortstr_cpy (name, "user");
#   else
    if ((passwd_entry = getpwuid (uid)) == NULL)
        icl_shortstr_cpy (name, "[none]");
    else
        icl_shortstr_cpy (name, passwd_entry->pw_name);
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_uid_group_name

    Type: Component method
    Get group name from group file.  Places the group name into the
    provided short string.  Returns 0 on success, 1 if the name could
    not be found.
    -------------------------------------------------------------------------
 */

int
    ipr_uid_group_name (
    uid_t gid,                          //  GID to look up
    char * name                         //  String to receive name
)
{
#   if (defined (DOES_UID))
    struct group
        *group_entry;
#   endif
    int
        rc;                             //  Return value

    rc = 0;
#   if (defined (DOES_UID))
    if ((group_entry = getgrgid (gid)) == NULL)
        icl_shortstr_cpy (name, "[none]");
    else
        icl_shortstr_cpy (name, group_entry->gr_name);
#   else
    icl_shortstr_cpy (name, "group");
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_uid_set_real_user

    Type: Component method
    This function switches to the real user id. This function can be used
    by 'setuid' programs; i.e. programs that run under a fixed uid such as
    'root'.  Typically such programs need to access root resources, but user
    data files.  To do this they must switch between the 'root' uid and
    the 'user' uid. Use ipr_uid_set_root_user () to switch (back) to the
    'root' uid.
    -------------------------------------------------------------------------
 */

int
    ipr_uid_set_real_user (
void)
{
    int
        rc;                             //  Return value

#   if (defined (DOES_UID))
    rc = seteuid (get_uid (REAL_ID));
#   else
    rc = 0;
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_uid_set_root_user

    Type: Component method
    Sets the current process effective user ID to the effective user ID,
    which is root for setuid programs.
    -------------------------------------------------------------------------
 */

int
    ipr_uid_set_root_user (
void)
{
    int
        rc;                             //  Return value

#   if (defined (DOES_UID))
    rc = seteuid (get_uid (EFFECTIVE_ID));
#   else
    rc = 0;
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_uid_set_real_group

    Type: Component method
    This function switches to the real group id. This function can be used
    by 'setuid' programs; i.e. programs that run under a fixed uid such as
    'root'.  Typically such programs need to access root resources, but user
    data files.  To do this they must switch between the 'root' gid and
    the 'user' gid. Use ipr_uid_set_root_group () to switch (back) to the
    'root' gid.
    -------------------------------------------------------------------------
 */

int
    ipr_uid_set_real_group (
void)
{
    int
        rc;                             //  Return value

#   if (defined (DOES_UID))
    rc = setegid (get_gid (REAL_ID));
#   else
    rc = 0;
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_uid_set_root_group

    Type: Component method
    Sets the current process effective group ID to the effective group ID,
    which is root for setuid programs.
    -------------------------------------------------------------------------
 */

int
    ipr_uid_set_root_group (
void)
{
    int
        rc;                             //  Return value

#   if (defined (DOES_UID))
    rc = setegid (get_gid (EFFECTIVE_ID));
#   else
    rc = 0;
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_uid_get_uid

    Type: Component method
    Returns the current process user id.
    -------------------------------------------------------------------------
 */

int
    ipr_uid_get_uid (
void)
{
    int
        uid;                            //  Return value

#   if (defined (DOES_UID))
    uid = get_uid (EFFECTIVE_ID);
#   else
    uid = 0;
#   endif

    return (uid);
}
/*  -------------------------------------------------------------------------
    ipr_uid_get_gid

    Type: Component method
    Returns the current process group id.
    -------------------------------------------------------------------------
 */

int
    ipr_uid_get_gid (
void)
{
    int
        gid;                            //  Return value

#   if (defined (DOES_UID))
    gid = get_gid (REAL_ID);
#   else
    gid = 0;
#   endif

    return (gid);
}
/*  -------------------------------------------------------------------------
    ipr_uid_run_as

    Type: Component method
    Sets the current program's user and group to new values as specified as names.
    The program must be currently running as 'root'.  Returns 0 if the new names
    could be correctly used.  Returns -1 if the user/group could not be changed.
    The group name may be null.
    -------------------------------------------------------------------------
 */

int
    ipr_uid_run_as (
    char * user,                        //  Name of user to switch to
    char * group                        //  Name of group to switch to
)
{
#   if (defined (DOES_UID))
    struct passwd
        *pwdbuf = NULL;
    struct group
        *grpbuf = NULL;
#   endif
    int
        rc;                             //  Return value

    //
    rc = 0;
    assert (user);
#   if (defined (DOES_UID))
    if (group) {
        grpbuf = getgrnam (group);
        if (grpbuf)
            rc = setgid (grpbuf->gr_gid);
        else
            rc = -1;
    }
    if (rc == 0) {
        pwdbuf = getpwnam (user);
        if (pwdbuf)
            rc = setuid (pwdbuf->pw_uid);
        else
            rc = -1;
    }
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_uid_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_uid_selftest (
void)
{
icl_shortstr_t
    user,
    group;

//
ipr_uid_user_name  (ipr_uid_get_uid (), user);
ipr_uid_group_name (ipr_uid_get_gid (), group);
}
/*  -------------------------------------------------------------------------
    ipr_uid_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_uid_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_uid_animating = enabled;
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_uid_version_start        = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_uid_component            = "ipr_uid ";
char *EMBED__ipr_uid_version              = "1.0 ";
char *EMBED__ipr_uid_copyright            = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_uid_filename             = "ipr_uid.icl ";
char *EMBED__ipr_uid_builddate            = "2011/03/01 ";
char *EMBED__ipr_uid_version_end          = "VeRsIoNeNd:ipc";

