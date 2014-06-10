!----------------------------------------------------------------------------!
!                                                                            !
!   smtecho.l - Echo daemon agent                                            !
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
!----------------------------------------------------------------------------!
-schema=smtschm.c

After-Init:
    (--) Ok                                 -> Start-Thread
          + Check-Socket-Type

Start-Thread:
    (--) Master                             -> Master-Input
          + Open-Agent-Log-File
          + Open-Master-Socket
          + Wait-For-Socket-Input
    (--) Client                             -> Echoing-Data
          + Read-Socket-Data-Repeatedly

Master-Input:
    (--) Ok                                 -> Master-Input
          + Accept-Client-Connection
          + Wait-For-Socket-Input
    (--) Error                              -> End-Thread
          + Signal-Socket-Error
          + Check-Socket-Type
    (--) Closed                             ->
          + Shutdown-The-Application
          + Terminate-The-Thread

Echoing-Data:
    (--) Read-Ok                            -> Echoing-Data
          + Echo-Socket-Data
    (--) Ok                                 -> Echoing-Data
          +
    (--) Closed                             ->
          + Terminate-The-Thread
    (--) Error                              -> End-Thread
          + Signal-Socket-Error
          + Check-Socket-Type

Defaults:
    (--) Exception                          -> End-Thread
          + Check-Socket-Type
    (--) Shutdown                           -> End-Thread
          + Check-Socket-Type
    (--) Fatal                              ->
          + Shutdown-The-Application
          + Terminate-The-Thread

End-Thread:
    (--) Master                             ->
          + Close-Agent-Log-File
          + Terminate-The-Thread
    (--) Client                             ->
          + Terminate-The-Thread
