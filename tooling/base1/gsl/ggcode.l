!----------------------------------------------------------------------------*
!                                                                            *
!   ggcode.l - Code generator functions                                      *
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
-nosort
-idle

After-Init:
    (--) Execute                            -> Continuing
          + Reset-Error-Occurrence
          + Call-Load-Initial-Script
          + Raise-Exception-If-Error-Occurred
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event
    (--) Start                              -> Starting
          + Reset-Error-Occurrence
    (--) GSL                                -> Continuing
          + Reset-Error-Occurrence
          + Call-Accept-GSL-Command
          + Raise-Exception-If-Error-Occurred
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event
    (--) End-Of-Script                      ->
          + Return-Ok-Feedback
          + Terminate-The-Thread
    (--) Exception                          ->
          + Return-Ok-Feedback
          + Terminate-The-Thread

Starting:
    (--) End-Of-Script                      -> Stepping
          + Return-Ok-Feedback
    (--) Script-Stack-Empty                 -> Stepping
          + Copy-Output-Buffer-To-Output
          + Return-Ok-Feedback
    (--) Continue                           -> Continuing
          + Call-Load-Initial-Script
          + Raise-Exception-If-Error-Occurred
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event
    (--) Next                               -> Stepping
          + Call-Load-Initial-Script
          + Raise-Exception-If-Error-Occurred
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Return-Ok-Feedback
    (--) GSL                                -> Continuing
          + Call-Accept-GSL-Command
          + Raise-Exception-If-Error-Occurred
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event
    (--) Call                               -> Stepping
          + Push-Script-Position
          + Get-Next-Script-Line
          + Call-Function-Call-Exception
          + Raise-Exception-If-Error-Occurred
          + Return-Ok-Feedback
    (--) Function-Call                      -> Continuing
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event
    (--) Exception                          -> Stepping
          + Reset-Error-Occurrence
          + Return-Ok-Feedback

Loading:
    (--) Error                              -> Continue-Loading
          + Flag-Error-Occurrence
          + Log-Load-Error
    (--) Done                               ->
          + Destroy-Script-Text
          + Start-New-Script
          + Dialog-Return

Continue-Loading:
    (--) Error                              -> 
          + Flag-Error-Occurrence
          + Log-Load-Error
    (--) Done                               ->
          + Destroy-Script-Text
          + Destroy-New-Script
          + Dialog-Return

Stepping:
    (--) Ok                                 ->
          + Return-Ok-Feedback
    (--) End-Of-Script                      -> 
          + Close-Script-File
          + Pop-Script-Position
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
    (--) Script-Stack-Empty                 ->
          + Copy-Output-Buffer-To-Output
          + Return-Ok-Feedback
    (--) Continue                           -> Continuing
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event
    (--) Next                               ->
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
    (--) GSL                                -> Continuing
          + Push-Script-Position
          + Call-Accept-GSL-Command
          + Raise-Exception-If-Error-Occurred
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event
    (--) Call                               -> 
          + Push-Script-Position
          + Get-Next-Script-Line
          + Call-Function-Call-Exception
          + Raise-Exception-If-Error-Occurred
          + Return-Ok-Feedback
    (--) Function-Call                      -> Continuing
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event
    (--) Exception                          -> 
          + Reset-Error-Occurrence
          + Return-Ok-Feedback

Continuing:
    (--) Ok                                 -> 
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event
    (--) End-Of-Script                      -> 
          + Close-Script-File
          + Pop-Script-Position
          + Get-Next-Script-Line
          + Call-Execute-Script-Line
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event
    (--) Script-Stack-Empty                 -> Stepping
          + Copy-Output-Buffer-To-Output
          + Return-Ok-Feedback
          + Finish-Exception-If-Execute-Full
    (--) Exception                          -> Stepping
          + Finish-Exception-If-Execute-Full
          + Reset-Error-Occurrence
          + Return-Ok-Feedback

