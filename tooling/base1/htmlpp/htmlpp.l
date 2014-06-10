!*===========================================================================*
!                                                                            *
!   htmlpp.l    HTML pre-processor                                           *
!                                                                            *
!   Written:    96/03/27  iMatix Corporation <ph@imatix.com>                 *
!   Revised:    98/10/20                                                     *
!                                                                            *
!   Copyright (c) 1996-98 iMatix Corporation                                 *
!                                                                            *
!   This program is free software; you can redistribute it and/or modify     *
!   it under the terms of the GNU General Public License as published by     *
!   the Free Software Foundation; either version 2 of the License, or        *
!   (at your option) any later version.                                      *
!                                                                            *
!   This program is distributed in the hope that it will be useful,          *
!   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
!   GNU General Public License for more details.                             *
!                                                                            *
!   You should have received a copy of the GNU General Public License        *
!   along with this program; if not, write to the Free Software              *
!   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                *
!*===========================================================================*
-schema=lrschema.pl

After-Init:
    (--) Ok                                 -> Have-Argument
          + Initialise-Program-Data
          + Load-Anchor-Definitions
          + Get-Next-Argument-Value
    (--) Error                              ->
          + Terminate-The-Program

Have-Argument:
    (--) Ok                                 -> First-Pass
          + Show-Banner-If-Needed
          + Text-To-Htmlpp-If-Guru
          + Start-First-Pass
          + Open-Main-Document
          + Open-Output-Work-File
          + Get-Next-Document-Line
    (--) Switch                             -> Have-Argument
          + Process-The-Switch
          + Get-Next-Argument-Value
    (--) Finished                           ->
          + Save-Anchor-Definitions
          + Terminate-The-Program

!--------------------------    FIRST PASS   --------------------------------

First-Pass:
    (--) Body-Text                          -> First-Pass
          + Expand-Symbols-In-Line
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Macro-Text                         -> First-Pass
          + Expand-Symbols-In-Macro
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Blank-Line                         -> First-Pass
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Comment                            -> First-Pass
          + Get-Next-Document-Line
    (--) Define                             -> First-Pass
          + Store-Symbol-Definition
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Macro                              -> First-Pass
          + Store-Macro-Definition
          + Get-Next-Document-Line
    (--) Include                            -> First-Pass
          + Expand-Symbols-In-Line
          + Take-Include-File-Name
          + Open-The-Document
          + Get-Next-Document-Line
    (--) Ignore                             -> First-Pass
          + Expand-Symbols-In-Line
          + Handle-Ignore-Command
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Page                               -> First-Pass
          + Expand-Symbols-In-Line
          + Check-If-Ignore-Pages
          + Parse-Page-Title-For-Toc
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Ignore-Pages                       -> First-Pass
          + Parse-Page-Title-Only
          + Get-Next-Document-Line
    (--) Block                              -> First-Pass-Block
          + Expand-Symbols-In-Line
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) End-Block                          -> First-Pass
          + Signal-Unexpected-End-Block
          + Get-Next-Document-Line
    (--) If                                 -> First-Pass
          + Expand-Symbols-In-Line
          + Skip-If-Block-If-False
          + Get-Next-Document-Line
    (--) Else                               -> First-Pass
          + Skip-Else-Block-Always
          + Get-Next-Document-Line
    (--) End-If                             -> First-Pass
          + Close-If-Block-If-Open
          + Get-Next-Document-Line
    (--) For                                -> First-Pass
          + Expand-Symbols-In-Line
          + Repeat-For-Loop-Contents
          + Get-Next-Document-Line
    (--) End-For                            -> First-Pass
          + Signal-Unexpected-End-For
          + Get-Next-Document-Line
    (--) Build                              -> First-Pass
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Echo                               -> First-Pass
          + Expand-Symbols-In-Line
          + Echo-Text-To-Console
          + Get-Next-Document-Line
    (--) Finished                           -> Main-Pass
          + Close-The-Document
          + Start-Main-Pass
          + Reuse-Work-File-As-Input
          + Open-Output-Work-File
          + Get-Next-Document-Line
    (--) Finished-Include                   -> First-Pass
          + Get-Next-Document-Line
    (--) Error                              -> First-Pass
          + Get-Next-Document-Line
    (--) Pipe                               -> First-Pass
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) End-Pipe                           -> First-Pass
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line

