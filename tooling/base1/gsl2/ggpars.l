!----------------------------------------------------------------------------*
!                                                                            *
!   ggpars.l - Script parser                                                 *
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

!- ggpars can be called in 3 ways: gg_parse_template, gg_parse_gsl and
!- gg_parse_expression

After-Init:
    (--) Template                           -> Allow-Text
          + Read-Script-Line
          + Generate-Script-Exception
          + Get-Template-Token
    (--) GSL                                -> Expect-Script
          + Read-Script-Line
          + Generate-Script-Exception
          + Generate-Comment-Exception
          + Generate-Text-Exception
          + Get-Script-Command-Token
    (--) Expression                         -> Allow-Operand
          + Push-Expression-State
          + Get-Expression-Token
    (--) Script                             -> Expect-Script
          + Generate-Comment-Exception
          + Get-Script-Command-Token
    (--) Text                               -> Allow-Text
          + Get-Template-Token
    (--) Comment                            ->
          + Insert-Comment-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) End-Of-File                        ->
          + Signal-Unexpected-End-Of-File
          + Rollback-Memory-Allocations
          + Terminate-The-Program

!- Dialog state while parsing text for direct output

Allow-Text:
    (--) Text                               ->
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Template-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Text-State
          + Push-Substitute-State
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Extend                             ->
          + Insert-Spaces-Only-Node
          + Set-Extend-Flag
          + Return-To-Root-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) End-Of-Line                        ->
          + Insert-Spaces-Only-Node
          + Return-To-Root-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program

Expect-Text:
    (--) Text                               -> Allow-Text
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Template-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Text-State
          + Push-Substitute-State
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token

!- Expression dialog states.  Pops state off stack when done.

Expect-Operand:
    (--) Quote                              -> Continue-Quoted
          + Push-Quote-Character
          + Push-Operand-State
          + Insert-Literal-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op1-Field
          + Get-Quoted-Token
    (--) Sign                               -> After-Sign
          + Save-The-Sign
          + Count-Token-Length
          + Get-Expression-Token
    (--) Unary-Operator                     -> Expect-Operand
          + Insert-Unary-Operator-Node
          + Count-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Number                             -> After-Number  
          + Push-Operand-State
          + Insert-Number-Node
          + Ready-For-Op1-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Point                              -> After-Point 
          + Push-Operand-State
          + Count-Token-Length
          + Get-Expression-Token
    (--) Literal                            -> Continue-Identifier
          + Push-Simple-Operand-State
          + Push-Continue-Scope-Or-Attribute-State
          + Insert-Symbol-Node
          + Ready-For-Op1-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Open-Bracket                       -> Expect-Bracketed-Identifier
          + Push-Simple-Operand-State
          + Push-Continue-Scope-Or-Attribute-State
          + Insert-Symbol-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Bracketed-Identifier-Token
    (--) Member                             -> Expect-Identifier
          + Push-Operand-State
          + Push-Continue-Attribute-State
          + Push-Continue-Scope-State
          + Insert-Symbol-Node
          + Ready-For-Op1-Field
          + Insert-Member-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Simple-Operand-State
          + Push-Continue-Scope-Or-Attribute-State
          + Insert-Unknown-Operand-Node
          + Ready-For-Op1-Field
          + Push-Substitute-State
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Open                               -> Expect-Operand
          + Add-One-Bracket
          + Count-Token-Length
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

After-Sign:
    (--) Number                             -> After-Number
          + Push-Operand-State
          + Insert-Number-Node
          + Copy-The-Operator
          + Ready-For-Op1-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Point                              -> After-Sign-Point
          + Push-Operand-State
          + Count-Token-Length
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Expect-Operand
          + Restore-The-Sign
          + Insert-Unary-Operator-Node
          + Ready-For-Op2-Field
          + Rewind-To-Same-Token
          + Get-Expression-Token
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error                              ->
          + Rollback-Memory-Allocations
          + Terminate-The-Program

