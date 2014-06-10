!----------------------------------------------------------------------------!
!   smtsock.l - TCP/IP Socket agent                                          !
!                                                                            !
!   Copyright (c) 1991-2009 iMatix Corporation                               !
!                                                                            !
!   ------------------ GPL Licensed Source Code ------------------           !
!   iMatix makes this software available under the GNU General               !
!   Public License (GPL) license for open source projects.  For              !
!   details of the GPL license please see www.gnu.org or read the            !
!   file license.gpl provided in this package.                               !
!                                                                            !
!   This program is free software; you can redistribute it and/or            !
!   modify it under the terms of the GNU General Public License as           !
!   published by the Free Software Foundation; either version 2 of           !
!   the License, or (at your option) any later version.                      !
!                                                                            !
!   This program is distributed in the hope that it will be useful,          !
!   but WITHOUT ANY WARRANTY; without even the implied warranty of           !
!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            !
!   GNU General Public License for more details.                             !
!                                                                            !
!   You should have received a copy of the GNU General Public                !
!   License along with this program in the file 'license.gpl'; if            !
!   not, write to the Free Software Foundation, Inc., 59 Temple              !
!   Place - Suite 330, Boston, MA 02111-1307, USA.                           !
!                                                                            !
!   You can also license this software under iMatix's General Terms          !
!   of Business (GTB) for commercial projects.  If you have not              !
!   explicitly licensed this software under the iMatix GTB you may           !
!   only use it under the terms of the GNU General Public License.           !
!                                                                            !
!   For more information, send an email to info@imatix.com.                  !
!----------------------------------------------------------------------------!
-schema=smtschm.c

After-Init:
    (--) Ok                                 -> Getting-Event
          + Schedule-Non-Blocking-Select
    (--) Error                              ->
          + Terminate-The-Thread

Getting-Event:
    (--) Read                               -> Getting-Event
          + Create-Read-Request
    (--) Readh                              -> Getting-Event
          + Create-Huge-Read-Request
    (--) Readr                              -> Getting-Event
          + Create-Read-Repeat-Request
    (--) Readrh                             -> Getting-Event
          + Create-Huge-Read-Repeat-Request
    (--) Write                              -> Getting-Event
          + Create-Write-Request
    (--) Writeh                             -> Getting-Event
          + Create-Huge-Write-Request
    (--) Input                              -> Getting-Event
          + Create-Input-Request
    (--) Output                             -> Getting-Event
          + Create-Output-Request
    (--) Connect                            -> Getting-Event
          + Create-Connect-Request
    (--) Close                              -> Getting-Event
          + Create-Close-Request
    (--) Flush                              -> Getting-Event
          + Flush-Socket-Requests
    (--) Timeout                            -> Getting-Event
          + Find-Timed-Out-Request
          + Reply-Timeout-To-Request
    (--) Activity                           -> Socket-Activity
          + Check-First-Socket-Activity
          + Generate-Request-Type-Event

Socket-Activity:
    (--) No-Active-Request                  -> Getting-Event
          + Schedule-Non-Blocking-Select
    (--) Write                              -> After-Activity
          + Write-Data-To-Socket
    (--) Output                             -> Socket-Activity
          + Reply-OK-To-Request
          + Check-Next-Socket-Activity
          + Generate-Request-Type-Event
    (--) Read                               -> After-Activity
          + Read-Data-From-Socket
    (--) Input                              -> Socket-Activity
          + Reply-OK-To-Request
          + Check-Next-Socket-Activity
          + Generate-Request-Type-Event
    (--) Close                              -> Socket-Activity
          + Close-The-Socket
          + Check-Next-Socket-Activity
          + Generate-Request-Type-Event
    (--) Connect                            -> After-Activity
          + Confirm-Socket-Connected
    (--) Exception                          -> Socket-Activity
          + Process-Socket-Exception
          + Check-Next-Socket-Activity
          + Generate-Request-Type-Event

After-Activity:
    (--) Incomplete                         -> Socket-Activity
          + Check-Next-Socket-Activity
          + Generate-Request-Type-Event
    (--) Finished                           -> Socket-Activity
          + Reply-OK-To-Request
          + Check-Next-Socket-Activity
          + Generate-Request-Type-Event
    (--) Closed                             -> Socket-Activity
          + Reply-Closed-To-All-Requests
          + Check-Next-Socket-Activity
          + Generate-Request-Type-Event
    (--) Error                              -> Socket-Activity
          + Reply-Error-To-All-Requests
          + Check-Next-Socket-Activity
          + Generate-Request-Type-Event

Defaults:
    (--) Fatal                              ->
          + Request-Shutdown
          + Destroy-All-Requests
          + Terminate-The-Thread
    (--) Shutdown                           ->
          + Destroy-All-Requests
          + Terminate-The-Thread