First-Pass-Block:
    (--) Body-Text                          -> First-Pass-Block
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Macro-Text                         -> First-Pass-Block
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Blank-Line                         -> First-Pass-Block
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Comment                            -> First-Pass-Block
          + Get-Next-Document-Line
    (--) $other                             -> First-Pass-Block
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Block                              -> First-Pass-Block
          + Expand-Symbols-In-Line
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) End-Block                          -> First-Pass
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Finished                           -> Main-Pass
          + Close-The-Document
          + Start-Main-Pass
          + Reuse-Work-File-As-Input
          + Open-Output-Work-File
          + Get-Next-Document-Line
    (--) Finished-Include                   -> First-Pass
          + Get-Next-Document-Line
    (--) Error                              -> First-Pass
          + Get-Next-Document-Line

!--------------------------    MAIN PASS   ---------------------------------

Main-Pass:
    (--) Body-Text                          -> Main-Pass
          + Expand-Symbols-In-Line
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Macro-Text                         -> Main-Pass
          + Expand-Symbols-In-Macro
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Blank-Line                         -> Main-Pass
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Comment                            -> Main-Pass
          + Get-Next-Document-Line
    (--) Define                             -> Main-Pass
          + Store-Symbol-Definition
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Macro                              -> Main-Pass
          + Store-Macro-Definition
          + Get-Next-Document-Line
    (--) Include                            -> Main-Pass
          + Expand-Symbols-In-Line
          + Take-Include-File-Name
          + Open-The-Document
          + Get-Next-Document-Line
    (--) Ignore                             -> Main-Pass
          + Expand-Symbols-In-Line
          + Handle-Ignore-Command
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Page                               -> Main-Pass
          + Check-If-Ignore-Pages
          + Output-Footer-For-Page
          + Expand-Symbols-In-Line
          + Parse-Page-Title-And-Name
          + Copy-Action-Line-To-Output
          + Output-Header-For-Page
          + Get-Next-Document-Line
    (--) Ignore-Pages                       -> Main-Pass
          + Parse-Page-Title-Only
          + Get-Next-Document-Line
    (--) Block                              -> Main-Pass-Block
          + Clear-Specified-Block
          + Get-Next-Document-Line
    (--) End-Block                          -> Main-Pass
          + Signal-Unexpected-End-Block
          + Get-Next-Document-Line
    (--) If                                 -> Main-Pass
          + Expand-Symbols-In-Line
          + Skip-If-Block-If-False
          + Get-Next-Document-Line
    (--) Else                               -> Main-Pass
          + Skip-Else-Block-Always
          + Get-Next-Document-Line
    (--) End-If                             -> Main-Pass
          + Close-If-Block-If-Open
          + Get-Next-Document-Line
    (--) For                                -> Main-Pass
          + Expand-Symbols-In-Line
          + Repeat-For-Loop-Contents
          + Get-Next-Document-Line
    (--) End-For                            -> Main-Pass
          + Signal-Unexpected-End-For
          + Get-Next-Document-Line
    (--) Build                              -> Main-Pass
          + Expand-Symbols-In-Line
          + Build-Specified-Table
          + Get-Next-Document-Line
    (--) Echo                               -> Main-Pass
          + Expand-Symbols-In-Line
          + Echo-Text-To-Console
          + Get-Next-Document-Line
    (--) Finished                           -> Toc-Pass
          + Close-The-Document
          + Output-Footer-For-Page
          + Check-If-Inter-Pass-Needed
          + Start-Main-Pass
          + Reuse-Work-File-As-Input
          + Get-Next-Document-Line
    (--) Finished-Include                   -> Main-Pass
          + Get-Next-Document-Line
    (--) Need-Inter-Pass                    -> Inter-Pass
          + Start-Inter-Pass
          + Reuse-Work-File-As-Input
          + Open-Output-Work-File
          + Get-Next-Document-Line
    (--) Error                              -> Main-Pass
          + Get-Next-Document-Line
    (--) Pipe                               -> Main-Pass
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) End-Pipe                           -> Main-Pass
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line