Coding:
    (--) Line                               -> 
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Copy-Line-To-Output
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Comment                            -> 
          + After-Executing-Line
          + Dialog-Return
    (--) Define                             -> Evaluating-Define
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Generate-Operator-Type-Event
    (--) Macro                              -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Register-Macro-Or-Function
          + Skip-Block
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Function                           -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Register-Macro-Or-Function
          + Skip-Block
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Output                             -> 
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Close-Output-File
          + Open-File-For-Output
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Append                             -> 
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Close-Output-File
          + Open-File-For-Append
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) GSL                                -> 
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Push-Script-Position
          + Call-Prepare-To-Interpret-GSL
          + Raise-Exception-If-Error-Occurred
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Direct                             -> 
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Copy-Direct-To-Output
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) XML                                -> 
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-As
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-To 
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Before
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-After
          + Raise-Exception-If-Error-Occurred
          + Load-Source-XML
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Template                           -> 
          + After-Executing-Line
          + Dialog-Return
    (--) End-Template                       -> 
          + After-Executing-Line
          + Dialog-Return
    (--) Close                              -> 
          + Close-Output-File
          + After-Executing-Line
          + Dialog-Return
    (--) Sort                               -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-As
          + Raise-Exception-If-Error-Occurred
          + Call-Build-Iteration-List
          + Raise-Exception-If-Error-Occurred
          + Sort-XML-Items
          + Destroy-For-Block
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) For                                -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-As
          + Raise-Exception-If-Error-Occurred
          + Call-Build-Iteration-List
          + Raise-Exception-If-Error-Occurred
          + Initialise-For-Block
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) End-For                            -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Go-To-Top-Of-Block
          + Confirm-Scope-Name-Is-Correct
          + Raise-Exception-If-Error-Occurred
          + Free-Result-Root
          + Iterate-For-Loop
          + After-Executing-Line
          + Dialog-Return
    (--) Scope                              -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-As
          + Raise-Exception-If-Error-Occurred
          + Initialise-Scope-Block
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) End-Scope                          -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Confirm-Scope-Name-Is-Correct
          + Raise-Exception-If-Error-Occurred
          + Free-Result-Root
          + Close-The-Scope
          + After-Executing-Line
          + Dialog-Return
    (--) While                              -> 
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Skip-Block-If-Condition-False
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) End-While                          -> 
          + Create-Result-Root
          + Go-To-Top-Of-Block
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Skip-Block-If-Condition-False
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Next                               -> Iterating-Loop
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Go-To-Top-Of-Named-Block
          + Raise-Exception-If-Error-Occurred
          + Free-Result-Root
          + Generate-Node-Type-Event
    (--) Last                               -> Terminating-Loop
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Go-To-Top-Of-Named-Block
          + Raise-Exception-If-Error-Occurred
          + Free-Result-Root
          + Generate-Node-Type-Event
    (--) If                                 -> Testing-Condition
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Generate-Condition-Event
          + Raise-Exception-If-Error-Occurred
    (--) Else                               -> Skipping-Block
          + Skip-Block
          + Generate-Node-Type-Event
    (--) Elsif                              -> Skipping-Block
          + Skip-Block
          + Generate-Node-Type-Event
    (--) End-If                             -> 
          + After-Executing-Line
          + Dialog-Return
    (--) Echo                               -> 
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Echo-Text-To-Console
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Abort                              -> Stepping
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Echo-Text-To-Console
          + Free-Result-Root
          + Invoke-Abort-Handler
          + Return-Ok-Feedback
          + After-Executing-Line
          + Finish-Exception-If-Execute-Full
    (--) Copy                               -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-As
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-To 
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Before
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-After
          + Raise-Exception-If-Error-Occurred
          + Copy-Xml-Item
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Delete                             -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Delete-Xml-Item
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Move                               -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-As
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-To 
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Before
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-After
          + Raise-Exception-If-Error-Occurred
          + Move-Xml-Item
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Save                               -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-As
          + Raise-Exception-If-Error-Occurred
          + Save-Xml-File
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) New                                -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-As
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-To 
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Before
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-After
          + Raise-Exception-If-Error-Occurred
          + Initialise-New-Block
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) End-New                            -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Confirm-Scope-Name-Is-Correct
          + Raise-Exception-If-Error-Occurred
          + Free-Result-Root
          + Close-The-Scope
          + After-Executing-Line
          + Dialog-Return
    (--) Call                               -> 
          + Create-Result-Root
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Call
          + Raise-Exception-If-Error-Occurred
          + Pickup-Call-Result
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Evaluate-Call                      -> Evaluating-Call
          + Generate-Call-Type-Event
          + Raise-Exception-If-Error-Occurred
    (--) Return                             ->
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Save-Return-Value
          + Close-Macro-Block
          + Raise-Exception-If-Error-Occurred
          + Free-Result-Root
          + Pop-Script-Position
          + Pop-Current-Node
          + Dialog-Pop
          + After-Executing-Line
          + Dialog-Return
    (--) End-Macro                          -> 
          + Close-Macro-Block
          + Pop-Script-Position
          + Pop-Current-Node
          + Dialog-Pop
          + After-Executing-Line
          + Dialog-Return
    (--) End-Function                       -> 
          + Close-Macro-Block
          + Pop-Script-Position
          + Pop-Current-Node
          + Dialog-Pop
          + After-Executing-Line
          + Dialog-Return
    (--) Exception                          -> 
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return

