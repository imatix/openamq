!============================================================================*
!                                                                            *
!   lrcalc.l - Expression calculator functions                               *
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
-source=lrcalc

After-Init:
    (--) Ok                                 -> Expecting-Initial
          + Get-Next-Token
    (--) Error                              ->
          + Terminate-The-Program

Expecting-Initial:
    (--) Term-Op                            ->
          + Allow-Signed-Number
          + Signal-Invalid-Token
          + Terminate-The-Program
    (--) Number                             -> Expecting-Operator
          + Stack-The-Number
          + Get-Next-Token
    (--) Left-Par                           -> Expecting-Operand
          + Stack-The-Operator
          + Get-Next-Token
    (--) End-Mark                           ->
          + Terminate-The-Program

Expecting-Operand:
    (--) Term-Op                            ->
          + Allow-Signed-Number
          + Signal-Invalid-Token
          + Terminate-The-Program
    (--) Number                             -> Expecting-Operator
          + Stack-The-Number
          + Get-Next-Token
    (--) Left-Par                           -> Expecting-Operand
          + Stack-The-Operator
          + Get-Next-Token

Expecting-Operator:
    (--) Term-Op                            -> Expecting-Operand
          + Unstack-Ge-Operators
          + Stack-The-Operator
          + Get-Next-Token
    (--) Factor-Op                          -> Expecting-Operand
          + Unstack-Ge-Operators
          + Stack-The-Operator
          + Get-Next-Token
    (--) End-Mark                           ->
          + Unstack-All-Operators
          + Unstack-If-End-Mark
          + Terminate-The-Program
    (--) Right-Par                          -> Expecting-Operator
          + Unstack-All-Operators
          + Unstack-If-Left-Par
          + Get-Next-Token

Defaults:
    (--) Number                             ->
          + Signal-Invalid-Token
          + Terminate-The-Program
    (--) Term-Op                            ->
          + Signal-Invalid-Token
          + Terminate-The-Program
    (--) Factor-Op                          ->
          + Signal-Invalid-Token
          + Terminate-The-Program
    (--) End-Mark                           ->
          + Signal-Token-Missing
          + Terminate-The-Program
    (--) Left-Par                           ->
          + Signal-Invalid-Token
          + Terminate-The-Program
    (--) Right-Par                          ->
          + Signal-Invalid-Token
          + Terminate-The-Program
    (--) Exception                          ->
          + Terminate-The-Program