Main-Pass-Block:
    (--) Body-Text                          -> Main-Pass-Block
          + Add-Line-To-Block
          + Get-Next-Document-Line
    (--) Macro-Text                         -> Main-Pass-Block
          + Add-Line-To-Block
          + Get-Next-Document-Line
    (--) Comment                            -> Main-Pass-Block
          + Get-Next-Document-Line
    (--) Blank-Line                         -> Main-Pass-Block
          + Add-Line-To-Block
          + Get-Next-Document-Line
    (--) $other                             -> Main-Pass-Block
          + Add-Action-Line-To-Block
          + Get-Next-Document-Line
    (--) Block                              -> Main-Pass-Block
          + Clear-Specified-Block
          + Get-Next-Document-Line
    (--) End-Block                          -> Main-Pass
          + Get-Next-Document-Line
    (--) Finished                           -> Toc-Pass
          + Close-The-Document
          + Output-Footer-For-Page
          + Check-If-Inter-Pass-Needed
          + Start-Main-Pass
          + Reuse-Work-File-As-Input
          + Get-Next-Document-Line
    (--) Finished-Include                   -> Main-Pass
          + Get-Next-Document-Line
    (--) Need-Inter-Pass                    -> Inter-Pass
          + Start-Main-Pass
          + Reuse-Work-File-As-Input
          + Open-Output-Work-File
          + Get-Next-Document-Line
    (--) Error                              -> Main-Pass
          + Get-Next-Document-Line

!--------------------------    INTER PASS   --------------------------------

Inter-Pass:
    (--) Body-Text                          -> Inter-Pass
          + Expand-Symbols-In-Line
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Macro-Text                         -> Inter-Pass
          + Expand-Symbols-In-Macro
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Blank-Line                         -> Inter-Pass
          + Copy-Line-To-Output
          + Get-Next-Document-Line
    (--) Comment                            -> Inter-Pass
          + Get-Next-Document-Line
    (--) Define                             -> Inter-Pass
          + Store-Symbol-Definition
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Macro                              -> Inter-Pass
          + Store-Macro-Definition
          + Get-Next-Document-Line
    (--) Include                            -> Inter-Pass
          + Expand-Symbols-In-Line
          + Take-Include-File-Name
          + Open-The-Document
          + Get-Next-Document-Line
    (--) Ignore                             -> Inter-Pass
          + Expand-Symbols-In-Line
          + Handle-Ignore-Command
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Page                               -> Inter-Pass
          + Check-If-Ignore-Pages
          + Parse-Page-Title-And-Name
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) Ignore-Pages                       -> Inter-Pass
          + Parse-Page-Title-Only
          + Get-Next-Document-Line
    (--) Block                              -> Inter-Pass-Block
          + Clear-Specified-Block
          + Get-Next-Document-Line
    (--) End-Block                          -> Inter-Pass
          + Signal-Unexpected-End-Block
          + Get-Next-Document-Line
    (--) If                                 -> Inter-Pass
          + Expand-Symbols-In-Line
          + Skip-If-Block-If-False
          + Get-Next-Document-Line
    (--) Else                               -> Inter-Pass
          + Skip-Else-Block-Always
          + Get-Next-Document-Line
    (--) End-If                             -> Inter-Pass
          + Close-If-Block-If-Open
          + Get-Next-Document-Line
    (--) For                                -> Inter-Pass
          + Expand-Symbols-In-Line
          + Repeat-For-Loop-Contents
          + Get-Next-Document-Line
    (--) End-For                            -> Inter-Pass
          + Signal-Unexpected-End-For
          + Get-Next-Document-Line
    (--) Build                              -> Inter-Pass
          + Expand-Symbols-In-Line
          + Build-Specified-Table
          + Get-Next-Document-Line
    (--) Echo                               -> Inter-Pass
          + Expand-Symbols-In-Line
          + Echo-Text-To-Console
          + Get-Next-Document-Line
    (--) Finished                           -> Toc-Pass
          + Close-The-Document
          + Check-If-Inter-Pass-Needed
          + Start-Main-Pass
          + Reuse-Work-File-As-Input
          + Get-Next-Document-Line
    (--) Finished-Include                   -> Inter-Pass
          + Get-Next-Document-Line
    (--) Need-Inter-Pass                    -> Inter-Pass
          + Start-Inter-Pass
          + Reuse-Work-File-As-Input
          + Open-Output-Work-File
          + Get-Next-Document-Line
    (--) Error                              -> Inter-Pass
          + Get-Next-Document-Line
    (--) Pipe                               -> Inter-Pass
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line
    (--) End-Pipe                           -> Inter-Pass
          + Copy-Action-Line-To-Output
          + Get-Next-Document-Line

