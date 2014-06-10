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
    name      = "ipr_uid"
    comment   = "User and group ID manipulation"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Provides functions to access and maniuplate the current process
    user and group IDs.
</doc>
<inherit class = "icl_base" />

<import class = "icl_shortstr" />

<private name = "header">
#define REAL_ID             0           //  Arguments for get_uid/get_gid
#define EFFECTIVE_ID        1
</private>

<method name = "user name" return = "rc">
    <doc>
    Get user name from passwd file.  Places the user name into the
    provided short string.  Returns 0 on success, 1 if the name could
    not be found.
    </doc>
    <argument name = "uid"  type = "uid_t" >UID to look up</argument>
    <argument name = "name" type = "char *">String to receive name</argument>
    <declare name = "rc" type = "int">Return value</declare>
    <local>
#   if (defined (DOES_UID))
    struct passwd
        *passwd_entry;
#   endif
    </local>
    rc = 0;
#   if (defined (__MSDOS__))
    icl_shortstr_cpy (name, "user");
#   else
    if ((passwd_entry = getpwuid (uid)) == NULL)
        icl_shortstr_cpy (name, "[none]");
    else
        icl_shortstr_cpy (name, passwd_entry->pw_name);
#   endif
</method>

<method name = "group name" return = "rc">
    <doc>
    Get group name from group file.  Places the group name into the
    provided short string.  Returns 0 on success, 1 if the name could
    not be found.
    </doc>
    <argument name = "gid"  type = "uid_t" >GID to look up</argument>
    <argument name = "name" type = "char *">String to receive name</argument>
    <declare name = "rc" type = "int">Return value</declare>
    <local>
#   if (defined (DOES_UID))
    struct group
        *group_entry;
#   endif
    </local>
    rc = 0;
#   if (defined (DOES_UID))
    if ((group_entry = getgrgid (gid)) == NULL)
        icl_shortstr_cpy (name, "[none]");
    else
        icl_shortstr_cpy (name, group_entry->gr_name);
#   else
    icl_shortstr_cpy (name, "group");
#   endif
</method>

<method name = "set real user" return = "rc">
    <doc>
    This function switches to the real user id. This function can be used
    by 'setuid' programs; i.e. programs that run under a fixed uid such as
    'root'.  Typically such programs need to access root resources, but user
    data files.  To do this they must switch between the 'root' uid and
    the 'user' uid. Use ipr_uid_set_root_user () to switch (back) to the
    'root' uid.
    </doc>
    <declare name = "rc" type = "int">Return value</declare>
#   if (defined (DOES_UID))
    rc = seteuid (get_uid (REAL_ID));
#   else
    rc = 0;
#   endif
</method>

<method name = "set root user" return = "rc">
    <doc>
    Sets the current process effective user ID to the effective user ID,
    which is root for setuid programs.
    </doc>
    <declare name = "rc" type = "int">Return value</declare>
#   if (defined (DOES_UID))
    rc = seteuid (get_uid (EFFECTIVE_ID));
#   else
    rc = 0;
#   endif
</method>

<method name = "set real group" return = "rc">
    <doc>
    This function switches to the real group id. This function can be used
    by 'setuid' programs; i.e. programs that run under a fixed uid such as
    'root'.  Typically such programs need to access root resources, but user
    data files.  To do this they must switch between the 'root' gid and
    the 'user' gid. Use ipr_uid_set_root_group () to switch (back) to the
    'root' gid.
    </doc>
    <declare name = "rc" type = "int">Return value</declare>
#   if (defined (DOES_UID))
    rc = setegid (get_gid (REAL_ID));
#   else
    rc = 0;
#   endif
</method>

<method name = "set root group" return = "rc">
    <doc>
    Sets the current process effective group ID to the effective group ID,
    which is root for setuid programs.
    </doc>
    <declare name = "rc" type = "int">Return value</declare>
#   if (defined (DOES_UID))
    rc = setegid (get_gid (EFFECTIVE_ID));
#   else
    rc = 0;
#   endif
</method>

<method name = "get uid" return = "uid">
    <doc>
    Returns the current process user id.
    </doc>
    <declare name = "uid" type = "int">Return value</declare>
#   if (defined (DOES_UID))
    uid = get_uid (EFFECTIVE_ID);
#   else
    uid = 0;
#   endif
</method>

<method name = "get gid" return = "gid">
    <doc>
    Returns the current process group id.
    </doc>
    <declare name = "gid" type = "int">Return value</declare>
#   if (defined (DOES_UID))
    gid = get_gid (REAL_ID);
#   else
    gid = 0;
#   endif
</method>

<method name = "run as" return = "rc">
    <doc>
    Sets the current program's user and group to new values as specified as names.
    The program must be currently running as 'root'.  Returns 0 if the new names
    could be correctly used.  Returns -1 if the user/group could not be changed.
    The group name may be null.
    </doc>
    <argument name = "user"  type = "char *">Name of user to switch to</argument>
    <argument name = "group" type = "char *">Name of group to switch to</argument>
    <declare name = "rc" type = "int">Return value</declare>
    <local>
#   if (defined (DOES_UID))
    struct passwd
        *pwdbuf = NULL;
    struct group
        *grpbuf = NULL;
#   endif
    </local>
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
</method>

<private name = "header">
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
</private>

<method name = "selftest">
    <local>
    icl_shortstr_t
        user,
        group;
    </local>
    //
    ipr_uid_user_name  (ipr_uid_get_uid (), user);
    ipr_uid_group_name (ipr_uid_get_gid (), group);
</method>

</class>