Evaluating-Call:
    (--) Method                             -> Evaluating-Parameters
          + Get-First-Parameter
    (--) Macro                              -> Continuing
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Push-Current-Node
          + Push-Script-Position
          + Initialise-Macro-Block
          + Raise-Exception-If-Error-Occurred
          + Generate-Ok-Event

Evaluating:
    (--) Substitute                         ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-As
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Substitute-Node
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Literal                            ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Literal-Node
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Number                             ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Number-Node
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Operand                            ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Parse-Op1-To-Operand-Exception
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Operand
          + Raise-Exception-If-Error-Occurred
          + Pickup-Operand-Result
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Symbol                             ->
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Symbol-Node
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Member                             ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Member-Node
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Call                               -> 
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Call
          + Raise-Exception-If-Error-Occurred
          + Pickup-Call-Result
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Evaluate-Call                      -> Evaluating-Call
          + Push-Current-Node
          + Generate-Call-Type-Event
          + Raise-Exception-If-Error-Occurred
          + Pop-Current-Node
    (--) Operator                           -> Evaluating-Operator
          + Generate-Operator-Type-Event
    (--) Text                               ->
          + Evaluate-Text-Node
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Exception                          -> 
          + Pop-Current-Node
          + Dialog-Return

Evaluating-Define:
    (--) Undefined                          -> 
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Define-Symbol-As-Node-Op2-Result
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Arithmetic                         -> 
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Prepare-Define-Expression
          + Evaluate-Arithmetic-Operator
          + Define-Symbol-As-Node-Result
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Default                            ->
          + Raise-Exception-If-Symbol-Is-Defined
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Define-Symbol-As-Node-Op2-Result
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Exception                          -> 
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return

Building-List:
    (--) Ok                                 ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Reject-If-Where-Clause-False
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Save-Sort-Key-Value
          + Get-Next-Child
          + Raise-Exception-If-Error-Occurred
    (--) Reject                             ->
          + Kill-Scope-Item
          + Get-Next-Child
          + Raise-Exception-If-Error-Occurred
    (--) None                               ->
          + Dialog-Return

! JS 2001/02/18 - for 'count' alias
Building-Count-List:
    (--) Ok                                 ->
          + Open-Count-Alias-Block
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Close-Count-Alias-Block
          + Reject-If-Where-Clause-False
          + Kill-Scope-Item
          + Get-Next-Child
          + Raise-Exception-If-Error-Occurred
    (--) Reject                             ->
          + Kill-Scope-Item
          + Get-Next-Child
          + Raise-Exception-If-Error-Occurred
    (--) None                               ->
          + Dialog-Return