After-Sign-Point:
    (--) Number                             -> Returning
          + Insert-Number-Node
          + Copy-The-Operator
          + Ready-For-Op2-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Return-To-Operand
          + Pop-Previous-State
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> After-Point 
          + Insert-Unary-Operator-Node
          + Ready-For-Op2-Field
          + Push-Operand-State
          + Rewind-To-Same-Token
          + Get-Expression-Token
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error                              ->
          + Rollback-Memory-Allocations
          + Terminate-The-Program

After-Point:
    (--) Number                             -> Returning
          + Insert-Number-Node
          + Ready-For-Op2-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Return-To-Operand
          + Pop-Previous-State
    (--) Literal                            -> Continue-Identifier
          + Insert-Symbol-Node
          + Ready-For-Op1-Field
          + Insert-Empty-Text-Node
          + Return-One-More
          + Ready-For-Op2-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Identifier-State
          + Push-Substitute-State
          + Insert-Symbol-Node
          + Ready-For-Op1-Field
          + Insert-Empty-Text-Node
          + Return-One-More
          + Ready-For-Op2-Field
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Open-Bracket                       -> Expect-Bracketed-Identifier
          + Insert-Symbol-Node
          + Ready-For-Op1-Field
          + Insert-Empty-Text-Node
          + Return-One-More
          + Ready-For-Op2-Field
          + Count-Token-Length
          + Get-Bracketed-Identifier-Token
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Insert-Symbol-Node
          + Ready-For-Op1-Field
          + Insert-Empty-Text-Node
          + Return-One-More
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error                              ->
          + Rollback-Memory-Allocations
          + Terminate-The-Program

After-Number:
    (--) Point                              -> After-Number-Point
          + Return-One-More
          + Count-Spaces-And-Token-Length
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Operand
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

After-Number-Point:
    (--) Number                             -> Returning
          + Ready-For-Op2-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Return-To-Operand
          + Pop-Previous-State
    (--) Literal                            -> Continue-Identifier
          + Change-Operand-To-Symbol-Node
          + Ready-For-Op2-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Identifier-State
          + Push-Substitute-State
          + Change-Operand-To-Symbol-Node
          + Ready-For-Op2-Field
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Open-Bracket                       -> Expect-Bracketed-Identifier
          + Change-Operand-To-Symbol-Node
          + Ready-For-Op2-Field
          + Count-Token-Length
          + Get-Bracketed-Identifier-Token
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Operand
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error                              ->
          + Rollback-Memory-Allocations
          + Terminate-The-Program

Allow-Operand: <Expect-Operand>
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Expression
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error                              ->
          + Rollback-Memory-Allocations
          + Terminate-The-Program

!- State to skip multi-line comments

Continue-Comment:
    (--) Close-Comment                      ->
          + Dialog-Return
    (--) End-Of-Line                        ->
          + Read-Script-Line
          + Get-Comment-Token
    (--) Text                               ->
          + Get-Comment-Token

!- Quoted string parsing state.  Pops state off stack when done.

Continue-Quoted:
    (--) Text                               ->
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Quoted-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Quoted-State
          + Push-Substitute-State
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Quote                              -> Returning
          + Insert-Spaces-Only-Node
          + Return-To-Operand
          + Return-One-More
          + Count-Spaces-And-Token-Length
          + Pop-Previous-State
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Quoted-Token
    (--) End-Of-Line                        ->
          + Insert-Spaces-Only-Node
          + Set-Line-Break-Flag
          + Read-Script-Line
          + Get-Quoted-Token

Expect-Number:
    (--) Number                             -> Returning
          + Insert-Number-Node
          + Ready-For-Op1-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Return-To-Operand
          + Pop-Previous-State
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

Allow-Number: <Expect-Number>
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Operand
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

Expect-Signed-Number:
    (--) Number                             -> Returning
          + Insert-Text-Node
          + Copy-The-Operator
          + Count-Token-Length
          + Return-To-Operand
          + Pop-Previous-State
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

