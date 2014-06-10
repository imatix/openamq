!----------------------------------------------------------------------------
!                                                                            *
!   sflsearch.l - search criteria parser                                     *
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
!----------------------------------------------------------------------------*/

After-Init:
    (--) Ok                                 -> Before-Token
          + Use-Method-Or
          + Use-Scope-Equal
          + Get-Next-Character

Before-Token:
    (--) Alpha                              -> Alpha-Token
          + Start-New-Token
          + Append-To-Token
          + Get-Next-Character
    (--) Digit                              -> Numeric-Token
          + Start-New-Token
          + Append-To-Token
          + Get-Next-Character
    (--) Plus                               -> Before-Token
          + Use-Method-And
          + Get-Next-Character
    (--) Minus                              -> Before-Token
          + Use-Method-Not
          + Get-Next-Character
    (--) Range                              -> Before-Token
          + Use-Scope-Greater-Equal
          + Get-Next-Character
    (--) Comma                              -> Before-Token
          + Get-Next-Character
    (--) Decimal                            -> Before-Token
          + Get-Next-Character
    (--) Delimiter                          -> Before-Token
          + Get-Next-Character
    (--) Finished                           ->
          + Terminate-The-Program

Alpha-Token:
    (--) Alpha                              -> Alpha-Token
          + Append-To-Token
          + Get-Next-Character
    (--) Digit                              -> Numeric-Token
          + Store-Alpha-Token
          + Use-Method-Or
          + Use-Scope-Equal
          + Start-New-Token
          + Append-To-Token
          + Get-Next-Character
    (--) Plus                               -> Before-Token
          + Store-Alpha-Token
          + Use-Method-And
          + Use-Scope-Equal
          + Get-Next-Character
    (--) Minus                              -> Before-Token
          + Store-Alpha-Token
          + Use-Method-Not
          + Use-Scope-Equal
          + Get-Next-Character
    (--) Range                              -> After-Range
          + Store-Alpha-Token
          + Use-Scope-Less-Equal
          + Get-Next-Character
    (--) Comma                              -> Before-Token
          + Store-Alpha-Token
          + Get-Next-Character
    (--) Decimal                            -> Before-Token
          + Store-Alpha-Token
          + Get-Next-Character
    (--) Delimiter                          -> Before-Token
          + Store-Alpha-Token
          + Use-Method-Or
          + Use-Scope-Equal
          + Get-Next-Character
    (--) Finished                           ->
          + Store-Alpha-Token
          + Terminate-The-Program

Numeric-Token:
    (--) Alpha                              -> Alpha-Token
          + Store-Numeric-Token
          + Start-New-Token
          + Append-To-Token
          + Get-Next-Character
    (--) Digit                              -> Numeric-Token
          + Append-To-Token
          + Get-Next-Character
    (--) Plus                               -> Before-Token
          + Store-Numeric-Token
          + Use-Method-And
          + Use-Scope-Equal
          + Get-Next-Character
    (--) Minus                              -> Before-Token
          + Store-Numeric-Token
          + Use-Method-Not
          + Use-Scope-Equal
          + Get-Next-Character
    (--) Range                              -> After-Range
          + Store-Numeric-Token
          + Use-Scope-Less-Equal
          + Get-Next-Character
    (--) Comma                              -> Numeric-Token
          + Get-Next-Character
    (--) Decimal                            -> Skipping-Decimal
          + Get-Next-Character
    (--) Delimiter                          -> Before-Token
          + Store-Numeric-Token
          + Use-Method-Or
          + Use-Scope-Equal
          + Get-Next-Character
    (--) Finished                           ->
          + Store-Numeric-Token
          + Terminate-The-Program

Skipping-Decimal: <Numeric-Token>
    (--) Digit                              -> Skipping-Decimal
          + Get-Next-Character
    (--) Decimal                            -> Skipping-Decimal
          + Get-Next-Character

After-Range:
    (--) Alpha                              -> Alpha-Token
          + Use-Scope-From-To
          + Start-New-Token
          + Append-To-Token
          + Get-Next-Character
    (--) Digit                              -> Numeric-Token
          + Use-Scope-From-To
          + Start-New-Token
          + Append-To-Token
          + Get-Next-Character
    (--) Plus                               -> Before-Token
          + Use-Method-And
          + Use-Scope-Equal
          + Get-Next-Character
    (--) Minus                              -> Before-Token
          + Use-Method-Not
          + Use-Scope-Equal
          + Get-Next-Character
    (--) Range                              -> After-Range
          + Get-Next-Character
    (--) Comma                              -> Before-Token
          + Get-Next-Character
    (--) Decimal                            -> Before-Token
          + Get-Next-Character
    (--) Delimiter                          -> Before-Token
          + Get-Next-Character
    (--) Finished                           ->
          + Terminate-The-Program

