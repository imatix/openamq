!----------------------------------------------------------------------------!
!                                                                            !
!   smttst3.l - Test reverse-DNS agent                                       !
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
    (--) Ok                                 -> Process-Argument
          + Initialise-Dns-Library
          + Display-Dns-Server-Address
          + Get-Next-Argument
          + Get-Command
    (--) Error                              ->
          + Terminate-The-Thread

Process-Argument:
    (--) Help                               -> Process-Argument
          + Display-Help
          + Get-Next-Argument
          + Get-Command
    (--) Debug                              -> Process-Argument
          + Set-Debug-Mode
          + Get-Next-Argument
          + Get-Command
    (--) Recursive                          -> Process-Argument
          + Set-Recursive-Mode
          + Get-Next-Argument
          + Get-Command
    (--) Server-Ip                          -> Process-Argument
          + Set-Server-Ip-Value
          + Display-Dns-Server-Address
          + Get-Next-Argument
          + Get-Command
    (--) Request                            -> Send-Request
          + Get-Argument-Type
    (--) Invalid                            -> Process-Argument
          + Display-Invalid-Argument
          + Get-Next-Argument
          + Get-Command
    (--) None                               -> Process-Command
          + Set-Prompt
          + Get-Command

Process-Command:
    (--) Ok                                 -> Process-Command
          + Set-Prompt
          + Get-Command
    (--) None                               -> Process-Command
          + Set-Prompt
          + Get-Command
    (--) Exit                               ->
          + Terminate-The-Thread
    (--) Help                               -> Process-Command
          + Display-Help
          + Set-Prompt
          + Get-Command
    (--) Debug                              -> Process-Command
          + Set-Debug-Mode
          + Set-Prompt
          + Get-Command
    (--) Invalid                            -> Process-Command
          + Display-Invalid-Command
          + Set-Prompt
          + Get-Command
    (--) Recursive                          -> Process-Command
          + Set-Recursive-Mode
          + Set-Prompt
          + Get-Command
    (--) Server-Ip                          -> Process-Command
          + Set-Server-Ip-Value
          + Display-Dns-Server-Address
          + Set-Prompt
          + Get-Command
    (--) Request                            -> Send-Request
          + Get-Argument-Type

Send-Request:
    (--) Ip                                 -> After-Request
          + Request-Host-Name
    (--) Host-Name                          -> After-Request
          + Request-Ip-Address
    (--) Error                              -> Process-Command
          + Display-Error-Value
          + Set-Prompt
          + Get-Command

After-Request:
    (--) Host                               -> Process-Command
          + Display-Host-Name
    (--) Ip                                 -> Process-Command
          + Display-Ip-Address
    (--) Error                              -> Process-Command
          + Display-Error-Value
    (--) End                                -> Process-Command
          + Display-End-Of-Search
    (--) Timeout                            -> Process-Command
          + Display-Time-Out

Defaults:
    (--) Exception                          ->
          + Terminate-The-Thread
    (--) Shutdown                           ->
          + Terminate-The-Thread
    (--) Fatal                              ->
          + Terminate-The-Thread