!- Identifier parsing states.  Pops state off stack when done.

Identifier-Superstate:
    (--) Literal                            -> Continue-Identifier
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Identifier-State
          + Push-Substitute-State
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token

Expect-Identifier: <Identifier-Superstate>
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Bracket                       -> Expect-Bracketed-Identifier
          + Count-Spaces-And-Token-Length
          + Get-Bracketed-Identifier-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

Allow-Identifier: <Expect-Identifier>
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Operand
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

Continue-Identifier: <Identifier-Superstate>
    (--) Number                             -> Continue-Identifier
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Operand
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

Expect-Bracketed-Identifier:
    (--) Literal                            -> Continue-Bracketed-Identifier
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Bracketed-Identifier-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Bracketed-Identifier-State
          + Push-Substitute-State
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token

Continue-Bracketed-Identifier: <Expect-Bracketed-Identifier>
    (--) Close-Bracket                      -> Returning
          + Count-Token-Length
          + Return-To-Operand
          + Pop-Previous-State

!- Scope parsing.

Scope-Superstate:
    (--) Member                             -> Expect-Identifier
          + Return-To-Scope
          + Push-Continue-Scope-State
          + Insert-Member-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

Expect-Scope: <Scope-Superstate>
    (--) Point                              -> Returning
          + Insert-Empty-Text-Node
          + Return-To-Scope
          + Pop-Previous-State
    (--) Open-Bracket                       -> Expect-Bracketed-Identifier
          + Push-Continue-Scope-State
          + Count-Token-Length
          + Get-Bracketed-Identifier-Token
    (--) Literal                            -> Continue-Identifier
          + Push-Continue-Scope-State
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Number                             -> Continue-Scope
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Sign                               -> Expect-Signed-Number
          + Push-Continue-Scope-State
          + Save-The-Sign
          + Count-Token-Length
          + Get-Expression-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Continue-Scope-State
          + Push-Substitute-State
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token

Allow-Scope: <Expect-Scope>
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Scope
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

Continue-Scope: <Scope-Superstate>
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Scope
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

!- Combined scope/attribute handling

Scope-Attribute-Superstate:
    (--) Member                             -> Expect-Identifier
          + Push-Continue-Attribute-State
          + Push-Continue-Scope-State
          + Insert-Member-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

Continue-Scope-Or-Attribute: <Scope-Attribute-Superstate>
    (--) Point                              -> Allow-Identifier
          + Return-To-Scope
          + Return-One-More
          + Change-Operand-To-Symbol-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Scope
          + Return-One-More
          + Move-Op1-To-Op2
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

Expect-Scope-Attribute: <Scope-Attribute-Superstate>
    (--) Point                              -> Allow-Identifier
          + Insert-Empty-Text-Node
          + Return-One-More
          + Count-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Open-Bracket                       -> Expect-Bracketed-Identifier
          + Push-Continue-Scope-Or-Attribute-State
          + Count-Token-Length
          + Get-Bracketed-Identifier-Token
    (--) Literal                            -> Continue-Identifier
          + Push-Continue-Scope-Or-Attribute-State
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Continue-Scope-Or-Attribute-State
          + Push-Substitute-State
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Number                             -> Continue-Scope-Or-Attribute
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Sign                               -> Expect-Signed-Number
          + Push-Continue-Scope-Or-Attribute-State
          + Save-The-Sign
          + Count-Token-Length
          + Get-Expression-Token

!- Attribute parsing

Continue-Attribute:
    (--) Point                              -> Allow-Identifier
          + Return-To-Scope
          + Return-One-More
          + Change-Operand-To-Symbol-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Scope
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

!- Operator handling

Expect-Operator:
    (--) Unary-Operator                     -> Expect-Operand
          + Insert-Operator-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Sign                               -> Expect-Operand
          + Insert-Operator-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Operator                           -> Expect-Operand
          + Insert-Operator-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Default-Operator                   -> Allow-Operand
          + Insert-Operator-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Close                              -> Expect-Operator
          + Close-One-Bracket
          + Count-Spaces-And-Token-Length
          + Get-Expression-Token
    (--) No-Bracket                         -> Returning
          + Return-To-Expression
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Return-To-Expression
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

