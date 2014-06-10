!===========================================================================
!                                                                           
!   srcdoc.l - Srcdoc Perl script
!                               
!   Copyright (c) 1991-2009 iMatix Corporation
!
!   ------------------ GPL Licensed Source Code ------------------
!   iMatix makes this software available under the GNU General
!   Public License (GPL) license for open source projects.  For
!   details of the GPL license please see www.gnu.org or read the
!   file license.gpl provided in this package.
!   
!   This program is free software; you can redistribute it and/or
!   modify it under the terms of the GNU General Public License as
!   published by the Free Software Foundation; either version 2 of
!   the License, or (at your option) any later version.
!   
!   This program is distributed in the hope that it will be useful,
!   but WITHOUT ANY WARRANTY; without even the implied warranty of
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!   GNU General Public License for more details.
!   
!   You should have received a copy of the GNU General Public
!   License along with this program in the file 'license.gpl'; if
!   not, write to the Free Software Foundation, Inc., 59 Temple
!   Place - Suite 330, Boston, MA 02111-1307, USA.
!   
!   You can also license this software under iMatix's General Terms
!   of Business (GTB) for commercial projects.  If you have not
!   explicitly licensed this software under the iMatix GTB you may
!   only use it under the terms of the GNU General Public License.
!   
!   For more information, send an email to info@imatix.com.
!   --------------------------------------------------------------
!
!===========================================================================

-schema=lrschema.pl

After-Init:
    (--) Ok                                 -> Have-Source-File
          + Initialise-Program-Data
          + Load-Anchor-Definitions
          + Parse-Coverpage-File
          + Get-Next-Source-File
    (--) Error                              ->
          + Terminate-The-Program

Have-Source-File:
    (--) Include                            -> Parsing-Include
          + May-Have-First-Include-File
          + Prepare-To-Parse-File
          + Look-For-Interesting-Line
    (--) Code                               -> Parsing-Code
          + May-Have-First-Code-File
          + Prepare-To-Parse-File
          + Look-For-Interesting-Line
    (--) Empty                              -> Have-Source-File
          + Get-Next-Source-File
    (--) No-More                            ->
          + Terminate-The-Program

Parsing-Include:
    (--) Prolog                             -> Parsing-Include
          + Build-Tables-Of-Definitions
          + Parse-Prolog-Block
          + Look-For-Interesting-Line
    (--) Define                             -> Parsing-Include
          + Parse-Define-Command
          + Look-For-Interesting-Line
    (--) Typedef                            -> Parsing-Include
          + Parse-Typedef-Command
          + Look-For-Interesting-Line
    (--) Proto                              -> Parsing-Include
          + Parse-Function-Prototype
          + Look-For-Interesting-Line
    (--) Header                             -> Parsing-Include
          + Ignore-Function-Header
          + Look-For-Interesting-Line
    (--) No-More                            -> Have-Source-File
          + Build-Tables-Of-Definitions
          + Get-Next-Source-File

Parsing-Code:
    (--) Prolog                             -> Parsing-Code
          + Look-For-Interesting-Line
    (--) Define                             -> Parsing-Code
          + Look-For-Interesting-Line
    (--) Typedef                            -> Parsing-Code
          + Look-For-Interesting-Line
    (--) Proto                              -> Parsing-Code
          + Look-For-Interesting-Line
    (--) Header                             -> Parsing-Code
          + Parse-Function-Header
          + Parse-Function-Body
          + Build-Function-Definition
          + Look-For-Interesting-Line
    (--) No-More                            -> Have-Source-File
          + Get-Next-Source-File

Defaults:
    (--) Exception                          ->
          + Terminate-The-Program
