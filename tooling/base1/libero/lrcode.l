!*===========================================================================*
!                                                                            *
!   lrcode.l - Code generator functions                                      *
!                                                                            *
!   Copyright (c) 1991-2009 iMatix Corporation                               *
!                                                                            *
!   ------------------ GPL Licensed Source Code ------------------           *
!   iMatix makes this software available under the GNU General               *
!   Public License (GPL) license for open source projects.  For              *
!   details of the GPL license please see www.gnu.org or read the            *
!   file license.gpl provided in this package.                               *
!                                                                            *
!   This program is free software; you can redistribute it and/or            *
!   modify it under the terms of the GNU General Public License as           *
!   published by the Free Software Foundation; either version 2 of           *
!   the License, or (at your option) any later version.                      *
!                                                                            *
!   This program is distributed in the hope that it will be useful,          *
!   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
!   GNU General Public License for more details.                             *
!                                                                            *
!   You should have received a copy of the GNU General Public                *
!   License along with this program in the file 'license.gpl'; if            *
!   not, write to the Free Software Foundation, Inc., 59 Temple              *
!   Place - Suite 330, Boston, MA 02111-1307, USA.                           *
!                                                                            *
!   You can also license this software under iMatix's General Terms          *
!   of Business (GTB) for commercial projects.  If you have not              *
!   explicitly licensed this software under the iMatix GTB you may           *
!   only use it under the terms of the GNU General Public License.           *
!                                                                            *
!   For more information, send an email to info@imatix.com.                  *
!   --------------------------------------------------------------           *
!*===========================================================================*
-source=lrcode

After-Init:
    (--) Ok                                 -> Have-Schema-Line
          + Alloc-Symbol-Table
          + Open-Schema-File
          + Get-Next-Schema-Line
    (--) Finished                           ->
          + Terminate-The-Program

Have-Schema-Line:
    (--) Comment                            ->
          + Get-Next-Schema-Line
    (--) Source-Line                        -> Have-Schema-Line
          + Output-Line-Of-Source
          + Get-Next-Schema-Line
    (--) Option                             -> Have-Schema-Line
          + Parse-Option-Value
          + Get-Next-Schema-Line
    (--) Declare                            -> Have-Schema-Line
          + Declare-User-Variable
          + Get-Next-Schema-Line
    (--) Output                             -> Have-Schema-Line
          + Open-Output-File
          + Get-Next-Schema-Line
    (--) Extend                             -> Have-Schema-Line
          + Open-Extend-File
          + Get-Next-Schema-Line
    (--) Close                              -> Have-Schema-Line
          + Check-Output-Opened
          + Close-Output-File
          + Get-Next-Schema-Line
    (--) Include                            -> Have-Schema-Line
          + Include-Specified-File
          + Get-Next-Schema-Line
    (--) Rename                             -> Have-Schema-Line
          + Rename-Specified-File
          + Get-Next-Schema-Line
    (--) Copy                               -> Have-Schema-Line
          + Copy-Specified-File
          + Get-Next-Schema-Line
    (--) Delete                             -> Have-Schema-Line
          + Delete-Specified-File
          + Get-Next-Schema-Line
    (--) Echo                               -> Have-Schema-Line
          + Write-Message-To-Stdout
          + Get-Next-Schema-Line
    (--) Set                                -> Have-Schema-Line
          + Assign-Variable-Value
          + Get-Next-Schema-Line
    (--) Push                               -> Have-Schema-Line
          + Push-Variable-Value
          + Get-Next-Schema-Line
    (--) Pop                                -> Have-Schema-Line
          + Pop-Variable-Value
          + Get-Next-Schema-Line
    (--) Substr                             -> Have-Schema-Line
          + Parse-Substring-Specification
          + Get-Next-Schema-Line
    (--) Internal                           -> Have-Schema-Line
          + Parse-Internal-Module-Name
          + Get-Next-Schema-Line
    (--) Do                                 -> After-Test-Do
          + Open-Do-Block-Level
          + Parse-Do-Block
          + Check-Do-Is-Not-Reentrant
          + Test-Do-Conditions
    (--) Enddo                              -> After-Test-Do
          + Check-Do-Block-Is-Open
          + Test-Do-Conditions
    (--) If                                 -> After-Test-If
          + Open-If-Block-Level
          + Parse-If-Block
          + Test-If-Conditions
    (--) Else                               -> Have-Schema-Line
          + Check-If-Block-Is-Open
          + Skip-To-If-Block-End
          + Close-If-Block-Level
          + Get-Next-Schema-Line
    (--) Endif                              -> Have-Schema-Line
          + Check-If-Block-Is-Open
          + Close-If-Block-Level
          + Get-Next-Schema-Line
    (--) Exit                               ->
          + Set-Exit-Code-If-Any
          + Close-Output-File
          + Close-Schema-File
          + Trace-Symbols-If-Wanted
          + Free-Symbol-Table
          + Terminate-The-Program
    (--) File-Exists                        ->
          + Close-Output-File
          + Close-Schema-File
          + Trace-Symbols-If-Wanted
          + Free-Symbol-Table
          + Terminate-The-Program
    (--) Finished                           ->
          + Close-Output-File
          + Close-Schema-File
          + Trace-Symbols-If-Wanted
          + Free-Symbol-Table
          + Terminate-The-Program

After-Test-Do:
    (--) Ok                                 -> Have-Schema-Line
          + Skip-To-Do-Block-Start
          + Get-Next-Schema-Line
    (--) Ignore                             -> After-Test-Do
          + Test-Do-Conditions
    (--) Done                               -> Have-Schema-Line
          + Skip-To-Do-Block-End
          + Close-Do-Block-Level
          + Get-Next-Schema-Line

After-Test-If:
    (--) True                               -> Have-Schema-Line
          + Skip-To-If-Block-Start
          + Get-Next-Schema-Line
    (--) False                              -> Have-Schema-Line
          + Check-If-Else-Defined
          + Skip-To-If-Block-End
          + Close-If-Block-Level
          + Get-Next-Schema-Line
    (--) Have-Else                          -> Have-Schema-Line
          + Skip-To-If-Block-Else
          + Get-Next-Schema-Line

Defaults:
    (--) Error                              ->
          + Close-Output-File
          + Close-Schema-File
          + Trace-Symbols-If-Wanted
          + Return-Error-Feedback
          + Free-Symbol-Table
          + Terminate-The-Program