!- Pick up function calls by '(' following operand

After-Simple-Operand: <Expect-Operator>
    (--) Open                               -> Allow-Operand
          + Change-To-Call-Node
          + Push-Call-State
          + Push-Arguments-State
          + Ready-For-Op3-Field
          + Count-Token-Length
          + Get-Expression-Token

!- State to pick up arguments for functions and macro invocations

Expect-Arguments:
    (--) Next-Arg                           -> Allow-Operand
          + Return-To-Arguments
          + Insert-Operator-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Push-Arguments-State
          + Get-Expression-Token
    (--) Close                              -> Returning
          + Count-Spaces-And-Token-Length
          + Return-To-Arguments
          + Pop-Previous-State
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

!- Dialog to handle modifiers and end of substitution token

Expect-Substitute:
    (--) Modifier                           -> Expect-Modifier
          + Count-Spaces-And-Token-Length
          + Ready-For-Modifier-Field
          + Get-Modifier-Token
    (--) Close                              -> Returning
          + Count-Spaces-And-Token-Length
          + Pop-Previous-State
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Modifier-Token

Expect-Modifier:
    (--) Text                               ->
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Modifier-Token
    (--) Substitute                         -> Expect-Operand
          + Confirm-Modifier-Specified
          + Push-Modifier-State
          + Push-Substitute-State
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Modifier-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Expect-Substitute
          + Insert-Empty-Node-If-Needed
          + Return-To-Operand
          + Return-One-More
          + Rewind-To-Same-Token
          + Get-Modifier-Token
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

!- Handle script statements

Expect-Script:
    (--) Comment                            -> 
          + Insert-Comment-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) End-Of-Line                        ->
          + Insert-Comment-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Close                              ->
          + Insert-Close-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Else                               ->
          + Insert-Else-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) End-If                             ->
          + Insert-End-If-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) End-For                            ->
          + Insert-End-For-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) End-Macro                          ->
          + Insert-End-Macro-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) End-Function                       ->
          + Insert-End-Function-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) End-New                            ->
          + Insert-End-New-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) End-While                          ->
          + Insert-End-While-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Output                             -> Expect-Operand
          + Insert-Output-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Append                             -> Expect-Operand
          + Insert-Append-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Echo                               -> Expect-Operand
          + Insert-Echo-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Abort                              -> Allow-Operand
          + Insert-Abort-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Define                             -> Expect-Scope-Attribute
          + Push-Define-State
          + Insert-Define-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
!- Literals/Substitute may be either implicit .invoke or .define
    (--) Literal                            -> Continue-Identifier
          + Push-Implicit-Assign-Or-Invoke-State
          + Push-Continue-Scope-Or-Attribute-State
          + Insert-Define-Node
          + Ready-For-Op1-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Open-Bracket                       -> Expect-Bracketed-Identifier
          + Push-Implicit-Assign-Or-Invoke-State
          + Push-Continue-Scope-Or-Attribute-State
          + Insert-Define-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Bracketed-Identifier-Token
    (--) Substitute                         -> Expect-Operand
          + Push-Implicit-Assign-Or-Invoke-State
          + Push-Continue-Scope-Or-Attribute-State
          + Insert-Define-Node
          + Ready-For-Op1-Field
          + Push-Substitute-State
          + Insert-Substitute-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Get-Expression-Token