Iterating-Loop:
    (--) For                                -> 
          + Iterate-For-Loop
          + After-Executing-Line
          + Dialog-Return
    (--) While                              -> 
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Skip-Block-If-Condition-False
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) Exception                          -> 
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return

Terminating-Loop:
    (--) For                                ->
          + Destroy-For-Block
          + Skip-Block
          + After-Executing-Line
          + Dialog-Return
    (--) While                              ->
          + Skip-Block
          + After-Executing-Line
          + Dialog-Return

Testing-Condition:
    (--) True                               -> 
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return
    (--) False                              -> Finding-Block
          + Free-Result-Root
          + Skip-Block
          + Generate-Node-Type-Event

Finding-Block:
    (--) Elsif                              -> Testing-Condition
          + Create-Result-Root
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Generate-Condition-Event
          + Raise-Exception-If-Error-Occurred
    (--) Else                               ->
          + After-Executing-Line
          + Dialog-Return
    (--) End-If                             ->
          + After-Executing-Line
          + Dialog-Return
    (--) Exception                          -> 
          + Free-Result-Root
          + After-Executing-Line
          + Dialog-Return

Skipping-Block:
    (--) Elsif                              ->
          + Skip-Block
          + Generate-Node-Type-Event
    (--) Else                               ->
          + Skip-Block
          + Generate-Node-Type-Event
    (--) End-If                             ->
          + After-Executing-Line
          + Dialog-Return

Evaluating-Parameters:
    (--) Value                              ->
          + Call-Evaluate-The-Parameter
          + Raise-Exception-If-Error-Occurred
          + Get-Next-Parameter
    (--) Reference                          ->
          + Call-Evaluate-The-Reference-Parameter
          + Raise-Exception-If-Error-Occurred
          + Get-Next-Parameter
    (--) Expression                         ->
          + Insert-Null-Result-Node
          + Get-Next-Parameter
    (--) Simple-Scope                       ->
          + Call-Evaluate-The-Simple-Scope-Parameter
          + Raise-Exception-If-Error-Occurred
          + Get-Next-Parameter
    (--) Extended-Scope                     ->
          + Call-Evaluate-The-Extended-Scope-Parameter
          + Raise-Exception-If-Error-Occurred
          + Get-Next-Parameter
    (--) None                               -> After-Calling-Method
          + Evaluate-Call-Method-Node
          + Raise-Exception-If-Error-Occurred

After-Calling-Method:
    (--) Result                             ->
          + Dialog-Return

Evaluating-Simple-Scope:
    (--) Literal                            ->
          + Report-Illegal-Scope-Parameter
          + Dialog-Return
    (--) Call                               -> 
          + Report-Illegal-Scope-Parameter
          + Dialog-Return
    (--) Operator                           -> 
          + Report-Illegal-Scope-Parameter
          + Dialog-Return
    (--) Operand                            ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Copy-Result-From-Op1
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Symbol                             ->
          + Node-Scope-Must-Be-Null
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Copy-Result-From-Name
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Number                             ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Number-Node
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Operator                           -> Evaluating-Operator
          + Generate-Operator-Type-Event

Evaluating-Extended-Scope:
    (--) Literal                            ->
          + Report-Illegal-Scope-Parameter
          + Dialog-Return
    (--) Call                               -> 
          + Report-Illegal-Scope-Parameter
          + Dialog-Return
    (--) Operator                           -> 
          + Report-Illegal-Scope-Parameter
          + Dialog-Return
    (--) Operand                            ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Symbol                             ->
          + One-Of-Scope-Name-Must-Be-Null
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Copy-Name-To-Scope-If-Defined
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Number                             ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Number-Node
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Operator                           -> Evaluating-Operator
          + Generate-Operator-Type-Event

Evaluating-Reference:
    (--) Operand                            ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Symbol                             ->
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return

