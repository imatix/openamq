!============================================================================*
!                                                                            *
!   lreval.l - Logic evaluator functions                                     *
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
-source=lreval

After-Init:
    (--) Ok                                 -> Expect-Action
          + Get-Next-Token
    (--) Error                              ->
          + Terminate-The-Program

Expect-Action:
    (--) Normal                             -> Have-Action
          + Classify-Token-As-Action
    (--) String                             -> Left-String-Next
          + Init-Left-And-Right-Expr
          + Save-Left-Expr-Position
          + Concat-Token-To-Left-Expr
          + Get-Next-Token
    (--) Relator                            ->
          + Signal-Action-Expected
          + Terminate-The-Program
    (--) Finished                           ->
          + Signal-Action-Expected
          + Terminate-The-Program

Have-Action:
    (--) Not                                -> Expect-Action
          + Invert-Condition-Result
          + Get-Next-Token
    (--) Animate                            ->
          + Evaluate-Animation-Enabled
          + Terminate-The-Program
    (--) Caps                               ->
          + Evaluate-Style-Is-Caps
          + Terminate-The-Program
    (--) Cobol                              ->
          + Evaluate-Style-Is-Cobol
          + Terminate-The-Program
    (--) Check                              ->
          + Evaluate-Check-Enabled
          + Terminate-The-Program
    (--) Defaults                           ->
          + Evaluate-Defaults-Defined
          + Terminate-The-Program
    (--) Exist                              ->
          + Get-Next-Token
          + Evaluate-File-Exists
          + Terminate-The-Program
    (--) Headline                           ->
          + Evaluate-Style-Is-Headline
          + Terminate-The-Program
    (--) Plain                              ->
          + Evaluate-Style-Is-Plain
          + Terminate-The-Program
    (--) Normal                             ->
          + Evaluate-Style-Is-Normal
          + Terminate-The-Program
    (--) Title                              ->
          + Evaluate-Style-Is-Title
          + Terminate-The-Program
    (--) State                              ->
          + Get-Next-Token
          + Evaluate-State-Exists
          + Terminate-The-Program
    (--) Event                              ->
          + Get-Next-Token
          + Evaluate-Event-Exists
          + Terminate-The-Program
    (--) Module                             ->
          + Get-Next-Token
          + Evaluate-Module-Exists
          + Terminate-The-Program
    (--) Test                               -> Left-Expr-First
          + Init-Left-And-Right-Expr
          + Get-Next-Token
    (--) Other                              -> Left-Expr-Next
          + Init-Left-And-Right-Expr
          + Save-Left-Expr-Position
          + Concat-Token-To-Left-Expr
          + Get-Next-Token

Left-Expr-First:
    (--) Normal                             -> Left-Expr-Next
          + Save-Left-Expr-Position
          + Concat-Token-To-Left-Expr
          + Get-Next-Token
    (--) String                             -> Left-String-Next
          + Save-Left-Expr-Position
          + Concat-Token-To-Left-Expr
          + Get-Next-Token
    (--) Relator                            ->
          + Signal-Expr-Or-String-Expected
          + Terminate-The-Program
    (--) Finished                           ->
          + Signal-Expr-Or-String-Expected
          + Terminate-The-Program

Left-Expr-Next:
    (--) Normal                             -> Left-Expr-Next
          + Concat-Token-To-Left-Expr
          + Get-Next-Token
    (--) String                             ->
          + Signal-Relator-Expected
          + Terminate-The-Program
    (--) Relator                            -> Right-Expr-First
          + Get-Next-Token
    (--) Finished                           ->
          + Signal-Relator-Expected
          + Terminate-The-Program

Right-Expr-First:
    (--) Normal                             -> Right-Expr-Next
          + Save-Right-Expr-Position
          + Concat-Token-To-Right-Expr
          + Get-Next-Token
    (--) String                             ->
          + Signal-Expression-Expected
          + Terminate-The-Program
    (--) Relator                            ->
          + Signal-Relator-Ignored
          + Terminate-The-Program
    (--) Finished                           ->
          + Signal-Expression-Expected
          + Terminate-The-Program

Right-Expr-Next:
    (--) Normal                             -> Right-Expr-Next
          + Concat-Token-To-Right-Expr
          + Get-Next-Token
    (--) String                             ->
          + Signal-Expression-Expected
          + Terminate-The-Program
    (--) Relator                            ->
          + Signal-Relator-Ignored
          + Terminate-The-Program
    (--) Finished                           ->
          + Evaluate-Numeric-Comparison
          + Terminate-The-Program

Left-String-Next:
    (--) Normal                             ->
          + Signal-String-Expected
          + Terminate-The-Program
    (--) String                             -> Left-String-Next
          + Concat-Token-To-Left-Expr
          + Get-Next-Token
    (--) Relator                            -> Right-String-First
          + Get-Next-Token
    (--) Finished                           ->
          + Signal-Relator-Expected
          + Terminate-The-Program

Right-String-First:
    (--) Normal                             ->
          + Signal-String-Expected
          + Terminate-The-Program
    (--) String                             -> Right-String-Next
          + Save-Right-Expr-Position
          + Concat-Token-To-Right-Expr
          + Get-Next-Token
    (--) Relator                            ->
          + Signal-Relator-Ignored
          + Terminate-The-Program
    (--) Finished                           ->
          + Signal-String-Expected
          + Terminate-The-Program

Right-String-Next:
    (--) Normal                             ->
          + Signal-String-Expected
          + Terminate-The-Program
    (--) String                             -> Right-String-Next
          + Concat-Token-To-Right-Expr
          + Get-Next-Token
    (--) Relator                            ->
          + Signal-Relator-Ignored
          + Terminate-The-Program
    (--) Finished                           ->
          + Evaluate-String-Comparison
          + Terminate-The-Program

Defaults:
    (--) Finished                           ->
          + Signal-Token-Missing
          + Terminate-The-Program
    (--) Exception                          ->
          + Terminate-The-Program
