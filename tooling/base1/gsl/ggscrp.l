!----------------------------------------------------------------------------*
!                                                                            *
!   ggscrp.l - Script execution functions                                    *
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
!----------------------------------------------------------------------------*

-schema=smtschm.c

After-Init:
    (--) File                               -> Parsing
          + Wait-On-Semaphore
          + Open-The-File
          + Parse-Line
    (--) Text                               -> Parsing
          + Open-The-Text
          + Parse-Line
    (--) Wait                               ->
          + Wait-On-Semaphore
          + Return-Done
          + Terminate-The-Thread
    (--) Error                              ->
          + Destroy-Script-Source-Node
          + Send-File-Error-Message
          + Return-Done
          + Terminate-The-Thread

!- Dialog state while parsing text for direct output

Parsing:
    (--) Ok                                 -> Treating
          + Save-The-Node
          + Generate-Node-Type-Event
    (--) End-Of-File                        ->
          + Confirm-Control-Stack-Is-Empty
          + Clean-Control-Stack
          + Return-Done
          + Terminate-The-Thread
    (--) Unmatched-Control                  ->
          + Send-Unmatched-Control-Error
          + Clean-Control-Stack
          + Return-Done
          + Terminate-The-Thread
    (--) Error                              -> Parsing
          + Send-Error-Message
          + Parse-Line

Treating:
    (--) For                                -> Parsing
          + Save-Loop-Start
          + Push-The-Control
          + Parse-Line
    (--) Scope                              -> Parsing
          + Push-The-Control
          + Parse-Line
    (--) While                              -> Parsing
          + Save-Loop-Start
          + Push-The-Control
          + Parse-Line
    (--) If                                 -> Parsing
          + Push-The-Control
          + Parse-Line
    (--) New                                -> Parsing
          + Push-The-Control
          + Parse-Line
    (--) Macro                              -> Parsing
          + Push-The-Control
          + Set-Template-Mode
          + Parse-Line
    (--) Function                           -> Parsing
          + Push-The-Control
          + Set-GSL-Mode
          + Parse-Line
    (--) Template                           -> Parsing
          + Push-The-Control
          + Set-Template-Or-GSL-Mode
          + Parse-Line
    (--) End-If                             -> Parsing
          + Pop-If-Or-Else-Control
          + Parse-Line
    (--) Else                               -> Parsing
          + Pop-If-Control
          + Push-The-Control
          + Parse-Line
    (--) Else-If                            -> Parsing
          + Pop-If-Control
          + Push-If-Control
          + Parse-Line
    (--) End-For                            -> Parsing
          + Pop-For-Control
          + Get-Previous-Loop-Start
          + Parse-Line
    (--) End-Scope                          -> Parsing
          + Pop-Scope-Control
          + Parse-Line
    (--) End-While                          -> Parsing
          + Pop-While-Control
          + Get-Previous-Loop-Start
          + Parse-Line
    (--) End-Macro                          -> Parsing
          + Pop-Macro-Control
          + Parse-Line
    (--) End-Function                       -> Parsing
          + Pop-Function-Control
          + Parse-Line
    (--) End-New                            -> Parsing
          + Pop-New-Control
          + Parse-Line
    (--) End-Template                       -> Parsing
          + Pop-Template-Control
          + Parse-Line
    (--) Incoming                           -> Skip-Incoming
          + Read-Script-Line
    (--) Other                              -> Parsing
          + Parse-Line
    (--) Unmatched-Control                  -> Parsing
          + Send-Unmatched-Control-Error
          + Parse-Line

Skip-Incoming:
    (--) Ok                                 -> 
          + Extend-Script-Line
    (--) Terminator                         -> Parsing
          + Parse-Line
    (--) End-Of-File                        -> 
          + Unexpected-EOF-Error
          + Clean-Control-Stack
          + Return-Done
          + Terminate-The-Thread

Defaults:
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Shutdown-Everything
    (--) Shutdown                           ->
          + Terminate-The-Thread