Parsing-Expression:
    (--) Ok                                 ->
          + Save-Parse-Result-To-Operand
          + Dialog-Return
    (--) Error                              -> 
          + Flag-Error-Occurrence
          + Log-Parse-Error
          + Finish-Exception-If-Execute-Full
          + Free-Result-Root
          + Dialog-Return

Evaluating-Operator:
    (--) Undefined                          ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Concatenate-Operands
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Arithmetic                         ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Arithmetic-Operator
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Default                            ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Return-Op1-If-Defined
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Return-Op2
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Comparison                         ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Comparison-Operator
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Safe-Comparison                    ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Return-False-If-Op1-Undefined
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Return-False-If-Op2-Undefined
          + Evaluate-Comparison-Operator
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Or                                 ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Return-True-If-Op1-True
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Arithmetic-Operator
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) And                                ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Return-False-If-Op1-False
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Evaluate-Arithmetic-Operator
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Next-Arg                           ->
          + Call-Evaluate-Op1
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Op2
          + Raise-Exception-If-Error-Occurred
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Dialog-Return                      ->
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Exception                          -> 
          + Pop-Current-Node
          + Dialog-Return

Defaults:
    (--) Execute-Line                       -> Coding
          + Check-For-Shutdown-Request
          + Before-Executing-Line
          + Generate-Node-Type-Event
    (--) Load-Script-File                   -> Loading
          + Request-Load-Script-File
    (--) Load-Script-Text                   -> Loading
          + Request-Load-Script-Text
    (--) Evaluate                           -> Evaluating
          + Check-For-Shutdown-Request
          + Push-Current-Node
          + Begin-Evaluating-Node
          + Generate-Node-Type-Event
    (--) Dialog-Return                      ->
          + Finish-Evaluating-Node
          + Pop-Current-Node
          + Dialog-Return
    (--) Parse-Op1-To-Operand               -> Parsing-Expression
          + Parse-Op1-To-Operand
    (--) Evaluate-Reference                 -> Evaluating-Reference
          + Push-Current-Node
          + Begin-Evaluating-Node
          + Generate-Node-Type-Event
    (--) Evaluate-Simple-Scope              -> Evaluating-Simple-Scope
          + Push-Current-Node
          + Begin-Evaluating-Node
          + Generate-Node-Type-Event
    (--) Evaluate-Extended-Scope            -> Evaluating-Extended-Scope
          + Push-Current-Node
          + Begin-Evaluating-Node
          + Generate-Node-Type-Event
    (--) Evaluate-Count                     ->
          + Build-Fake-For-Node
          + Call-Evaluate-Scope
          + Raise-Exception-If-Error-Occurred
          + Call-Evaluate-Name
          + Raise-Exception-If-Error-Occurred
! JS 2001/02/18 - for 'count' alias
!         + Call-Build-Iteration-List
          + Call-Build-Count-Iteration-List
          + Raise-Exception-If-Error-Occurred
          + Save-Total-As-Result
          + Destroy-For-Block
          + Destroy-Fake-For-Node
          + Dialog-Return
    (--) Build-List                         -> Building-List
          + Get-First-Child
          + Raise-Exception-If-Error-Occurred
! JS 2001/02/18 - for 'count' alias
    (--) Build-Count-List                   -> Building-Count-List
          + Get-First-Child
          + Raise-Exception-If-Error-Occurred
    (--) Script-Stack-Empty                 -> 
          + Signal-Internal-Error
          + Terminate-The-Thread
    (--) Error                              -> 
          + Flag-Error-Occurrence
          + Dialog-Return
    (--) Exception                          -> 
          + Dialog-Return
    (--) Finish                             -> 
          + Terminate-The-Thread
    (--) Fatal                              ->
          + Terminate-The-Thread
    (--) Anomaly                            ->
          + Signal-Internal-Error
          + Terminate-The-Thread
    (--) Shutdown                           ->
          + Terminate-The-Thread