!- Point, Member, Number and Sign introduce implicit .define
    (--) Point                              -> Allow-Identifier
          + Push-Define-State
          + Insert-Define-Node
          + Count-Token-Length
          + Ready-For-Op1-Field
          + Insert-Empty-Text-Node
          + Return-One-More
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Member                             -> Expect-Identifier
          + Push-Define-State
          + Push-Continue-Attribute-State
          + Push-Continue-Scope-State
          + Insert-Define-Node
          + Ready-For-Op1-Field
          + Insert-Member-Node
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Number                             -> Continue-Scope-Or-Attribute
          + Push-Define-State
          + Insert-Define-Node
          + Ready-For-Op1-Field
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Sign                               -> Expect-Signed-Number
          + Push-Define-State
          + Push-Continue-Scope-Or-Attribute-State
          + Insert-Define-Node
          + Ready-For-Op1-Field
          + Save-The-Sign
          + Count-Token-Length
          + Get-Expression-Token
    (--) Macro                              -> Expect-Identifier
          + Push-Macro-State
          + Insert-Macro-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Function                           -> Expect-Identifier
          + Push-Macro-State
          + Insert-Function-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Return                             -> Allow-Operand
          + Insert-Return-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Invoke                             -> Expect-Identifier
          + Push-Invoke-State
          + Insert-Call-Node
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Include                            -> Expect-Operand
          + Insert-GSL-Node
          + Ready-For-Op1-Field
          + Insert-Source-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Gsl                                -> Expect-Source
          + Insert-Gsl-Node
          + Ready-For-Op1-Field
          + Get-Script-Token
    (--) Direct                             -> Expect-Source
          + Insert-Direct-Node
          + Ready-For-Op1-Field
          + Get-Script-Token
    (--) Javascript                         -> Expect-Source
          + Insert-Javascript-Node
          + Ready-For-Op1-Field
          + Get-Script-Token
    (--) XML                                -> Expect-Locator
          + Push-XML-State
          + Insert-XML-Node
          + Get-Script-Token
    (--) Template                           -> Allow-Number
          + Insert-Template-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) End-Template                       ->
          + Insert-End-Template-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Save                               -> Expect-Scope
          + Push-Save-State
          + Insert-Save-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) If                                 -> Expect-Operand
          + Insert-If-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Elsif                              -> Expect-Operand
          + Insert-Elsif-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) New                                -> Expect-Scope
          + Push-Locator-State
          + Insert-New-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Move                               -> Allow-Scope
          + Push-Locator-State
          + Insert-Move-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Copy                               -> Allow-Scope
          + Push-Locator-State
          + Insert-Copy-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Load                               -> Expect-Operand
          + Push-Load-State
          + Insert-XML-Node
          + Ready-For-Op1-Field
          + Insert-Source-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Delete                             -> Allow-Scope
          + Insert-Delete-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) For                                -> Expect-Scope-Attribute
          + Push-For-State
          + Insert-For-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) While                              -> Expect-Operand
          + Insert-While-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Next                               ->
          + Insert-Next-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Last                               ->
          + Insert-Last-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Sort                               -> Expect-Scope-Attribute
          + Push-Sort-State
          + Insert-Sort-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Script-Command-Token

!- Handle implicit function/macro calls and assignments

Expect-Assign-Or-Args:
    (--) Assign                             -> Allow-Operand
          + Copy-The-Operator
          + Count-Spaces-And-Token-Length
          + Ready-For-Op3-Field
          + Get-Expression-Token
    (--) Open                               -> Allow-Operand
          + Change-To-Call-Node
          + Push-Arguments-State
          + Ready-For-Op3-Field
          + Count-Token-Length
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Script-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Script-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Script-Token

!- After identifier in .define statement

Expect-Assign:
    (--) Assign                             -> Allow-Operand
          + Copy-The-Operator
          + Count-Spaces-And-Token-Length
          + Ready-For-Op3-Field
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Script-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Script-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Script-Token

After-Macro:
    (--) Open                               -> Allow-Macro-Args
          + Count-Spaces-And-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) End-Of-Line                        ->
          + Return-To-Root-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

Allow-Macro-Args:
    (--) Close                              -> Expect-End-Of-Line
          + Count-Spaces-And-Token-Length
          + Return-To-Root-Node
          + Get-Expression-Token
    (--) Literal                            -> Expect-Macro-Args
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

