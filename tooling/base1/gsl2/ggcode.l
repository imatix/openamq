!----------------------------------------------------------------------------*
!                                                                            *
!   ggcode.l - Code generator                                                *
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

After-Init:
    (--) Ok                                 -> Have-Script-Line
          + Get-Next-Script-Line

Have-Script-Line:
    (--) Comment                            -> Have-Script-Line
          + Get-Next-Script-Line
    (--) Define                             -> Have-Script-Line
          + Store-Symbol-Definition
          + Get-Next-Script-Line
    (--) Macro                              -> Have-Script-Line
          + Store-Macro-Definition
          + Skip-Macro-Body
          + Get-Next-Script-Line
    (--) Function                           -> Have-Script-Line
          + Store-Function-Definition
          + Skip-Function-Body
          + Get-Next-Script-Line
    (--) Output                             -> Have-Script-Line
          + Close-Output-File
          + Open-File-For-Output
          + Get-Next-Script-Line
    (--) Append                             -> Have-Script-Line
          + Close-Output-File
          + Open-File-For-Append
          + Get-Next-Script-Line
    (--) GSL                                -> Have-Script-Line
          + Push-Script-Position
          + Prepare-To-Interpret-GSL
          + Get-Next-Script-Line
    (--) Direct                             -> Have-Script-Line
          + Copy-Direct-To-Output
          + Get-Next-Script-Line
    (--) XML                                -> Have-Script-Line
          + Load-Source-XML
          + Get-Next-Script-Line
    (--) Javascript                         -> Have-Script-Line
          + Load-Source-Javascript
          + Get-Next-Script-Line
    (--) Call                               -> Have-Script-Line
          + Push-Script-Position
          + Initialise-Macro-Block
          + Get-Next-Script-Line
    (--) End-Macro                          -> Have-Script-Line
          + Close-Macro-Block
          + Pop-Script-Position
          + Get-Next-Script-Line
    (--) End-Function                       -> Have-Script-Line
          + Close-Function-Block
          + Pop-Script-Position
          + Get-Next-Script-Line
    (--) Template                           -> Have-Script-Line
          + Push-Template-Control
          + Get-Next-Script-Line
    (--) End-Template                       -> Have-Script-Line
          + Pop-Template-Control
          + Get-Next-Script-Line
    (--) Close                              -> Have-Script-Line
          + Close-Output-File
          + Get-Next-Script-Line
    (--) For                                -> Have-Script-Line
          + Initialise-For-Block
          + Get-Next-Script-Line
    (--) End-For                            -> Have-Script-Line
          + Iterate-For-Block
          + Get-Next-Script-Line
    (--) While                              -> Have-Script-Line
          + Initialise-While-Block
          + Get-Next-Script-Line
    (--) End-While                          -> Have-Script-Line
          + Iterate-While-Block
          + Get-Next-Script-Line
    (--) Next                               -> Have-Script-Line
          + Iterate-For-Or-While-Block
          + Get-Next-Script-Line
    (--) Last                               -> Have-Script-Line
          + Terminate-For-Or-While-Block
          + Get-Next-Script-Line
    (--) If                                 -> Have-Script-Line
          + Skip-If-Block-If-False
          + Get-Next-Script-Line
    (--) Else                               -> Have-Script-Line
          + Skip-Else-Block-Always
          + Get-Next-Script-Line
    (--) Elsif                              -> Have-Script-Line
          + Skip-Elsif-Block-Always
          + Get-Next-Script-Line
    (--) End-If                             -> Have-Script-Line
          + Close-If-Block
          + Get-Next-Script-Line
    (--) Echo                               -> Have-Script-Line
          + Echo-Text-To-Console
          + Get-Next-Script-Line
    (--) Abort                              ->
          + Echo-Text-To-Console
          + Return-Abort-Feedback
          + Terminate-The-Program
    (--) Text                               -> Have-Script-Line
          + Copy-Line-To-Output
          + Get-Next-Script-Line
    (--) Copy                               -> Have-Script-Line
          + Copy-Xml-Item
          + Get-Next-Script-Line
    (--) Delete                             -> Have-Script-Line
          + Delete-Xml-Item
          + Get-Next-Script-Line
    (--) Move                               -> Have-Script-Line
          + Move-Xml-Item
          + Get-Next-Script-Line
    (--) Save                               -> Have-Script-Line
          + Save-Xml-File
          + Get-Next-Script-Line
    (--) New                                -> Have-Script-Line
          + Initialise-New-Block
          + Get-Next-Script-Line
    (--) End-New                            -> Have-Script-Line
          + Close-New-Block
          + Get-Next-Script-Line
    (--) Sort                               -> Have-Script-Line
          + Sort-XML-Items
          + Get-Next-Script-Line
    (--) End-Of-Script                      -> Have-Script-Line
          + Verify-All-Controls-Closed
          + Close-Script-File
          + Pop-Script-Position
          + Get-Next-Script-Line
    (--) Script-Stack-Empty                 ->
          + Terminate-The-Program

Defaults:
    (--) Anomaly                            ->
          + Return-Anomaly-Feedback
          + Terminate-The-Program