Inter-Pass-Block:
    (--) Body-Text                          -> Inter-Pass-Block
          + Add-Line-To-Block
          + Get-Next-Document-Line
    (--) Macro-Text                         -> Inter-Pass-Block
          + Add-Line-To-Block
          + Get-Next-Document-Line
    (--) Comment                            -> Inter-Pass-Block
          + Get-Next-Document-Line
    (--) Blank-Line                         -> Inter-Pass-Block
          + Add-Line-To-Block
          + Get-Next-Document-Line
    (--) $other                             -> Inter-Pass-Block
          + Add-Action-Line-To-Block
          + Get-Next-Document-Line
    (--) Block                              -> Inter-Pass-Block
          + Clear-Specified-Block
          + Get-Next-Document-Line
    (--) End-Block                          -> Inter-Pass
          + Get-Next-Document-Line
    (--) Finished                           -> Toc-Pass
          + Close-The-Document
          + Check-If-Inter-Pass-Needed
          + Start-Main-Pass
          + Reuse-Work-File-As-Input
          + Get-Next-Document-Line
    (--) Finished-Include                   -> Inter-Pass
          + Get-Next-Document-Line
    (--) Need-Inter-Pass                    -> Inter-Pass
          + Start-Inter-Pass
          + Reuse-Work-File-As-Input
          + Open-Output-Work-File
          + Get-Next-Document-Line
    (--) Error                              -> Inter-Pass
          + Get-Next-Document-Line

!--------------------------    TOC PASS   --------------------------------

Toc-Pass:
    (--) Body-Text                          -> Toc-Pass
          + Collect-Title-If-Present
          + Get-Next-Document-Line
    (--) Macro-Text                         -> Toc-Pass
          + Collect-Title-If-Present
          + Get-Next-Document-Line
    (--) Page                               -> Toc-Pass
          + Check-If-Ignore-Pages
          + Parse-Page-Title-And-Name
          + Get-Next-Document-Line
    (--) Ignore                             -> Toc-Pass
          + Expand-Symbols-In-Line
          + Handle-Ignore-Command
          + Get-Next-Document-Line
    (--) Ignore-Pages                       -> Toc-Pass
          + Parse-Page-Title-Only
          + Get-Next-Document-Line
    (--) Finished                           -> Final-Pass
          + Start-Final-Pass
          + Reuse-Work-File-As-Input
          + Get-Next-Document-Line
    (--) $other                             -> Toc-Pass
          + Get-Next-Document-Line

!--------------------------    FINAL PASS   --------------------------------

Final-Pass:
    (--) Body-Text                          -> Final-Pass
          + Expand-Symbols-In-Line
          + Collect-Title-If-Present
          + Copy-Line-To-Final-Output
          + Get-Next-Document-Line
    (--) Macro-Text                         -> Final-Pass
          + Expand-Symbols-In-Line
          + Collect-Title-If-Present
          + Copy-Line-To-Final-Output
          + Get-Next-Document-Line
    (--) Blank-Line                         -> Final-Pass
          + Copy-Line-To-Final-Output
          + Get-Next-Document-Line
    (--) Ignore                             -> Final-Pass
          + Expand-Symbols-In-Line
          + Handle-Ignore-Command
          + Get-Next-Document-Line
    (--) Page                               -> Final-Pass
          + Expand-Symbols-In-Line
          + Parse-Page-Title-And-Name
          + Open-New-Output-Page
          + Get-Next-Document-Line
    (--) Pipe                               -> Final-Pass
          + Open-Piped-Output
          + Get-Next-Document-Line
    (--) End-Pipe                           -> Final-Pass
          + Close-Piped-Output
          + Get-Next-Document-Line
    (--) Define                             -> Final-Pass
          + Get-Next-Document-Line
    (--) Build                              -> Final-Pass
          + Expand-Symbols-In-Line
          + Build-Specified-Table-Final
          + Get-Next-Document-Line
    (--) Finished                           -> Have-Argument
          + Close-The-Document
          + Clean-Up-Work-Files
          + Signal-Document-Processed
          + Get-Next-Argument-Value
    (--) Error                              -> Final-Pass
          + Get-Next-Document-Line
    (--) $other                             -> Final-Pass
          + Signal-Internal-Failure
          + Get-Next-Document-Line
Defaults:
    (--) Exception                          ->
          + Terminate-The-Program
