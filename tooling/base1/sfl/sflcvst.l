!----------------------------------------------------------------------------
!                                                                            *
!   sflcvst.l - Convert a string to a time                                   *
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
!----------------------------------------------------------------------------*/

After-Init:
    (--) Ok                                 -> Expect-Hour
          + Get-Next-Component

Expect-Hour:
    (--) Number                             -> Expect-Minute
          + Have-Hour
          + Get-Next-Component
    (--) Am-Pm                              -> Expect-Hour
          + Have-Am-Pm-Indicator
          + Get-Next-Component

Expect-Minute:
    (--) Number                             -> Expect-Second
          + Have-Minute
          + Get-Next-Component
    (--) Am-Pm                              -> Expect-Minute
          + Have-Am-Pm-Indicator
          + Get-Next-Component

Expect-Second:
    (--) Number                             -> Expect-Centisecond
          + Have-Second
          + Get-Next-Component
    (--) Am-Pm                              -> Expect-Second
          + Have-Am-Pm-Indicator
          + Get-Next-Component

Expect-Centisecond:
    (--) Number                             -> Allow-Am-Pm
          + Have-Centisecond
          + Get-Next-Component
    (--) Am-Pm                              -> Expect-Centisecond
          + Have-Am-Pm-Indicator
          + Get-Next-Component

Allow-Am-Pm:
    (--) Am-Pm                              -> Expect-Finished
          + Have-Am-Pm-Indicator
          + Get-Next-Component

Expect-Finished:
    (--) Finished                           ->
          + Have-Complete-Time
          + Terminate-The-Program

Defaults:
    (--) Number                             ->
          + Have-Invalid-Time
          + Terminate-The-Program
    (--) Am-Pm                              ->
          + Have-Invalid-Time
          + Terminate-The-Program
    (--) Finished                           ->
          + Have-Complete-Time
          + Terminate-The-Program
    (--) Delimiter                          ->
          + Have-Delimiter
          + Get-Next-Component
    (--) Error                              ->
          + Have-Invalid-Time
          + Terminate-The-Program
