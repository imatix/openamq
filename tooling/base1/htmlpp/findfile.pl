#   findfile.pl     Finds a file on a path symbol; returns full filename
#                   or an empty string if the file was not found.
#
#   Copyright (c) 1991-2009 iMatix Corporation
#
#   $name = &findfile (filename, path_symbol);
#   e.g.    $filename = &findfile ("myfile.c", "PATH");
#
#   ------------------ GPL Licensed Source Code ------------------
#   iMatix makes this software available under the GNU General
#   Public License (GPL) license for open source projects.  For
#   details of the GPL license please see www.gnu.org or read the
#   file license.gpl provided in this package.
#   
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2 of
#   the License, or (at your option) any later version.
#   
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#   
#   You should have received a copy of the GNU General Public
#   License along with this program in the file 'license.gpl'; if
#   not, write to the Free Software Foundation, Inc., 59 Temple
#   Place - Suite 330, Boston, MA 02111-1307, USA.
#   
#   You can also license this software under iMatix's General Terms
#   of Business (GTB) for commercial projects.  If you have not
#   explicitly licensed this software under the iMatix GTB you may
#   only use it under the terms of the GNU General Public License.
#   
#   For more information, send an email to info@imatix.com.
#   --------------------------------------------------------------
#

sub findfile {
    local ($filename, $path) = @_;
    local (@path_table);

    #   Split path differently on MS-DOS and UNIX (ugh!)
    @path_table = split (defined ($ENV {"COMSPEC"})? ";": ":", $ENV {$path});

    #   If file has absolute path, or exists locally, that's fine for us
    return $filename if -f $filename;
    foreach (@path_table) {
        return "$_/$filename" if -f "$_/$filename";
    }
    return "";
}

1;