Expect-Macro-Args:
    (--) Next-Arg                           -> Expect-Argument
          + Insert-Operator-Node
          + Count-Token-Length
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Close                              -> Returning
          + Count-Spaces-And-Token-Length
          + Return-To-Arguments
          + Pop-Previous-State
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

Expect-Argument:
    (--) Literal                            -> Expect-Macro-Args
          + Insert-Text-Node
          + Count-Token-Length
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

!- After identifier in .invoke statement

Expect-Invoke-Args:
    (--) Open                               -> Allow-Operand
          + Push-Arguments-State
          + Ready-For-Op3-Field
          + Count-Spaces-And-Token-Length
          + Get-Expression-Token
    (--) End-Of-Line                        ->
          + Return-To-Root-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

!- Specify where to copy, move, load or insert XML

Expect-Locator:
    (--) To                                 -> Expect-Scope
          + Push-Locator-State
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) Before                             -> Expect-Scope
          + Push-Locator-State
          + Ready-For-Pretty-Field
          + Get-Expression-Token
    (--) After                              -> Expect-Scope
          + Push-Locator-State
          + Ready-For-Format-Field
          + Get-Expression-Token
    (--) As                                 -> Expect-Identifier
          + Push-Locator-State
          + Ready-For-Op3-Field
          + Get-Expression-Token
    (--) Spaces                             ->
          + Get-Script-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Script-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Script-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Returning
          + Rewind-To-Same-Token
          + Pop-Previous-State
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program

!- Specify where to load from

Expect-Source:
    (--) From                               -> Expect-Operand
          + Insert-Source-Node
          + Ready-For-Op1-Field
          + Get-Expression-Token
    (--) Incoming                           -> Expect-Text
          + Insert-Source-Node
          + Ready-For-Op2-Field
          + Get-Template-Token
    (--) Spaces                             ->
          + Get-Script-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Script-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Script-Token
!- All states which have event $other must also define events Anomaly and Error
    (--) $other                             -> Expect-Operand
          + Rewind-To-Same-Token
          + Get-Expression-Token
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error                              ->
          + Rollback-Memory-Allocations
          + Terminate-The-Program

!- After scope in .save statement

Expect-Save:
    (--) As                                 -> Allow-Operand
          + Ready-For-Op2-Field
          + Get-Expression-Token
    (--) End-Of-Line                        ->
          + Return-To-Root-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Spaces                             ->
          + Get-Script-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Script-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Script-Token

!- After identifier and scope in .new statement

Expect-New:
    (--) As                                 -> Expect-Identifier
          + Push-New-State
          + Ready-For-Op3-Field
          + Get-Expression-Token
    (--) Before                             -> Expect-Identifier
          + Push-New-State
          + Ready-For-Pretty-Field
          + Get-Expression-Token
    (--) After                              -> Expect-Identifier
          + Push-New-State
          + Ready-For-Format-Field
          + Get-Expression-Token
    (--) End-Of-Line                        ->
          + Return-To-Root-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Spaces                             ->
          + Get-Script-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Script-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Script-Token

!- After identifier and scope in .for statement

Expect-For:
    (--) As                                 -> Expect-Identifier
          + Push-For-State
          + Ready-For-Op3-Field
          + Get-Expression-Token
    (--) Where                              -> Allow-Operand
          + Push-For-State
          + Ready-For-Pretty-Field
          + Get-Expression-Token
    (--) By                                 -> Allow-Operand
          + Push-For-State
          + Ready-For-Format-Field
          + Get-Expression-Token
    (--) End-Of-Line                        ->
          + Return-To-Root-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Spaces                             ->
          + Get-Script-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Script-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Script-Token

!- After identifier and scope in .sort statement

