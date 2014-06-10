!-----------------------------------------------------------------------------
!                                                                            !
!   smtxlog.l - SMT extended logging agent                                   !
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
!   --------------------------------------------------------------           !
!-----------------------------------------------------------------------------
-schema=smtschm.c

After-Init:
    (--) Ok                                 -> Before-Opening
          + Create-Event-Queue
    (--) Error                              ->
          + Terminate-The-Thread

Before-Opening:
    (--) Open                               -> Logfile-Open
          + Get-Logfile-Parameters
          + Build-Log-Filename
          + Check-If-Cycle-Before-Open
          + Open-Logfile-Append
          + Recalculate-Cycle-Timer
          + Check-Queued-Events-If-Any
    (--) Cycle-Now                          -> Cycle-Before-Open
          + Check-Cycle-Method
    (--) Cycle                              -> Cycle-While-Closed
          + Check-Cycle-Method
    (--) Clear                              -> Before-Opening
          + Delete-Old-Logfile

Cycle-Before-Open:
    (--) Rename-Old                         -> Logfile-Open
          + Rename-Old-Logfile
          + Open-Logfile-Append
          + Recalculate-Cycle-Timer
          + Check-Queued-Events-If-Any
    (--) Delete-Old                         -> Logfile-Open
          + Open-Logfile-Create
          + Recalculate-Cycle-Timer
          + Check-Queued-Events-If-Any
    (--) Move-Old                           -> Logfile-Open
          + Move-Old-Logfile
          + Open-Logfile-Append
          + Recalculate-Cycle-Timer
          + Check-Queued-Events-If-Any
    (--) Concat-Old                         -> Logfile-Open
          + Concat-Old-Logfile
          + Open-Logfile-Append
          + Recalculate-Cycle-Timer
          + Check-Queued-Events-If-Any
    (--) Process-Old                        -> Logfile-Open
          + Process-Old-Logfile
          + Open-Logfile-Create
          + Recalculate-Cycle-Timer
          + Check-Queued-Events-If-Any

Cycle-While-Closed:
    (--) Rename-Old                         -> Before-Opening
          + Rename-Old-Logfile
    (--) Delete-Old                         -> Before-Opening
          + Delete-Old-Logfile
    (--) Move-Old                           -> Before-Opening
          + Move-Old-Logfile
    (--) Concat-Old                         -> Before-Opening
          + Concat-Old-Logfile
          + Delete-Old-Logfile
    (--) Process-Old                        -> Before-Opening
          + Process-Old-Logfile

Logfile-Open:
    (--) Log                                -> Logfile-Open
          + Get-Log-Request-Arguments
          + Write-Request-Log-Entry
          + Free-Log-Request-Arguments
          + Check-Queued-Events-If-Any
    (--) Put                                -> Logfile-Open
          + Write-Plain-Log-Entry
          + Check-Queued-Events-If-Any
    (--) Cycle                              -> Cycle-Before-Open
          + Close-Logfile
          + Check-Cycle-Method
    (--) Clear                              -> Logfile-Open
          + Close-Logfile
          + Open-Logfile-Create
          + Check-Queued-Events-If-Any
    (--) Close                              -> Before-Opening
          + Close-Logfile
          + Flush-Event-Queue
    (--) Timer                              -> Logfile-Open
          + Check-If-Cycle-While-Open
          + Recalculate-Cycle-Timer
          + Check-Queued-Events-If-Any
    (--) Cycle-Now                          -> Cycle-Before-Open
          + Close-Logfile
          + Check-Cycle-Method

Defaults:
    (--) Exception                          ->
          + Destroy-Event-Queue
          + Terminate-The-Thread
    (--) Shutdown                           ->
          + Destroy-Event-Queue
          + Terminate-The-Thread
    (--) $other                             ->
          +