Expect-Sort:
    (--) As                                 -> Expect-Identifier
          + Push-Sort-State
          + Ready-For-Op3-Field
          + Get-Expression-Token
    (--) By                                 -> Allow-Operand
          + Push-Sort-State
          + Ready-For-Format-Field
          + Get-Expression-Token
    (--) End-Of-Line                        ->
          + Return-To-Root-Node
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Spaces                             ->
          + Get-Script-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Script-Token
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Script-Token

Expect-End-Of-Line:
    (--) Comment                            ->
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Spaces                             ->
          + Get-Expression-Token
    (--) Extend                             ->
          + Read-Script-Line
          + Get-Expression-Token
    (--) End-Of-Line                        ->
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Open-Comment                       -> Continue-Comment
          + Get-Comment-Token
          + Dialog-Call
    (--) Return                             ->
          + Get-Expression-Token

 - Dialog state to handling flow after popping state from stack.

Returning:
    (--) Expression                         ->
          + Expect-End-Of-Line
          + Return-To-Root-Node
          + Error-If-Empty-Expression
          + Commit-Memory-Allocations
          + Terminate-The-Program
    (--) Text                               -> Allow-Text
          + Get-Template-Token
    (--) Substitute                         -> Expect-Substitute
          + Return-One-More
          + Get-Modifier-Token
    (--) Operand                            -> Expect-Operator
          + Return-To-Operator
          + Get-Expression-Token
    (--) Simple-Operand                     -> After-Simple-Operand
          + Return-To-Operator
          + Get-Expression-Token
    (--) Continue-Scope-Or-Attribute        -> Continue-Scope-Or-Attribute
          + Get-Expression-Token
    (--) Continue-Scope                     -> Continue-Scope
          + Get-Expression-Token
    (--) Continue-Attribute                 -> Continue-Attribute
          + Get-Expression-Token
    (--) Bracketed-Identifier               -> Expect-Bracketed-Identifier
          + Get-Bracketed-Identifier-Token
    (--) Identifier                         -> Continue-Identifier
          + Get-Expression-Token
    (--) Quoted                             -> Continue-Quoted
          + Get-Quoted-Token
    (--) Implicit-Assign-Or-Invoke          -> Expect-Assign-Or-Args
          + Return-One-More
          + Get-Script-Token
    (--) Call                               -> Expect-Operator
          + Return-One-More
          + Get-Expression-Token
    (--) Arguments                          -> Expect-Arguments
          + Get-Expression-Token
    (--) Modifier                           -> Expect-Modifier
          + Get-Modifier-Token
    (--) Define                             -> Expect-Assign
          + Return-One-More
          + Get-Script-Token
    (--) Macro                              -> After-Macro
          + Return-One-More
          + Get-Expression-Token
    (--) Macro-Args                         -> Expect-Macro-Args
          + Get-Expression-Token
    (--) Invoke                             -> Expect-Invoke-Args
          + Return-One-More
          + Get-Expression-Token
    (--) Locator                            -> Expect-Locator
          + Return-One-More
          + Get-Script-Token
    (--) Load                               -> Expect-Locator
          + Return-One-More
          + Return-One-More
          + Get-Script-Token
    (--) XML                                -> Expect-Source
          + Ready-For-Op1-Field
          + Get-Script-Token
    (--) Save                               -> Expect-Save
          + Return-One-More
          + Get-Script-Token
    (--) New                                -> Expect-New
          + Return-One-More
          + Get-Script-Token
    (--) For                                -> Expect-For
          + Return-One-More
          + Get-Script-Token
    (--) Sort                               -> Expect-Sort
          + Return-One-More
          + Get-Script-Token
    (--) Empty-Stack                        -> Expect-End-Of-Line
          + Return-To-Root-Node
          + Get-Expression-Token

Defaults:
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Error
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) End-Of-File                        ->
          + Signal-Unexpected-End-Of-File
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) Other                              ->
          + Signal-Invalid-Token
          + Rollback-Memory-Allocations
          + Terminate-The-Program
    (--) $other                             ->
          + Signal-Invalid-Token
          + Rollback-Memory-Allocations
          + Terminate-The-Program
